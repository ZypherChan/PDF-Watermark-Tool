#ifndef __THUMBNAIL_H__
#define __THUMBNAIL_H__

#include <QImage>
#include <QWidget>

class Thumbnail : public QWidget
{
    Q_OBJECT
public:
    Thumbnail(const QImage& image, int index, QWidget* parent = nullptr);
    ~Thumbnail();

    Q_SIGNAL void clicked(int index);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    int m_index;
};

#endif  // __THUMBNAIL_H__