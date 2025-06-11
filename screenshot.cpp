#include "screenshot.h"
#include "countwindow.h"
#include <QtGui>

ScreenShot::ScreenShot(const QPixmap& screen,QWidget *parent)
    : QDialog(parent), m_image(screen) , m_shutter("shutter.wav")
{

    m_dragging = false;
    setMouseTracking(true);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    //showMaximized();
    showFullScreen();
}

ScreenShot::~ScreenShot()
{
}

void ScreenShot::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    int w = width(), h = height();

    //绘制桌面全图
    QPainter painter(this);
    QRectF target(0,0, width(),height());
    QRectF source(0,0, m_image.width(), m_image.height());
    //QRectF source(0,0, screen()->size().width(), screen()->size().height());
    painter.drawPixmap(target, m_image, source);

    //绘制十字线
    int x = m_pos.x(), y = m_pos.y();
    //x -= 1; y -= 1;
    x += 1; y += 1;
    //qDebug("repaint: (%dx%d)", x, y);
    painter.setPen(QColor(60,60,60, 150));
    painter.drawLine(0, y, w, y);
    painter.drawLine(x, 0, x, h);
    painter.setPen(QColor(160,160,160, 150));
    painter.drawLine(0, y, w, y);
    painter.drawLine(x, 0, x, h);

    if(m_dragging)
    {
        QRect selected(m_start, m_pos); //绘制选择区域
        painter.setPen(QColor(30,30,30));
        painter.drawRect(selected);

        QRect textRect(0,0, 80, 16);    //坐标显示区
        textRect.moveCenter(selected.center());

        painter.setBrush(QColor(30,30,30));
        painter.drawRect(textRect);

        painter.setPen(QColor(255,255,255));
        char text[40];
        sprintf(text, "%dx%d", selected.width(), selected.height());
        painter.drawText(textRect, Qt::AlignCenter, text);  //坐标区显示坐标
    }
}

void ScreenShot::mouseMoveEvent ( QMouseEvent * event )
{
    m_pos = event->pos();
    update();       //移动时重绘
}

void ScreenShot::mousePressEvent ( QMouseEvent * event )    //按下时开启拖动标志位，记录开始位置
{
    m_dragging = true;
    m_start = event->pos();
}

void ScreenShot::mouseReleaseEvent ( QMouseEvent * event )  //松开时关闭拖动标志位，记录结束位置
{
    m_end = event->pos();
    m_dragging = false;
    update();       //关闭时重绘

    if(m_end.x() > m_start.x() + 16 && m_end.y() > m_start.y() + 16)
    {
        m_shutter.play();
        selectRegion = QRect(m_start*2, m_end*2);       //也不知道为啥*2截取出来的就对了。。。
        accept();
    }
}
