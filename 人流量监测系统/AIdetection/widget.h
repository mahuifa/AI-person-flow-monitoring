#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    QTimer *timer;
    QCamera *camera;              //系统摄像头设备
    QCameraViewfinder *finder;    //摄像头取景器
    QCameraImageCapture *capture; //截图部件
    QImage Image;                 //保存照片
    bool Switch;                  //开关
    int Position;                 //保存滑动条值
    QImage img ;                  //返回照片
    int ins = 0;                  //进入人数
    int outs = 0;                 //离开人数

    void photo();                 //拍照
    void display();               //显示
    void photograph();            //相机初始化
    void cameraImageCaptured(int id, QImage image); //获得照片




/********************网络**************************************/
    QNetworkAccessManager *manager;    //网络访问管理器
    QNetworkReply * reply;             //网络回复
    QNetworkRequest *requst;           //网络请求
    bool connection = false;

    void initialization();            //网络
    void sendHTTP();                  //发送请求

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
