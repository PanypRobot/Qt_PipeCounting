#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QDialog>
#include <QtGui>
#include <QtGui/QPixmap>
#include <QSound>

class ScreenShot : public QDialog
{
public:
    ScreenShot(const QPixmap& screen, QWidget *parent = 0);
    ~ScreenShot();

private:
    virtual void	paintEvent(QPaintEvent *event);
    virtual void	mouseMoveEvent ( QMouseEvent * event );
    virtual void	mousePressEvent ( QMouseEvent * event );
    virtual void	mouseReleaseEvent ( QMouseEvent * event );

private:
    QPixmap m_image;

    QSound m_shutter;           //快门声音效果
    QPoint m_start, m_end;      //开始点，结束点
    QPoint m_pos;               //轨迹点，用于开始结束时赋值给开始点，结束点
    bool m_dragging;            //拖动标志位

public:
    QRect selectRegion;
};

#endif // SCREENSHOT_H
