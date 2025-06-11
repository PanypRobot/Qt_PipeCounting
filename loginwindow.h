#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <countwindow.h>


QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void login();
    void quit();
    void exchangeWin(); //切换信号


public:
    LoginWindow(QWidget *parent = nullptr);
    void checknum(); //自己写的次数检测函数
    countwindow *cw;
    void keyPressEvent(QKeyEvent *event);   //键盘按下事件


    ~LoginWindow();

private:
    Ui::LoginWindow *uii;
};
#endif // LOGINWINDOW_H
