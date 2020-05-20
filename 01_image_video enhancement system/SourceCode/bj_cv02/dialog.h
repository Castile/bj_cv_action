#ifndef DIALOG_H
#define DIALOG_H
#include<opencv2/opencv.hpp>

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
public slots:
    void OpenCamera_clicked();
    void Enhance_clicked();
    void on_originalImg_clicked();
    void on_processedImg_clicked();
//    void on_btnPauseOrResume_clicked();
private slots:


    void on_btnPauseOrResume_clicked();

private:
    Ui::Dialog *ui;
    cv::VideoCapture capture;

    cv::Mat matOriginal;
    cv::Mat matProcessed;

    QImage *qimgOriginal;
    QImage qimgProcessed;
    QTimer *tmrTimer1;
    QTimer *tmrTimer;
    QTimer *tmrTimer2;



};
#endif // DIALOG_H
