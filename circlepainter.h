#ifndef CIRCLEPAINTER_H
#define CIRCLEPAINTER_H

#include <QFrame>
#include <QColor>
#include <QPixmap>


namespace Ui {
class CirclePainter;
}

class CirclePainter : public QFrame
{
    Q_OBJECT

public:
    explicit CirclePainter(QWidget *parent = nullptr);
    ~CirclePainter();

    int timerid2; //定时器id为整形
    QPixmap Pix;

    void paintEvent(QPaintEvent * event);
    virtual  void timerEvent(QTimerEvent *event);   //重写，继承自QObject
    void mousePressEvent(QMouseEvent * event);


    Ui::CirclePainter *ui;   //控件公开


    void draw_circle(float x, float y,float radius);
//    void draw_clear();


private:
//    Ui::CirclePainter *ui;    //因为直接提升了，所以把ui写到public里去，使其子控件能被在父窗口中调用

};

#endif // CIRCLEPAINTER_H
