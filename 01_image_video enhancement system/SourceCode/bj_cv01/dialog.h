#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<QTimer>
#include "MSRCR.h"

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
private slots:
    void on_btnOpenImage_clicked();

    void on_ritnex_clicked();

    void on_dark_clicked();

    void on_Sritnex_clicked();

private:
    Ui::Dialog *ui;
    QString fileName;

    cv::Mat matOriginal;
    cv::Mat matProcessed;

    QImage *qimgOriginal;
    QImage qimgProcessed;


};
#endif // DIALOG_H
