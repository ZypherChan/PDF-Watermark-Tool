#pragma once

#include <QtWidgets/QWidget>
#include "ui_PdfTool.h"

#include "WatermarkSetting.h"

#include ".\pdfium\include\fpdf_doc.h"
#include ".\pdfium\include\fpdfview.h"
#include ".\pdfium\include\fpdf_thumbnail.h"
#include ".\pdfium\include\fpdf_save.h"
#include ".\pdfium\include\fpdf_edit.h"

#include <QImage>
#include <QVector>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
};
QT_END_NAMESPACE

class PdfTool : public QMainWindow
{
    Q_OBJECT

public:
    PdfTool(QWidget* parent = nullptr);
    ~PdfTool();

private:
    void init();
    void initThumbnail();
    void renderPage();

    QImage addWatermarkPreview(const QImage& image, const QString& watermarkText);
    bool addWatermark(FPDF_DOCUMENT doc);
    void save(const QString& filePath);

    void slotOpenPDF();
    void slotSave();
    void slotSaveAs();
    void slotAddWatermark();

    void slotPageUp();
    void slotPageDown();

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::MainWindow* ui;
    FPDF_DOCUMENT m_doc;
    QImage m_currentImage;
    int m_currentPageIndex = 0;
    int m_pageCount;
    QMap<int, QImage> m_pageImageCache;
    QString m_pdfFilePath;
    WaterMarkInfo m_watermarkInfo;
};
