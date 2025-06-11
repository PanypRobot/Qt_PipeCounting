#include "mycamera.h"
#include "ui_mycamera.h"
#include "countwindow.h"


#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

#include <QFile>
#include <QDebug>


extern QString fileName_read;
bool camerashotflag = false;

MyCamera::MyCamera(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyCamera)
{
    ui->setupUi(this);
    QList <QCameraInfo>cameras = QCameraInfo::availableCameras();
    if(cameras.count()>0)
    {
        foreach (const QCameraInfo &cameraInfo, cameras)
        {
            qDebug() << cameraInfo.description();
        }
        camera = new QCamera(cameras.at(0));
    }
    viewfinder = new QCameraViewfinder(this);
    camera->setViewfinder(viewfinder);
    viewfinder->resize(600,350);
    imageCapture = new QCameraImageCapture(camera);
    camera->start();

    connect(this->ui->TakePhotograph, SIGNAL(clicked(bool)), this, SLOT(TakePhotograph()));
    connect(this->ui->CloseWindow, SIGNAL(clicked(bool)), this, SLOT(CloseWindow()));

}


MyCamera::~MyCamera()
{
    delete ui;
}


void MyCamera::TakePhotograph()
{
    camera->setCaptureMode(QCamera::CaptureStillImage);

    QString fileName = QDir::currentPath()+"/camera_temp.jpg";
    //QString fileName ="/camera_temp.jpg";  //这里只能用绝对路径，书中也说了
    qDebug()<<"current currentPath: "<<QDir::currentPath();

    qDebug()<<fileName;
    camera->searchAndLock();

    imageCapture->capture(fileName);   //因为不知道怎么抓出图来，只知道以固定名字保存，所以先保存成临时文件，是否保存就成了是否进行拷贝操作，最后删除临时文件
    qDebug()<<"goodjob";
    camera->unlock();


    QDateTime delay=QDateTime::currentDateTime();
    QDateTime now;
    do
    {
        now=QDateTime::currentDateTime();
    } while (delay.secsTo(now)<=0.5);          //需要一个延时，来保存图片，测试0.5就行了，大了会卡

//        QFile file("camera_temp.jpg");  //删掉临时文件      //放到外部去了，为了进行读取
//        file.remove();

        camerashotflag = true; //拍照了，退出后要进行读取
        //QMessageBox::about(NULL,tr("提示"),tr("成功拍摄"));
        camera->stop();
        this->close();

}


void MyCamera::CloseWindow()
{
    camerashotflag = false; //未拍照，不读取
    camera->stop();
    this->close();
}

void MyCamera::closeEvent(QCloseEvent *event)   //关闭窗口发出信号
{
    if(camerashotflag == true)  //拍照了才触发该信号，用于保存拍照图
    {
    emit ExitWin();
    }
}
