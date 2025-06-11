#ifndef COUNTWINDOW_H
#define COUNTWINDOW_H

#include <QMainWindow>
//#include <QFileInfo>
#include <mycamera.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class countwindow; }
QT_END_NAMESPACE

class countwindow : public QMainWindow
{
    Q_OBJECT

public:
    countwindow(QWidget *parent = nullptr);
    ~countwindow();

    cv::Mat image;
    cv::Mat processimg;
    int Qvalue;
    int radius=50;

    //霍夫圆参数
    int centerdist = 10;
    int cannyvalue = 100;
    int roundness = 30;
    int minradius = 15;
    int maxradius = 50;

    //std::vector<cv::Vec3f> circles; //存储圆轮廓

    virtual  void timerEvent(QTimerEvent *event);   //重写，继承自QObject
    int timerid1; //定时器id为整形

    MyCamera *mc; //用于摄像头跳转


protected:


public slots:
    void SetCenterdist();
    void SetCannyvalue();
    void SetRoundness();
    void SetMinradius();
    void SetMaxradius();


    void ReadImage();
    void ReadImageFromComputer();
    void ReadImageFromScreenshot();
    void ReadImageFromCamera();
    void CameraRead();
    void SaveCountImage();

    void SaveImage(QPixmap SaveImageQPixmap);

 //   bool FileOverlapCheck(QFileInfo fileInfo);
    void ProcessImage();
    void HoughProcess();
    void PipeNumClear();
    void PipeNumBack();


    //int onCapture();
    //int onSave();
//    int onDelayedCapture();


private:
    Ui::countwindow *ui;

};
#endif // COUNTWINDOW_H
