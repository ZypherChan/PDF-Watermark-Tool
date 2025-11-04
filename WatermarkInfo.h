#ifndef __WATERMARKINFO_H__
#define __WATERMARKINFO_H__

#include <QString>
#include <QColor>

struct WaterMarkInfo
{
    QString text;
    QString family;
    QColor color;
    int fontSize;
    double angle;
    double opacity;
    int repeatRows;
    int repeatCols;

    bool enabled = false;
};

#endif  // __WATERMARKINFO_H__