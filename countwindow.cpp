#include "countwindow.h"
#include "ui_countwindow.h"
#include "circlepainter.h"
#include "ui_circlepainter.h"
#include "screenshot.h"
#include "mycamera.h"


#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QSpinBox>
#include <QPainter>
#include <QFileInfo>
#include <QTimer>
#include <QDesktopWidget>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//#define DESKTOPPATH "/Users/panyp/Desktop/"

    int pipenum_untreated = 0;  //圆的检测值
    int pipenum_plus = 0;       //圆的增减校正
    int pipenum_minus = 0;
    int pipenum_ans = 0;        //圆的最终计算值

    std::vector<cv::Vec3f> circles;             //存储圆轮廓
    std::vector<cv::Vec2f> circles_plus;        //存储增加圆心
    std::vector<cv::Vec2f> circles_minus;       //存储减少圆心
    std::vector<int> circlecountflag;

    float AdjustPara; //传入图片的长或宽的最小值，用于QPainter的比例校正
    QString fileName_read;
    QString fileName_write;

    bool processflag = 0;

countwindow::countwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::countwindow)
{
    ui->setupUi(this);
    timerid1 = startTimer(100);     //使用定时器刷新pipenum显示
    //ui->PipeNum->setText("未处理");
    connect(this->ui->ReadImageFromComputer,SIGNAL(clicked(bool)),this,SLOT(ReadImageFromComputer()));  //按下“读取图片”按钮，原图片在“show1”窗口显示
    connect(this->ui->ReadImageFromCamera,SIGNAL(clicked(bool)),this,SLOT(ReadImageFromCamera()));  //按下“读取图片”按钮，原图片在“show1”窗口显示
    connect(this->ui->ReadImageFromScreenshot,SIGNAL(clicked(bool)),this,SLOT(ReadImageFromScreenshot()));  //按下“读取图片”按钮，原图片在“show1”窗口显示

    //以下五个参数通过将spin和slider动作相互关联，进行两个控件数据同步，Qt并不会使之无限互相调用

    //cannyvalue参数
    connect(this->ui->spin_canny, SIGNAL(valueChanged(int)),this->ui->slider_canny, SLOT(setValue(int)));
    connect(this->ui->slider_canny, SIGNAL(valueChanged(int)), this->ui->spin_canny, SLOT(setValue(int)));
    connect(this->ui->spin_canny, SIGNAL(valueChanged(int)), this, SLOT(SetCannyvalue()));
    connect(this->ui->spin_canny, SIGNAL(valueChanged(int)), this, SLOT(ProcessImage()));

    //centerdist参数
    connect(this->ui->spin_centerdist, SIGNAL(valueChanged(int)),this->ui->slider_centerdist, SLOT(setValue(int)));
    connect(this->ui->slider_centerdist, SIGNAL(valueChanged(int)), this->ui->spin_centerdist, SLOT(setValue(int)));
    connect(this->ui->spin_centerdist, SIGNAL(valueChanged(int)), this, SLOT(SetCenterdist()));
    connect(this->ui->spin_centerdist, SIGNAL(valueChanged(int)), this, SLOT(HoughProcess()));

    //roundness参数
    connect(this->ui->spin_roundness, SIGNAL(valueChanged(int)),this->ui->slider_roundness, SLOT(setValue(int)));
    connect(this->ui->slider_roundness, SIGNAL(valueChanged(int)), this->ui->spin_roundness, SLOT(setValue(int)));
    connect(this->ui->spin_roundness, SIGNAL(valueChanged(int)), this, SLOT(SetRoundness()));
    connect(this->ui->spin_roundness, SIGNAL(valueChanged(int)), this, SLOT(HoughProcess()));

    //minradius参数
    connect(this->ui->spin_miniradius, SIGNAL(valueChanged(int)),this->ui->slider_miniradius, SLOT(setValue(int)));
    connect(this->ui->slider_miniradius, SIGNAL(valueChanged(int)), this->ui->spin_miniradius, SLOT(setValue(int)));
    connect(this->ui->spin_miniradius, SIGNAL(valueChanged(int)), this, SLOT(SetMinradius()));
    connect(this->ui->spin_miniradius, SIGNAL(valueChanged(int)), this, SLOT(HoughProcess()));

    //maxradius参数
    connect(this->ui->spin_maxradius, SIGNAL(valueChanged(int)),this->ui->slider_maxradius, SLOT(setValue(int)));
    connect(this->ui->slider_maxradius, SIGNAL(valueChanged(int)), this->ui->spin_maxradius, SLOT(setValue(int)));
    connect(this->ui->spin_maxradius, SIGNAL(valueChanged(int)), this, SLOT(SetMaxradius()));
    connect(this->ui->spin_maxradius, SIGNAL(valueChanged(int)), this, SLOT(HoughProcess()));


    //计数校正的交互按钮
    connect(this->ui->PipeNumClear, SIGNAL(clicked(bool)), this, SLOT(PipeNumClear()));
    connect(this->ui->PipeNumBack, SIGNAL(clicked(bool)), this, SLOT(PipeNumBack()));


    connect(this->ui->SaveImage,SIGNAL(clicked(bool)),this,SLOT(SaveCountImage()));  //保存图片
}

countwindow::~countwindow()
{
    delete ui;
}


//五个函数，从slider或spin读取参数

void countwindow::SetCenterdist(){
    centerdist = this->ui->spin_centerdist->value();
}

void countwindow::SetCannyvalue(){
    cannyvalue = this->ui->spin_canny->value();
}

void countwindow::SetRoundness(){
    roundness = this->ui->spin_roundness->value();
}

void countwindow::SetMinradius(){
    minradius = this->ui->spin_miniradius->value();
}

void countwindow::SetMaxradius(){
    maxradius = this->ui->spin_maxradius->value();
}



void countwindow::ReadImageFromComputer()
{
    fileName_read = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));     //弹出文件选取框，选取一个来自该三种格式的图片
    qDebug()<<"fileName_read:"<<fileName_read;
    ReadImage();
}


void countwindow::ReadImageFromCamera()
{

    mc = new MyCamera(NULL); //指针指向新摄像头窗口，无父级
    mc->setAttribute(Qt::WA_ShowModal, true);   //设置非模态，由于QWidget没有QDialog一样的非模态，所以不能写if(xx.exec() == QDialog::Accepted)的判断
    mc->show();


    connect(mc,SIGNAL(ExitWin()),this,SLOT(CameraRead()));
   //CameraRead();
}

void countwindow::CameraRead() //使用信号槽机制实现阻塞，摄像头窗口退出后才进行保存，读取，删除临时文件等操作
{
    qDebug()<<"goodjob2";

    fileName_write = QFileDialog::getSaveFileName(this,"Save File","."); //若是使用默认路径，则采用后一个参数可用QDir::currentPath()
    if(fileName_write.isEmpty())
    {
        QMessageBox::about(NULL,tr("提示"),tr("退出未保存！"));
        //qDebug()<<"fileName_write:"<<fileName_write;

    }
    else
    {
        fileName_write = fileName_write + ".jpg";
        //qDebug()<<"fileName_write:"<<fileName_write;
        QFileInfo fileInfo(fileName_write);
        if(fileInfo.isFile())
        {
            QMessageBox choose;
            choose.setText(tr("文件名重复,使用新文件覆盖目录下重复文件？"));
            choose.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

            if(choose.exec() == QMessageBox::Yes)
            {
                QFile::copy("camera_temp.jpg",fileName_write);
                QMessageBox::about(NULL,tr("提示"),tr("覆盖成功"));
            }
            else
            {
                QMessageBox::about(NULL,tr("提示"),tr("文件未保存"));
                return;
            }
        }
        else
        {
            QFile::copy("camera_temp.jpg",fileName_write);
            QMessageBox::about(NULL,tr("提示"),tr("保存成功"));
        }
}

    fileName_read = ("camera_temp.jpg");    //将临时文件读取到图片待处理框
    ReadImage();

    QFile file("camera_temp.jpg");  //删掉临时文件
    file.remove();
}


/*****************/

void countwindow::ReadImageFromScreenshot()
{
    //this->hide();

    QDateTime delay=QDateTime::currentDateTime(); //延时函数
    QDateTime now;
    do
    {
        now=QDateTime::currentDateTime();
    } while (delay.secsTo(now)<=1);          //延时1s，否则原窗口还没有完全响应隐藏

    QPixmap screen 	= QPixmap::grabWindow(QApplication::desktop()->winId()); //将桌面抓取为一个QPixmap
    ScreenShot dlg(screen, this);   //将抓图用一个对话框窗口显示

    QFile file("screenshot.jpg");   //给图片取个文件名字
    if(QDialog::Accepted == dlg.exec())
    {
       QPixmap region = screen.copy(dlg.selectRegion);  //截取得到的图片copy给region

       file.open(QIODevice::WriteOnly);
       region.save(&file, "jpg");       //保存到当前目录
       SaveImage(region);
    }

    //screen.save(&file,"jpg"); //QDir::tempPath()
    fileName_read = "screenshot.jpg"; //把保存图片的名字作为读取文件名，相对路径

    ReadImage();
    file.remove();  //读取完，删掉

    //this->showNormal();
}

//int countwindow::onSave()
//{
//	QPixmap picture = ui.frame->pixmap();
//	if(picture.isNull()) return 0;


//	QString filename = QFileDialog::getSaveFileName(this,
//		"Save Picture", "", "Image File(*.jpg)");
//	if(filename.length() > 0)
//	{
//		picture.save(filename);
//	}
//	return 0;
//}

/*****************/




void countwindow::ReadImage()
{
//    fileName_read = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));     //弹出文件选取框，选取一个来自该三种格式的图片
//    qDebug()<<"fileName_read:"<<fileName_read;

    ui->show1->resize(211,211); //防止多次读图，resize后长宽不可控地变小
    ui->show2->resize(211,211);
    ui->show3->resize(421,421);

    if(fileName_read.isEmpty()){         //String类型判断为空
        QMessageBox::information(NULL, "提示", "未选取图片！");
        return;
    }
    else
    {
        //image = cv::imread(fileName_read.toLatin1().data()); //image从文件中读出数据
        image = cv::imread(fileName_read.toUtf8().data()); //image从文件中读出数据,将toLatin1改成toUtf8后支持了中文文件名

        QMessageBox::information(NULL, "提示", "图片已读取！");

        cv::Mat rgb;
        QImage img;
        QImage imgScaled;
        if(image.channels()==3)      //cvt Mat BGR 2 QImage RGB
        {
            cvtColor(image,rgb,CV_BGR2RGB);
            img =QImage((const unsigned char*)(rgb.data),
                        rgb.cols,rgb.rows,
                        rgb.cols*rgb.channels(),
                        QImage::Format_RGB888);
        }
        else
        {
            img =QImage((const unsigned char*)(image.data),
                        image.cols,image.rows,
                        image.cols*image.channels(),
                        QImage::Format_RGB888);
        }

        if(rgb.cols > rgb.rows){        //校正参数为输入图片长宽中较大的那一个
            AdjustPara = rgb.cols;
        }
        else{
            AdjustPara = rgb.rows;
        }


        imgScaled=img.scaled(ui->show1->size(),Qt::KeepAspectRatio);

        ui->show1->setPixmap(QPixmap::fromImage(imgScaled));
        ui->show1->resize(ui->show1->pixmap()->size());
    }
}


void countwindow::SaveCountImage()
{
    QPixmap countImage 	= this->grab(QRect(320,220,421,421));   //后两个参数是截取面积的长宽，即结束点相对开始点的坐标哦
    SaveImage(countImage);
}

void countwindow::SaveImage(QPixmap SaveImageQPixmap)
{

    QString fileName_write = QFileDialog::getSaveFileName(this,"Save File","."); //若是使用默认路径，则采用后一个参数可用QDir::currentPath()
    if(fileName_write.isEmpty())
    {
        QMessageBox::about(NULL,tr("提示"),tr("退出未保存！"));
        //qDebug()<<"fileName_write:"<<fileName_write;

    }
    else
    {
        fileName_write = fileName_write + ".jpg";
        //qDebug()<<"fileName_write:"<<fileName_write;
        QFileInfo fileInfo(fileName_write);
        if(fileInfo.isFile()) //检测同名文件是否存在
        {
            QMessageBox choose;
            choose.setText(tr("文件名重复,使用新文件覆盖目录下重复文件？"));
            choose.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

            if(choose.exec() == QMessageBox::Yes)
            {
                //                cv::imwrite(fileName_write.toStdString(),processimg);//此处原报错，发现是imwrite第一个参数需要string，而不能是QString
                SaveImageQPixmap.save(fileName_write,"jpg"); //函数参数为QPixmap，即保存对象
                QMessageBox::about(NULL,tr("提示"),tr("覆盖成功"));
            }
            else
            {
                QMessageBox::about(NULL,tr("提示"),tr("文件未保存"));
                return;
            }
        }
        else
        {
            //cv::imwrite(fileName_write.toStdString(),processimg);//此处原报错，发现是imwrite第一个参数需要string，而不能是QString
            //            QPixmap pixmap = this->ui->frame->ui->SaveWindow->grab();

            //            pixmap.save(fileName_write,"jpg");
            SaveImageQPixmap.save(fileName_write,"jpg"); //函数参数为QPixmap，即保存对象
            QMessageBox::about(NULL,tr("提示"),tr("保存成功"));
        }
    }
}



void countwindow::ProcessImage()
{
    if(fileName_read.isEmpty()){         //String类型判断为空
        QMessageBox::information(NULL, tr("提示"), tr("未选取图片！"));
        return;
    }
    else
    {
        cv::GaussianBlur(image, processimg, cv::Size(3, 3), 0);

        cvtColor(processimg, processimg, cv::COLOR_BGR2GRAY);

        //Canny算子
        Canny(processimg, processimg, cannyvalue/2, cannyvalue, 3);

        //膨胀操作
        cv::Mat element = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::morphologyEx(processimg, processimg, cv::MORPH_DILATE, element);

        //imshow("processimg",processimg);

        cvtColor(processimg, processimg, CV_GRAY2RGB);
        QImage img(processimg.data, processimg.cols, processimg.rows, processimg.step, QImage::Format_RGB888);

        QImage imgScaled;
        imgScaled=img.scaled(ui->show2->size(),Qt::KeepAspectRatio);

        ui->show2->setPixmap(QPixmap::fromImage(imgScaled));
        ui->show2->resize(ui->show2->pixmap()->size());

        cv::Mat rgb;
        cvtColor(image,rgb,CV_BGR2RGB);
        QImage img2 =QImage((const unsigned char*)(rgb.data),
                            rgb.cols,rgb.rows,
                            rgb.cols*rgb.channels(),
                            QImage::Format_RGB888);
        QImage imgScaled2 = img2.scaled(ui->show3->size(),Qt::KeepAspectRatio);

        ui->show3->setPixmap(QPixmap::fromImage(imgScaled2));
        ui->show3->resize(ui->show3->pixmap()->size());
    }

}




void countwindow::HoughProcess(){

    if(fileName_read.isEmpty()){         //String类型判断为空
        QMessageBox::information(NULL, tr("提示"), tr("未选取图片！"));
        return;
    }
    else
    {
        processflag = 1;
        circles.clear();
        cv::Mat src = image;
        cv::Mat moutput;
        cv::medianBlur(src, moutput, 3);
        cv::cvtColor(moutput, moutput, CV_BGR2GRAY);

        HoughCircles(moutput, circles, cv::HOUGH_GRADIENT, 1, centerdist, cannyvalue, roundness, minradius, maxradius);    //霍夫圆测试代码，输入需要是灰度图
        if(circles.size() > 1000)
        {
            circles.clear();
            QMessageBox::information(NULL, tr("提示"), tr("计数超过1000！请考虑重新校参"));
        }
        pipenum_untreated = circles.size();
    }
}



void countwindow::PipeNumClear()
{
    pipenum_minus = 0;
    pipenum_plus = 0;
    circles_plus.clear();
    circles_minus.clear();  //通过清空容器实现清除增减标记的
    circles.clear();
    circlecountflag.clear();
}


void countwindow::PipeNumBack()
{
    if(circlecountflag.size() == 0)
    {
        return;
    }
    else
    {
        int juge = circlecountflag.back();
        if(juge == 1)
        {
            circles_plus.pop_back();
            pipenum_plus--;
        }
        else if(juge == -1)
        {
            circles_minus.pop_back();
            pipenum_minus--;
        }
        circlecountflag.pop_back();
    }


}



void countwindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerid1 && processflag == 1)
    {
        ui->PipeNumUntreated->setNum(pipenum_untreated);
        ui->PipeNumAdjust_Plus->setText(tr("+")+QString::number(pipenum_plus));
        ui->PipeNumAdjust_Minus->setText(tr("-")+QString::number(pipenum_minus));
        pipenum_ans = pipenum_untreated + pipenum_plus - pipenum_minus;
        ui->PipeNumAns->setNum(pipenum_ans);
    }
}





