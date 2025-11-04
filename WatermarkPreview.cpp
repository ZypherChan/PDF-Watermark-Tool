#include "WatermarkPreview.h"
#include <QPainter>

WatermarkPreview::WatermarkPreview(QWidget* parent) : QWidget(parent)
{
}

WatermarkPreview::~WatermarkPreview()
{
}

void WatermarkPreview::setWatermarkInfo(const WaterMarkInfo& info)
{
    m_watermarkInfo = info;
    update();
}

void WatermarkPreview::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(m_watermarkInfo.color));
    
    // Calculate the text size to ensure that all text is displayed inside the control
    int optimalSize = calculateOptimalFontSize(m_watermarkInfo.text);
    
    painter.setFont(QFont(m_watermarkInfo.family, optimalSize));
    painter.setOpacity(m_watermarkInfo.opacity);
    QFontMetrics fm(painter.font());
    QRect textRect = fm.boundingRect(m_watermarkInfo.text);

    int x = width() / 2;
    int y = height() / 2;

    painter.translate(x, y);
    painter.rotate(-m_watermarkInfo.angle);

    QRect watermarkRect(-textRect.width() / 2, -textRect.height() / 2, textRect.width(), textRect.height());

    painter.drawText(watermarkRect, Qt::AlignCenter, m_watermarkInfo.text);
}

int WatermarkPreview::calculateOptimalFontSize(const QString& text)
{
    int fontSize = 1;
    int minSize = 1;
    int maxSize = 100;
    int maxWidth = width() * 0.8;
    int maxHeight = height() * 0.8;
    
    // Binary search method to find the appropriate font size
    while (minSize <= maxSize) {
        int midSize = (minSize + maxSize) / 2;
        QFont font(m_watermarkInfo.family, midSize);
        QFontMetrics fm(font);
        
        QRect textRect = fm.boundingRect(text);
        double angleRad = m_watermarkInfo.angle * 3.1415926535 / 180.0;
        int rotatedWidth = qAbs(textRect.width() * cos(angleRad)) + qAbs(textRect.height() * sin(angleRad));
        int rotatedHeight = qAbs(textRect.width() * sin(angleRad)) + qAbs(textRect.height() * cos(angleRad));
        
        if (rotatedWidth <= maxWidth && rotatedHeight <= maxHeight) {
            fontSize = midSize;
            minSize = midSize + 1;
        } else {
            maxSize = midSize - 1;
        }
    }
    
    return fontSize;
}
