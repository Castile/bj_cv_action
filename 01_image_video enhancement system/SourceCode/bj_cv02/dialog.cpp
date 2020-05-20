#include "dialog.h"
#include "ui_dialog.h"
#include<iostream>
#include"MSRCR.h"
using namespace std;
using namespace cv;

QImage Mat2QImage(cv::Mat& image);
cv::Mat QImageToMat(QImage image);
#include<QTimer>
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    capture.open(0);
    tmrTimer = new QTimer(this);

    tmrTimer1 = new QTimer(this);
    tmrTimer2 = new QTimer(this);
    connect(tmrTimer1, SIGNAL(timeout()), this, SLOT(OpenCamera_clicked()));
    connect(tmrTimer2, SIGNAL(timeout()), this, SLOT(Enhance_clicked()));
    connect(ui->btnOpenCamera, SIGNAL(clicked()), this, SLOT(on_originalImg_clicked()));
    connect(ui->Ritinex, SIGNAL(clicked()), this, SLOT(on_processedImg_clicked()));

}

Dialog::~Dialog()
{
    delete ui;
}


void  Dialog::OpenCamera_clicked()
{
    capture>>matOriginal;
    cvtColor(matOriginal, matOriginal, CV_BGR2RGB);
    QImage qimgOriginal((const unsigned char*)matOriginal.data, matOriginal.cols, matOriginal.rows, QImage::Format_RGB888);
    qimgOriginal = qimgOriginal.scaled( ui->lblOriginal->width(), ui->lblOriginal->height());
    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));
}

void  Dialog::Enhance_clicked(){


    Msrcr msrcr;
//    msrcr.MultiScaleRetinexCR(matOriginal, matProcessed, weight, sigema, 128, 128);
    msrcr.Retinex(matOriginal, matProcessed, 50);
//    cvtColor(matProcessed, matProcessed, COLOR_BGR2RGB);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);

    qimgProcessed = qimgProcessed.scaled( ui->lblProcessed_2->width(), ui->lblProcessed_2->height());
    ui->lblProcessed_2->setPixmap(QPixmap::fromImage(qimgProcessed));


}
void  Dialog::on_originalImg_clicked(){
    tmrTimer1->start(20);
    tmrTimer->start(20);

}
void  Dialog::on_processedImg_clicked(){
    tmrTimer2->start(20);
}


cv::Mat QImageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage Mat2QImage(cv::Mat& image)
{
    QImage img;

        if (image.channels()==3) {
            cvtColor(image, image, CV_BGR2RGB);
            img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                    image.cols*image.channels(), QImage::Format_RGB888);
        } else if (image.channels()==1) {
            img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                    image.cols*image.channels(), QImage::Format_ARGB32);
        } else {
            img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                    image.cols*image.channels(), QImage::Format_RGB888);
        }

        return img;
}






void Dialog::on_btnPauseOrResume_clicked()
{
    if(tmrTimer->isActive() == true){
        tmrTimer->stop();
        tmrTimer1->stop();
        tmrTimer2->stop();
        ui->btnPauseOrResume->setText("继续");

    }else{
        tmrTimer->start(20);
        tmrTimer1->start(20);
        tmrTimer2->start(20);
        ui->btnPauseOrResume->setText("暂停");

    }

}
