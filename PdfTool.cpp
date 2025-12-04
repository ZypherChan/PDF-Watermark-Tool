#include "PdfTool.h"
#include "Thumbnail.h"
#include "DpiManager.h"

#include "ttfsubset/subttf.h"

#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QFileInfo>
#include <QScrollBar>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMessageBox>
#include <QFontDatabase>
#include <QProgressDialog>

#include <fstream>

#define DEFAULT_FONT "NotoSansCJKsc-Bold.ttf"

bool geFontFilePathByQFont(const QFont& font, QString& fileName);

struct FileWriteContext : public FPDF_FILEWRITE
{
    std::ofstream* stream;
};

PdfTool::PdfTool(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow()), m_doc(nullptr)
{
    ui->setupUi(this);
    DpiManager::instance().adaptHDR(this);
    init();

    QString fontPath = QDir(QApplication::applicationDirPath() + "/fonts").filePath(DEFAULT_FONT);
    if(QFile::exists(fontPath))
        QFontDatabase::addApplicationFont(fontPath);

    connect(ui->btn_pageup, &QPushButton::clicked, this, &PdfTool::slotPageUp);
    connect(ui->btn_pagedown, &QPushButton::clicked, this, &PdfTool::slotPageDown);
    connect(ui->actionOpen, &QAction::triggered, this, &PdfTool::slotOpenPDF);
    connect(ui->actionSave, &QAction::triggered, this, &PdfTool::slotSave);
    connect(ui->actionSaveAs, &QAction::triggered, this, &PdfTool::slotSaveAs);
    connect(ui->actionSetting, &QAction::triggered, this, &PdfTool::slotAddWatermark);
}

PdfTool::~PdfTool()
{
    delete ui;
    if(m_doc) FPDF_CloseDocument(m_doc);
    FPDF_DestroyLibrary();
}

void PdfTool::init()
{
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->actionSetting->setEnabled(false);

    m_watermarkInfo.angle = 45.0;
    m_watermarkInfo.fontSize = 20;
    m_watermarkInfo.opacity = 0.5;
    m_watermarkInfo.family = "Microsoft YaHei";
    m_watermarkInfo.color = QColor(Qt::gray);
    m_watermarkInfo.text = QString::fromLocal8Bit("PDF - Ë®Ó¡¹¤¾ß");
    m_watermarkInfo.repeatRows = 4;
    m_watermarkInfo.repeatCols = 4;

    FPDF_InitLibrary();
}

void PdfTool::initThumbnail()
{
    QVBoxLayout* layout = (QVBoxLayout*)ui->widget_thumbnail->layout();
    if(layout)
    {
        int count = layout->count();
        for(int i = 0; i < count; i++)
        {
            QLayoutItem* item = layout->takeAt(0);
            if(item)
            {
                if(QWidget* widget = item->widget())
                {
                    widget->deleteLater();
                }
                delete item;
            }
        }
    }
    else
    {
        layout = new QVBoxLayout(ui->widget_thumbnail);
        ui->widget_thumbnail->setLayout(layout);
    }

    QProgressDialog progressDialog(this);
    progressDialog.setWindowTitle(tr("Open PDF"));
    progressDialog.setLabelText(tr("Parse PDF..."));
    progressDialog.setRange(0, m_pageCount);
    progressDialog.setCancelButton(nullptr);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    progressDialog.setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    progressDialog.setWindowFlag(Qt::WindowCloseButtonHint, false);
    progressDialog.setMinimumDuration(400);

    for(int i = 0; i < m_pageCount; i++)
    {
        FPDF_PAGE page = FPDF_LoadPage(m_doc, i);
        float width = FPDF_GetPageWidthF(page);
        float height = FPDF_GetPageHeightF(page);

        int thumbWidth = POINT2PIXEL(width * 0.2f);
        int thumbHeight = POINT2PIXEL(height * 0.2f);

        QImage image(thumbWidth, thumbHeight, QImage::Format_RGBA8888);
        image.fill(0xFFFFFFFF);

        FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(), FPDFBitmap_BGRA,
                                                 image.scanLine(0), image.bytesPerLine());
        FPDF_RenderPageBitmap(bitmap, page, 0, 0, image.width(), image.height(), 0,
                              FPDF_REVERSE_BYTE_ORDER | FPDF_ANNOT);
        FPDFBitmap_Destroy(bitmap);

        Thumbnail* thumb = new Thumbnail(image, i);
        connect(thumb, &Thumbnail::clicked, this, [this](int index) {
            m_currentPageIndex = index;
            renderPage();
        });
        layout->addWidget(thumb);

        FPDF_ClosePage(page);
        progressDialog.setValue(i + 1);
    }
    ui->scrollArea->verticalScrollBar()->setValue(0);
}

QImage PdfTool::addWatermarkPreview(const QImage& image, const QString& watermarkText)
{
    QImage resultImage = image.copy();
    if(m_watermarkInfo.text.isEmpty() || m_watermarkInfo.opacity < 1e-6) return resultImage;

    QPainter painter(&resultImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(m_watermarkInfo.color));
    painter.setFont(QFont(m_watermarkInfo.family, m_watermarkInfo.fontSize));
    painter.setOpacity(m_watermarkInfo.opacity);

    QFontMetrics fm(painter.font());
    QRect textRect = fm.boundingRect(watermarkText);

    int cols = m_watermarkInfo.repeatCols;
    int rows = m_watermarkInfo.repeatRows;

    const int spacingX = resultImage.width() / cols;
    const int spacingY = resultImage.height() / rows;

    for(int row = 0; row < rows; row++)
    {
        for(int col = 0; col < cols; col++)
        {
            painter.save();

            int x = col * spacingX + spacingX / 2;
            int y = row * spacingY + spacingY / 2;

            painter.translate(x, y);
            painter.rotate(-m_watermarkInfo.angle);

            QRect watermarkRect(-textRect.width() / 2, -textRect.height() / 2, textRect.width(),
                                textRect.height());

            painter.drawText(watermarkRect, Qt::AlignCenter, watermarkText);
            painter.restore();
        }
    }

    return resultImage;
}

bool PdfTool::addWatermark(FPDF_DOCUMENT doc)
{
    if(m_watermarkInfo.text.isEmpty() || m_watermarkInfo.opacity < 1e-6) return true;
    QString fontPath;
    FPDF_FONT font = nullptr;
    if(!geFontFilePathByQFont(QFont(m_watermarkInfo.family), fontPath))
    {
        fontPath = QDir(QApplication::applicationDirPath() + "/fonts").filePath(DEFAULT_FONT);
        if(!QFile::exists(fontPath))
        {  // Use standard font if custom font file not found
            font = FPDFText_LoadStandardFont(doc, "Helvetica-Bold");
        }
        QMessageBox::information(this, tr("Add Watermark"), tr("Using the default font due to missing font file"));
    }
    
    if(!font && !fontPath.isEmpty())
    {
        std::string fontFilePath = fontPath.toLocal8Bit().constData();
        wchar_t* wcharArray = new wchar_t[m_watermarkInfo.text.size() + 1];
        int length = m_watermarkInfo.text.toWCharArray(wcharArray);
        wcharArray[length] = L'\0';
        std::vector<uint8_t> fontData = subsetTrueTypeFontData(fontFilePath.c_str(), wcharArray);
        if(fontData.empty())
        {
            QMessageBox::warning(this, tr("Add Watermark"), tr("Failed to subset font"));
            delete[] wcharArray;
            return false;
        }
        font = FPDFText_LoadFont(doc, fontData.data(), static_cast<uint32_t>(fontData.size()),
                                 FPDF_FONT_TRUETYPE, TRUE);
    }
    if(!font)
    {
        QMessageBox::warning(this, tr("Add Watermark"), tr("Failed to load font for watermark."));
        return false;
    }

    float rad = static_cast<float>(m_watermarkInfo.angle * 3.1415926535 / 180.0);
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    // Get text measurements (pixels) with Qt and convert to PDF points
    QFont qfont(m_watermarkInfo.family, m_watermarkInfo.fontSize);
    QFontMetrics fm(qfont);
    QRect textRect = fm.boundingRect(m_watermarkInfo.text);
    float textWidth = PIXEL2POINT(textRect.width());
    float textHeight = PIXEL2POINT(textRect.height());
    float descent = PIXEL2POINT(fm.descent());

    // We replace the original text h/2 with (text h/2 - descent pt) in the matrix to put the baseline in the
    // center
    float halfHeightMinusDescent = textHeight * 0.5f - descent;

    int rows = m_watermarkInfo.repeatRows;
    int cols = m_watermarkInfo.repeatCols;
    int color = m_watermarkInfo.color.rgba();
    FPDF_WIDESTRING text = (FPDF_WIDESTRING)m_watermarkInfo.text.utf16();

    QProgressDialog progressDialog(this);
    progressDialog.setWindowTitle(tr("Export PDF"));
    progressDialog.setLabelText(tr("Adding watermark to PDF pages..."));
    progressDialog.setRange(0, m_pageCount);
    progressDialog.setCancelButton(nullptr);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    progressDialog.setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    progressDialog.setWindowFlag(Qt::WindowCloseButtonHint, false);
    progressDialog.setMinimumDuration(400);

    for(int i = 0; i < m_pageCount; i++)
    {
        FPDF_PAGE page = FPDF_LoadPage(doc, i);
        if(!page)
        {
            progressDialog.cancel();
            QMessageBox::warning(this, tr("Add Watermark"), tr("Failed to load page"));
            return false;
        }

        float pageWidth = FPDF_GetPageWidthF(page);
        float pageHeight = FPDF_GetPageHeightF(page);

        const int spacingX = pageWidth / cols;
        const int spacingY = pageHeight / rows;

        for(int row = 0; row < rows; row++)
        {
            for(int col = 0; col < cols; col++)
            {
                FPDF_PAGEOBJECT textObj = FPDFPageObj_CreateTextObj(doc, font, m_watermarkInfo.fontSize);
                if(!textObj) 
                    continue;

                FPDFText_SetText(textObj, text);
                FPDFPageObj_SetFillColor(textObj, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF,
                                         static_cast<int>(m_watermarkInfo.opacity * 255));

                float cx = col * spacingX + spacingX / 2.0f;
                // PDF Y-axis origin is at the bottom left corner, adjust to be consistent with Qt
                float cy = pageHeight - (row * spacingY + spacingY / 2.0f);

                // Rotation/translation matrix, note the use of half_h_minus_descent to correct baseline
                // offset
                FS_MATRIX matrix = { cosA,
                                     sinA,
                                     -sinA,
                                     cosA,
                                     cx - (textWidth / 2.0f) * cosA + (halfHeightMinusDescent)*sinA,
                                     cy - (textWidth / 2.0f) * sinA - (halfHeightMinusDescent)*cosA };
                FPDFPageObj_SetMatrix(textObj, &matrix);

                FPDFPage_InsertObject(page, textObj);
            }
        }
        FPDFPage_GenerateContent(page);
        FPDF_ClosePage(page);
        progressDialog.setValue(i + 1);
    }
    FPDFFont_Close(font);

    return true;
}

void PdfTool::save(const QString& filePath)
{
    if(m_doc)
    {
        auto doc = FPDF_LoadDocument(m_pdfFilePath.toStdString().c_str(), nullptr);
        if(!doc)
        {
            QMessageBox::warning(this, tr("Save PDF"), tr("Failed to reload original PDF document."));
            return;
        }

        if(m_watermarkInfo.enabled && !addWatermark(doc)) return;
        std::ofstream outputFile(filePath.toLocal8Bit().constData(), std::ios::binary);
        if(!outputFile.is_open())
        {
            QMessageBox::warning(this, tr("Save PDF"), tr("Failed to open output file for writing."));
            return;
        }
        FileWriteContext fileWrite;
        fileWrite.version = 1;
        fileWrite.stream = &outputFile;
        fileWrite.WriteBlock = [](FPDF_FILEWRITE* pThis, const void* buffer, unsigned long size) {
            std::ofstream* file = static_cast<FileWriteContext*>(pThis)->stream;
            if(file && file->is_open())
            {
                file->write(static_cast<const char*>(buffer), size);
                return static_cast<int>(size);
            }
            return 0;
        };

        bool isok = FPDF_SaveAsCopy(doc, &fileWrite, 0);
        outputFile.close();
        if(!isok)
            QMessageBox::warning(this, tr("Save PDF"), tr("Failed to save PDF document."));
        else
            QMessageBox::information(this, tr("Save PDF"), tr("PDF document saved successfully."));
    }
}

void PdfTool::slotOpenPDF()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open PDF"), "", "PDF Files (*.pdf)");
    if(filePath.isEmpty()) return;

    if(m_doc) FPDF_CloseDocument(m_doc);

    m_doc = FPDF_LoadDocument(filePath.toStdString().c_str(), nullptr);
    if(!m_doc)
    {
        auto error = FPDF_GetLastError();
        qDebug() << "Failed to open PDF document. Error code:" << error;
        QMessageBox::warning(this, tr("Open PDF"), tr("Failed to open PDF document."));
        return;
    }
    if(ui->stackedWidget->currentIndex() != 1) ui->stackedWidget->setCurrentIndex(1);

    m_pageImageCache.clear();
    ui->actionSetting->setEnabled(true);
    m_pdfFilePath = filePath;
    m_watermarkInfo.enabled = false;

    QString title = tr("PDF Watermark Tool - %1").arg(QFileInfo(filePath).fileName());
    setWindowTitle(title);

    m_pageCount = FPDF_GetPageCount(m_doc);
    m_currentPageIndex = 0;
    renderPage();
    initThumbnail();
}

void PdfTool::slotSave()
{
    save(m_pdfFilePath);
}

void PdfTool::slotSaveAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save PDF As"), "", "PDF Files (*.pdf)");
    if(filePath.isEmpty()) return;

    save(filePath);
}

void PdfTool::slotAddWatermark()
{
    WatermarkSetting* setting = new WatermarkSetting(m_watermarkInfo, this);
    setting->setWindowFlag(Qt::Window);
    setting->setAttribute(Qt::WA_DeleteOnClose);

    connect(setting, &WatermarkSetting::applyWatermark, this, [this, setting](const WaterMarkInfo& info) {
        m_watermarkInfo = info;
        m_watermarkInfo.enabled = true;
        renderPage();
    });
    connect(setting, &WatermarkSetting::cancelApply, this, [this, setting]() {
        m_watermarkInfo.enabled = false;
        renderPage();
    });
    connect(setting, &WatermarkSetting::exportPDF, this, [this, setting]() {
        QString fileName = QFileInfo(m_pdfFilePath).fileName();
        fileName.replace(".pdf", "_watermarked.pdf");
        QString filePath = QFileDialog::getSaveFileName(this, tr("Export PDF with Watermark"), fileName,
                                                        "PDF Files (*.pdf)");
        if(filePath.isEmpty()) return;
        m_watermarkInfo = setting->watermarkInfo();
        m_watermarkInfo.enabled = true;
        save(filePath);
    });

    setting->show();
}

void PdfTool::slotPageUp()
{
    if(m_currentPageIndex <= 0) return;

    m_currentPageIndex--;
    renderPage();
}

void PdfTool::slotPageDown()
{
    if(m_currentPageIndex >= m_pageCount - 1) return;
    m_currentPageIndex++;
    renderPage();
}

void PdfTool::renderPage()
{
    if(!m_pageImageCache.contains(m_currentPageIndex))
    {
        FPDF_PAGE page = FPDF_LoadPage(m_doc, m_currentPageIndex);
        float width = FPDF_GetPageWidthF(page);
        float height = FPDF_GetPageHeightF(page);

        QImage image(POINT2PIXEL(width), POINT2PIXEL(height), QImage::Format_RGBA8888);
        image.fill(0xFFFFFFFF);

        FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(), FPDFBitmap_BGRA,
                                                 image.scanLine(0), image.bytesPerLine());
        FPDF_RenderPageBitmap(bitmap, page, 0, 0, image.width(), image.height(), 0,
                              FPDF_REVERSE_BYTE_ORDER | FPDF_ANNOT);
        FPDFBitmap_Destroy(bitmap);

        m_pageImageCache.insert(m_currentPageIndex, image);
        FPDF_ClosePage(page);
    }

    m_currentImage = m_pageImageCache[m_currentPageIndex];
    if(m_watermarkInfo.enabled) m_currentImage = addWatermarkPreview(m_currentImage, m_watermarkInfo.text);

    ui->label->setPixmap(QPixmap::fromImage(m_currentImage)
                             .scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QString indexText = QString("%1 / %2").arg(m_currentPageIndex + 1).arg(m_pageCount);
    ui->label_pageIndex->setText(indexText);
}

void PdfTool::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

void PdfTool::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if(!m_currentImage.isNull())
    {
        ui->label->setPixmap(QPixmap::fromImage(m_currentImage)
                                 .scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
