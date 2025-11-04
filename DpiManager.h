#ifndef __DPIMANAGER_H__
#define __DPIMANAGER_H__

#include <QWidget>

#define POINT2PIXEL(value) DpiManager::instance().pointSize2Pixel(value)
#define PIXEL2POINT(value) DpiManager::instance().pixel2PointSize(value)

class DpiManager
{
public:
    static DpiManager& instance();
    int scale(int v);
    QSize scale(const QSize& size);
    double scaleFactor() const { return m_scaleFactor; }
    void adaptHDR(QWidget* w);
    int pointSize2Pixel(float pointSize);
    float pixel2PointSize(int pixelSize);

private:
    DpiManager();
    void init();

private:
    int m_dpi = 96;
    double m_scaleFactor = 1.0;
};

#endif
