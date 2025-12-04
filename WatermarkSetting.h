#ifndef __WATERMARKSETTING_H__
#define __WATERMARKSETTING_H__

#include "WatermarkInfo.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class WatermarkSettingClass;
};
QT_END_NAMESPACE

class WatermarkSetting : public QWidget
{
    Q_OBJECT

public:
    WatermarkSetting(const WaterMarkInfo& watermarkInfo, QWidget* parent = nullptr);
    ~WatermarkSetting();

    WaterMarkInfo watermarkInfo() const { return m_watermarkInfo; }

    Q_SIGNAL void applyWatermark(const WaterMarkInfo& watermarkInfo);
    Q_SIGNAL void cancelApply();
    Q_SIGNAL void exportPDF();

private:
    void updatePreview();
    void slotApply();

private:
    Ui::WatermarkSettingClass* ui;
    WaterMarkInfo m_watermarkInfo;
    bool m_applied = false;
};

#endif  // __WATERMARKSETTING_H__
