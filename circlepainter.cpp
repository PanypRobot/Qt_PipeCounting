#include "circlepainter.h"
#include "ui_circlepainter.h"


#include "countwindow.h"
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QMouseEvent>

extern QString fileName_read;

extern int pipenum_untreated;
extern int pipenum_plus;
extern int pipenum_minus;
extern int pipenum_ans;
extern std::vector<int> circlecountflag;

extern std::vector<cv::Vec3f> circles; //存储圆轮廓
extern std::vector<cv::Vec2f> circles_plus;     //存储增加圆心
extern std::vector<cv::Vec2f> circles_minus;    //存储减少圆心
extern float AdjustPara;
float scale;



static const QPointF points[13]=
{
    QPointF(-3,0),
    QPointF(-7,3),
    QPointF(-3,7),
    QPointF(0,3),
    QPointF(3,7),
    QPointF(7,3),
    QPointF(3,0),
    QPointF(7,-3),
    QPointF(3,-7),
    QPointF(0,-3),
    QPointF(-3,-7),
    QPointF(-7,-3),
    QPointF(-3,0)
};


//static const QPointF points[13]=
//{
//    QPointF(-6,0),
//    QPointF(-14,6),
//    QPointF(-6,14),
//    QPointF(0,6),
//    QPointF(6,14),
//    QPointF(14,6),
//    QPointF(6,0),
//    QPointF(14,-6),
//    QPointF(6,-14),
//    QPointF(0,-6),
//    QPointF(-6,-14),
//    QPointF(-14,-6),
//    QPointF(-6,0)
//};

CirclePainter::CirclePainter(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CirclePainter)
{
    ui->setupUi(this);
    timerid2 = startTimer(500); //定时重绘

//    QFrame *SaveWindow = new QFrame();  //设置一个保存窗口，但是不显示出来
//    this->ui->SaveWindow->hide();

}

CirclePainter::~CirclePainter()
{
    delete ui;
}

void CirclePainter::paintEvent(QPaintEvent * event)
{
//        QPainter background(this->ui->SaveWindow);
//        QPixmap map;
//        map.load(fileName_read);

//        QPixmap imgScaled=map.scaled(ui->DrawPlace->size(),Qt::KeepAspectRatio);
//        //QPixmap fitpixmap = map.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
//        background.drawPixmap(0,0,imgScaled.size().width(),imgScaled.size().height(),map);


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    scale = float(421.0/AdjustPara);  //相对比例，421是控件的长宽
    //painter.scale((421.0/499.0),(421.0/499.0)); //改变绘制比例
    painter.scale(scale,scale); //改变绘制比例

    QPen pen;
    pen.setBrush(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);

    for (size_t i = 0; i < circles.size(); i++)     //绘制算法检测出的圆，红色
    {
        painter.drawEllipse(QPointF(circles[i][0],circles[i][1]),circles[i][2],circles[i][2]);
        //qDebug()<<"circles"<<i<<circles[i][0]<<circles[i][1]<<circles[i][2]<<endl;
    }

    pen.setBrush(Qt::green);    //绿色，校正增加的圆
    painter.setPen(pen);
    for (size_t i = 0; i < circles_plus.size(); i++)
    {
        painter.drawEllipse(QPointF(circles_plus[i][0],circles_plus[i][1]),5,5);
    }

    pen.setBrush(Qt::yellow);     //黄色，校正减少的叉叉多边形
    painter.setPen(pen);

    for (size_t i = 0; i < circles_minus.size(); i++)
    {
        painter.translate(circles_minus[i][0],circles_minus[i][1]);     //绘制初始点移至点击位置
        painter.drawPolygon(points, 13);                                //开始绘制多边形
        painter.translate(-circles_minus[i][0],-circles_minus[i][1]);   //绘制初始点回归左上角        
    }


    //painter.scale(scale,scale); //还是画的检测圆用于覆盖，此时再次改变绘制比例

    pen.setBrush(Qt::yellow);
    painter.setPen(pen);

    for(size_t i = 0; i < circles_minus.size(); i++) //不太懂size_t是什么数据类型。。。
    {
        for(size_t j = 0; j < circles.size(); j++)
        {
            //  if(QPoint::dotProduct((circles_minus[i][0],circles_minus[i][1]),(circles[j][0],circles[j][1])) < circles[j][3])
            float x_length = (std::pow(circles_minus[i][0]-circles[j][0], 2));   //x距离的平方
            float y_length = (std::pow(circles_minus[i][1]-circles[j][1], 2));   //y距离的平方
            if(x_length + y_length <= std::pow(circles[j][2], 2))    //横纵距离分别平方的和是否小于圆心的平凡，来判断minus点是否在之前的检测圆内
            {
                painter.drawEllipse(QPointF(circles[j][0],circles[j][1]),circles[j][2],circles[j][2]);
            }
        }
    }

    painter.end();

    //QPainter painter_now(this);
    //painter.drawPixmap(0,0,400,400,map);


//    painter.translate(30,30);
//    painter.drawPolygon(points, 13);
//    painter.translate(60,60);
//    painter.drawPolygon(points, 13);

    //    painter.setPen(pen);
    //    for (size_t i = 0; i < circles_minus.size(); i++)
    //    {
    //        painter.drawEllipse(QPointF(circles_minus[i][0],circles_minus[i][1]),5,5);
    //        qDebug()<<"goodluck"<<circles_minus[i][0]<<circles_minus[i][1]<<endl;
    //    }

    //    painter.end();





}


void CirclePainter::mousePressEvent(QMouseEvent * e)
{
    //获取点击的下标
    //qDebug() << e->x() << ":" << e->y();  //改坐标是完全相对frame控件产生的
    if(e->button() == Qt::LeftButton)       //左键，增加的点
    {
        qDebug() << "左键" ;

        //std::vector<int>::iterator ret1;
        auto ret1 = std::find(circles_plus.begin(), circles_plus.end(), (cv::Vec2f(e->x()/scale,e->y()/scale))); //自动变量，太方便了

        if(ret1 == circles_plus.end())      //遍历vector，避免相近位置重复添加标记，因为重复添加没有实际意义，其图片标记上看不出来
        {
            circles_plus.push_back(cv::Vec2f(e->x()/scale,e->y()/scale));
            pipenum_plus++;
            circlecountflag.push_back(1);
            //qDebug() << pipenum_untreated << pipenum_plus;
        }
        else //遍历到了最后一个，没有找到太近的标记
        {
            QMessageBox::about(NULL,tr("提示"),tr("该处已有标记，添加失败！"));
        }
    }

    else if(e->button() == Qt::RightButton)
    {

        auto ret2 = std::find(circles_minus.begin(), circles_minus.end(), (cv::Vec2f(e->x()/scale,e->y()/scale))); //自动变量，太方便了

        if(ret2 == circles_minus.end())      //遍历vector，避免相近位置重复添加标记，因为重复添加没有实际意义，其图片标记上看不出来
        {
            qDebug() << "右键" ;
            circles_minus.push_back(cv::Vec2f(e->x()/scale,e->y()/scale));  //本来的坐标是相对控件生成，正确的，但是修改了绘制scale，所以要除去一个比例参数以回归正确位置
            pipenum_minus++;
            circlecountflag.push_back(-1);
            //qDebug() << pipenum_untreated << pipenum_minus;
        }
        else //遍历到了最后一个，没有找到太近的标记
        {
            QMessageBox::about(NULL,tr("提示"),tr("该处已有标记，添加失败！"));
        }

    }
}



void CirclePainter::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerid2)
    {
        QTime now = QTime::currentTime();
        QString text = now.toString("HH:mm:ss");
//        qDebug() << text;
        this->ui->label22->setText(text);
    }
    repaint();
    //update();
}
