#ifndef __WATERMARKPREVIEW_H__
#define __WATERMARKPREVIEW_H__

#include "WatermarkInfo.h"
#include <QWidget>

class WatermarkPreview : public QWidget
{
    Q_OBJECT

public:
    WatermarkPreview(QWidget* parent);
    ~WatermarkPreview();

    void setWatermarkInfo(const WaterMarkInfo& info);

protected:
    void paintEvent(QPaintEvent* event) override;
    int calculateOptimalFontSize(const QString& text);

private:
    WaterMarkInfo m_watermarkInfo;
};

#endif  // __WATERMARKPREVIEW_H__
