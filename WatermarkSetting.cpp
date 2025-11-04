#include "WatermarkSetting.h"
#include "DpiManager.h"

WatermarkSetting::WatermarkSetting(const WaterMarkInfo& watermarkInfo, QWidget* parent /*= Q_NULLPTR*/)
    : QWidget(parent), ui(new Ui::WatermarkSettingClass()), m_watermarkInfo(watermarkInfo)
{
    ui->setupUi(this);
    DpiManager::instance().adaptHDR(this);
    setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);

    ui->lineEdit_text->setText(m_watermarkInfo.text);
    ui->spinBox_fontSize->setValue(static_cast<int>(m_watermarkInfo.fontSize));
    ui->spinBox_angle->setValue(static_cast<int>(m_watermarkInfo.angle));
    ui->doubleSpinBox_opacity->setValue(m_watermarkInfo.opacity);
    ui->spinBox_rowCount->setValue(m_watermarkInfo.repeatRows);
    ui->spinBox_colCount->setValue(m_watermarkInfo.repeatCols);

    connect(ui->btn_apply, &QPushButton::clicked, this, &WatermarkSetting::slotApply);
    connect(ui->btn_cancel, &QPushButton::clicked, this, [this]() {
        m_applied = false;
        emit cancelApply();
        });
    connect(ui->btn_export, &QPushButton::clicked, this, &WatermarkSetting::exportPDF);
    connect(ui->lineEdit_text, &QLineEdit::textChanged, this, &WatermarkSetting::updatePreview);
    connect(ui->spinBox_fontSize, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &WatermarkSetting::updatePreview);
    connect(ui->spinBox_angle, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &WatermarkSetting::updatePreview);
    connect(ui->doubleSpinBox_opacity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &WatermarkSetting::updatePreview);
    connect(ui->spinBox_rowCount, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &WatermarkSetting::updatePreview);
    connect(ui->spinBox_colCount, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &WatermarkSetting::updatePreview);

    updatePreview();
}

void WatermarkSetting::updatePreview()
{
    m_watermarkInfo.text = ui->lineEdit_text->text();
    m_watermarkInfo.fontSize = ui->spinBox_fontSize->value();
    m_watermarkInfo.angle = ui->spinBox_angle->value();
    m_watermarkInfo.opacity = ui->doubleSpinBox_opacity->value();
    ui->widget_preview->setWatermarkInfo(m_watermarkInfo);

    if(m_applied) slotApply();
}

void WatermarkSetting::slotApply()
{
    m_applied = true;
    m_watermarkInfo.text = ui->lineEdit_text->text();
    m_watermarkInfo.fontSize = ui->spinBox_fontSize->value();
    m_watermarkInfo.angle = ui->spinBox_angle->value();
    m_watermarkInfo.opacity = ui->doubleSpinBox_opacity->value();
    m_watermarkInfo.repeatRows = ui->spinBox_rowCount->value();
    m_watermarkInfo.repeatCols = ui->spinBox_colCount->value();
    emit applyWatermark(m_watermarkInfo);
}

WatermarkSetting::~WatermarkSetting()
{
    delete ui;
}
