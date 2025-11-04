#include "DpiManager.h"

#include <QStyle>
#include <QLabel>
#include <QScreen>
#include <QApplication>
#include <QAbstractButton>

DpiManager& DpiManager::instance()
{
    static DpiManager dpiManager;
    return dpiManager;
}

int DpiManager::scale(int v)
{
    return v * m_scaleFactor;
}

QSize DpiManager::scale(const QSize& size)
{
    return QSize(size.width() * m_scaleFactor, size.height() * m_scaleFactor);
}

void DpiManager::adaptHDR(QWidget* w)
{
    /* when scale factor equal 1.0 mean it is not high dpi so return */
    if(qAbs(scaleFactor() - 1.0) < 1e-5) return;

    const int maxSize = 16777215;
    if(QWidget* widget = qobject_cast<QWidget*>(w))
    {
        widget->resize(widget->size() * scaleFactor());
    }

    QList<QWidget*> widgetList = w->findChildren<QWidget*>();
    for(auto widget : widgetList)
    {
        if(widget)
        {
            int maxWidth = widget->maximumWidth();
            if(maxWidth * scaleFactor() <= maxSize) maxWidth = maxWidth * scaleFactor();
            int maxHeight = widget->maximumHeight();
            if(maxHeight * scaleFactor() <= maxSize) maxHeight = maxHeight * scaleFactor();
            widget->setMaximumSize(maxWidth, maxHeight);

            int minWidth = widget->minimumWidth();
            if(minWidth * scaleFactor() <= maxSize) minWidth = minWidth * scaleFactor();
            int minHeight = widget->minimumHeight();
            if(minHeight * scaleFactor() <= maxSize) minHeight = minHeight * scaleFactor();
            widget->setMinimumSize(minWidth, minHeight);

            if(QAbstractButton* button = qobject_cast<QAbstractButton*>(widget))
            {
                int defaultIconSize =
                    button->style()->pixelMetric(QStyle::PM_ButtonIconSize, nullptr, button);
                /* if not set icon size explicitly, qt would scale the default icon size when is high dpi
                qcommonstyle.cpp
                ....
                case PM_ButtonIconSize:
                case PM_SmallIconSize:
                    ret = int(QStyleHelper::dpiScaled(16.));
                ...
                */
                if(button->iconSize().width() != defaultIconSize
                   || button->iconSize().height() != defaultIconSize)
                {
                    button->setIconSize(button->iconSize() * scaleFactor());
                }
            }

            if(QLabel* label = qobject_cast<QLabel*>(widget))
            {
                const QPixmap* labelPm = label->pixmap();
                if(labelPm && !labelPm->isNull() && label->baseSize().width() > 0
                   && label->baseSize().height() > 0)
                {
                    QPixmap* pm = const_cast<QPixmap*>(labelPm);
                    *pm = pm->scaled(label->baseSize() * scaleFactor(), Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
                    label->setPixmap(*pm);
                }
            }
        }
    }
}

float DpiManager::pixel2PointSize(int pixelSize)
{
    return static_cast<float>(pixelSize * 72.0 / m_dpi);
}

int DpiManager::pointSize2Pixel(float pointSize)
{
    return static_cast<int>(pointSize * m_dpi / 72.0);
}

DpiManager::DpiManager()
{
    init();
};

void DpiManager::init()
{
    const QScreen* screen = QGuiApplication::primaryScreen();
    if(screen) m_dpi = qRound(screen->logicalDotsPerInch());

    m_scaleFactor = m_dpi / 96.0;
}