#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <QWidget>


namespace Ui {
class MyCamera;
}

class QCamera;
class QCameraViewfinder;
class QCameraImageCapture;

class MyCamera : public QWidget
{
    Q_OBJECT

public:
    explicit MyCamera(QWidget *parent = nullptr);
    ~MyCamera();


public slots:
    void closeEvent(QCloseEvent *); //重写关闭事件用于发出关闭信号
    void TakePhotograph();
    void CloseWindow();

signals:
    void ExitWin(); //窗口关闭信号，用于传给父窗口


private:
    Ui::MyCamera *ui;

    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;


};

#endif // MYCAMERA_H
