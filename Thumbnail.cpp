#include "Thumbnail.h"
#include "DpiManager.h"

#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QStyleOption>

Thumbnail::Thumbnail(const QImage& image, int index, QWidget* parent) : QWidget(parent), m_index(index)
{
    setFocusPolicy(Qt::StrongFocus);

    QLabel* thumbnailLabel = new QLabel(this);
    thumbnailLabel->setPixmap(
        QPixmap::fromImage(image).scaled(DpiManager::instance().scale(100), DpiManager::instance().scale(150),
                                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* textLabel = new QLabel(QString::number(index + 1), this);
    textLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setMargin(9);
    layout->addWidget(thumbnailLabel);
    layout->addWidget(textLabel);
}

Thumbnail::~Thumbnail()
{
}

void Thumbnail::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    emit clicked(m_index);
}

void Thumbnail::paintEvent(QPaintEvent* event)
{
    if(hasFocus())
    {
        QPainter painter(this);
        QColor bgColor = QColor(112, 179, 232);
        painter.fillRect(rect(), bgColor);
    }
    QWidget::paintEvent(event);
}
