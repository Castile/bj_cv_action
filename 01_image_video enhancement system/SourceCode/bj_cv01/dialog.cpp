#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<QTimer>
using namespace cv;
using namespace std;

cv::Mat multi_scale_retinex(const cv::Mat &img, const std::vector<double>& weights, const std::vector<double>& sigmas, int gain, int offset);

cv::Mat ALTM_retinex(const cv::Mat& img, bool LocalAdaptation = false, bool ContrastCorrect = true);

cv::Mat adaptive_logarithmic_mapping(const cv::Mat& img);

QImage Mat2QImage(cv::Mat& image);
cv::Mat QImageToMat(QImage image);

Mat darkChannel(Mat src);
Mat getSuccesMatwithDark(Mat image, Mat darkChannel1);


Dialog::Dialog(QWidget *parent): QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->qimgOriginal = new QImage();
//    connect(ui->btnOpenImage, SIGNAL(clicked()), this, SLOT(on_btnOpenImage_clicked()));

//    connect(ui->ritnex, SIGNAL(clicked()), this, SLOT(on_ritnex_clicked()));
//    connect(ui->dark, SIGNAL(clicked()), this, SLOT(on_dark_clicked()));


}

Dialog::~Dialog()
{
    delete ui;
}



void Dialog::on_btnOpenImage_clicked()
{
    //QString fileName = QFileDialog::getOpenFileName(this);
        fileName = QFileDialog::getOpenFileName(
            this, "open image file",
            ".",
            "Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
        if (fileName != "")
        {
            if (qimgOriginal->load(fileName))
            {
               *qimgOriginal = qimgOriginal->scaled( ui->lblOriginal->width(), ui->lblOriginal->height());
               ui->lblOriginal->setPixmap(QPixmap::fromImage(*qimgOriginal));
               ui->lblOriginal->resize(qimgOriginal->width()+10, qimgOriginal->height()+10);
               matOriginal = QImageToMat(*qimgOriginal);

            }
        }

}


/**
 * @brief Dialog::on_Sritnex_clicked
 * 单通道Ritinex算法
 */
void Dialog::on_Sritnex_clicked()
{

    Msrcr msrcr;
    Mat img = imread(fileName.toLatin1().data());  //读取Mat格式
//    Mat img = matOriginal;
    msrcr.Retinex(img, matProcessed, 100);
//    imshow("dst", matProcessed);

//    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);

//    qimgProcessed = Mat2QImage(matProcessed);

    cvtColor(matProcessed, matProcessed, COLOR_BGR2RGB);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);
    qimgProcessed = qimgProcessed.scaled( ui->lblProcessed->width(), ui->lblProcessed->height());
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));

//    ui->lblProcessed->resize(qimgProcessed.width()+10, qimgProcessed.height()+10);
//    *qimgProcessed = qimgProcessed->scaled( ui->lblProcessed->width(), ui->lblProcessed->height());
//    ui->lblProcessed->setPixmap(QPixmap::fromImage(*qimgProcessed));
//    ui->lblProcessed->resize(qimgProcessed->width()+10, qimgProcessed->height()+10);


}

/**
 * @brief Dialog::on_ritnex_clicked
 *
 * 多通道Ritinex算法
 */

void Dialog::on_ritnex_clicked()
{
    vector<double> sigema;
    vector<double> weight;
    for(int i = 0; i < 3; i++)
        weight.push_back(1./3);
    // 由于MSRCR.h中定义了宏USE_EXTRA_SIGMA，所以此处的vector<double> sigema并没有什么意义
    sigema.push_back(30);
    sigema.push_back(150);
    sigema.push_back(300);

//    Mat img = matOriginal;
    Mat img = imread(fileName.toUtf8().data());  //读取Mat格式

    Msrcr msrcr;
    msrcr.MultiScaleRetinexCR(img, matProcessed, weight, sigema, 128, 128);
//    imshow("dst", matProcessed);

//    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);

//    qimgProcessed = Mat2QImage(matProcessed);

//    cvtColor(matProcessed, matProcessed, COLOR_BGR2RGB);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);

    qimgProcessed = qimgProcessed.scaled( ui->lblProcessed->width(), ui->lblProcessed->height());
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));


//    ui->lblProcessed->resize(qimgProcessed.width()+10, qimgProcessed.height()+10);
//    *qimgProcessed = qimgProcessed->scaled( ui->lblProcessed->width(), ui->lblProcessed->height());
//    ui->lblProcessed->setPixmap(QPixmap::fromImage(*qimgProcessed));
//    ui->lblProcessed->resize(qimgProcessed->width()+10, qimgProcessed->height()+10);

}

void Dialog::on_dark_clicked()
{
//    Mat img = matOriginal;
    Mat img = imread(fileName.toUtf8().data());  //读取Mat格式
    Mat darkChannel1 = darkChannel(img);
    matProcessed = getSuccesMatwithDark(img, darkChannel1);
//    imshow("dark", matProcessed);


    //show

    cvtColor(matProcessed, matProcessed, COLOR_BGR2RGB);
//    imshow("dark", matProcessed);
    QImage qimgProcessed((uchar*)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_RGB888);
    qimgProcessed = qimgProcessed.scaled( ui->lblProcessed->width(), ui->lblProcessed->height());
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));

//    ui->lblProcessed->resize(qimgProcessed.width()+10, qimgProcessed.height()+10);


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

