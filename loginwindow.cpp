#include "loginwindow.h"
#include "ui_loginwindow.h"


#include "countwindow.h"
#include "ui_countwindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>


static int count = 0;

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , uii(new Ui::LoginWindow)
{
    uii->setupUi(this);
    connect(this->uii->btn1,SIGNAL(clicked(bool)),this,SLOT(login()));
    connect(this->uii->btn2,SIGNAL(clicked(bool)),this,SLOT(quit()));
}


void LoginWindow::login()
{
    QString name = this->uii->le_account->text();
    QString passwd = this->uii->le_passwd->text();

    if(name == "" || passwd == ""){
        count++;
        QMessageBox::information(NULL, tr("登录提示"), tr("账号栏或密码栏为空"));
        LoginWindow::checknum();
    }

    else if(name == "1" && passwd == "1"){
//    else if(name == "jiafan" && passwd == "123456"){
        qDebug()<<"true";
        QMessageBox::information(NULL, tr("登录提示"), tr("登陆成功"));
        LoginWindow::quit();
        LoginWindow::exchangeWin();
    }
    else{
        qDebug()<<"false";
        QMessageBox::information(NULL, tr("登录提示"), tr("账号不存在或密码错误"));
        count++;
        LoginWindow::checknum();
        }
}

void LoginWindow::quit()
{
    this->close();
}

void LoginWindow::checknum()
{
    if(count >= 3){
    QMessageBox::information(NULL, tr("登录提示"), tr("尝试过多,无法再尝试"));
    disconnect(this->uii->btn1,SIGNAL(clicked(bool)),this,SLOT(login()));
    this->close();
    this->uii->btn1->setDisabled(true);
    }
}


void LoginWindow::exchangeWin(){
     cw = new countwindow;
     cw->show();
}


LoginWindow::~LoginWindow()
{
    delete uii;
}



void LoginWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        {
        quit();     //Esc键退出登录框
        }
        else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)   //Enter键的捕获比较特殊，要两个同时关联
        {
        login();    //Enter键进行登录
        }
}


