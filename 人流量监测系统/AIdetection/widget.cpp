#include "widget.h"
#include "ui_widget.h"

#include <QBuffer>
#include <QByteArray>
#include <QImageReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    photograph();        //相机
    initialization();    //网络

}

Widget::~Widget()
{
    delete ui;
}

//网络
void Widget::initialization()
{
    manager  = new QNetworkAccessManager(this);
    requst = new QNetworkRequest;

    QString request_url = "https://aip.baidubce.com/rest/2.0/image-classify/v1/body_tracking";    //请求URL
    QString toke = "24.6878d2808d1dd6be2a3a760c82d0979a.2592000.1570716999.282335-17208954";      //access_token
    QString url = request_url + "?access_token=" + toke;
    requst->setUrl(QUrl(url));
    requst->setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");    //Content-Type
    connect(manager,&QNetworkAccessManager::finished,this,
            [=](QNetworkReply *reply)
            {
                QByteArray array = reply->readAll();        //获取信息
                qDebug() << array;
                QJsonObject json1 = QJsonDocument::fromJson(array).object();
                QString image = json1.value("image").toString();

                QByteArray ba;                                     //解析返回渲染图
                ba.append(image);
                QByteArray ba1 = ba.fromBase64(ba);

                QBuffer buffer(&ba1);
                buffer.open(QIODevice::ReadOnly);
                QImageReader reader(&buffer,"jpg");
                img = reader.read();

                int person_num = json1.value("person_num").toInt();
                QJsonObject object = json1.value("person_count").toObject();
                qDebug() << object;
                int in = object.value("in").toInt();
                int out = object.value("out").toInt();
                ins += in;
                outs += out;

                QString str = "当前人数：" + QString::number(person_num);      //返回人数
                ui->label_2->setText(str);
                QString str1 = "进入人数：" + QString::number(ins);
                ui->label_3->setText(str1);
                QString str2 = "离开人数：" + QString::number(outs);
                ui->label_4->setText(str2);

            });

}

//发送请求
void Widget::sendHTTP()
{
    QByteArray ba;
    QBuffer buf(&ba);
    Image.save(&buf, "jpg");

    QByteArray SendData = "";

    SendData.append("&dynamic=true");
    SendData.append("&case_id=1");
    SendData.append("&case_init=false");
    SendData.append("&image=");
    SendData.append(ba.toBase64().toPercentEncoding());
    SendData.append("&show=true");
    SendData.append("&area=1,1,700,1,700,719,1,719");
    qDebug() << SendData;

    manager->post(*requst,SendData);

}

//相机初始化
void Widget::photograph()
{
    this->resize(640,450);                               //设置窗口大小

    camera = new QCamera(this);                          //系统摄像头设备
    finder = new QCameraViewfinder(this);                //摄像头取景器部件
    capture = new QCameraImageCapture(camera);           //截图部件
    timer = new QTimer(this);

    ui->verticalLayout->addWidget(finder);               //在布局器上添加finder
    ui->label->setMaximumSize(0,0);                      //设置照片初始化大小
    ui->pushButton_2->setEnabled(false);

    Switch = false;
    Position = 0;

    connect(timer,&QTimer::timeout,this,&Widget::photo); //监听定时器
    connect(capture,&QCameraImageCapture::imageCaptured,this,
            &Widget::cameraImageCaptured);                               //图像捕获
    capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);  //设置要捕获的对象
    camera->setCaptureMode(QCamera::CaptureStillImage);                  //摄像机配置为静止帧捕获。
    camera->setViewfinder(finder);                                       //设置取景器
}

//拍照
void Widget::photo()
{
    capture->capture();             //拍照
    sendHTTP();
    display();                     //显示
}

//获得图像
void Widget::cameraImageCaptured(int id, QImage image)
{
    Image = image;
}

//打开摄像头
void Widget::on_pushButton_clicked()
{
    if(!Switch)
    {
        camera->start();          //打开摄像头
        ui->pushButton->setText("关闭摄像头");
        ui->pushButton_2->setEnabled(true);   //打开button
        Switch = true;
    }
    else
    {
        camera->stop();
        ui->pushButton->setText("打开摄像头");
        ui->pushButton_2->setEnabled(false);
        Switch = false;
      //  connection = false;
    }

}

//定时拍照
void Widget::on_pushButton_2_clicked()
{
    if(!timer->isActive())
    {
        timer->start(200);
        ui->pushButton_2->setText("停止");
    }
    else
    {
        timer->stop();
        ui->pushButton_2->setText("开始");
    }

}

//滑动切换
void Widget::on_horizontalSlider_sliderMoved(int position)
{
    Position = position;
    display();
}

//显示
void Widget::display()
{
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(this->width());
    ui->label->setMaximumSize(Position * (16/9),Position);

    QPixmap pixmap = QPixmap::fromImage(img );
    QPixmap fitpixmap = pixmap.scaled(Position * (16/9),Position, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    ui->label->setPixmap(fitpixmap);
    qDebug() << Position;
}
