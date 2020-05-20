#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

/*
低照度；不均匀光照；雾天；雨天
实现多种图像增强算法：
1.Retinex（低光照，不均匀光照）IR算法；AIADANE
2.退化模型（去雾去雨）
3.暗通道
4.直方图均衡
5.基于暗原色（和3一样？？？）
6.基于偏振光
*/


//3.求暗通道
Mat darkChannel(Mat src)
{
    Mat rgbmin = Mat::zeros(src.rows, src.cols, CV_8UC1);
    Mat dark = Mat::zeros(src.rows, src.cols, CV_8UC1);
    Vec3b intensity;

    for (int m = 0; m < src.rows; m++)
    {
        for (int n = 0; n < src.cols; n++)
        {
            intensity = src.at<Vec3b>(m, n);
            rgbmin.at<uchar>(m, n) = min(min(intensity.val[0], intensity.val[1]), intensity.val[2]);
        }
    }

    //模板尺寸
    int scale = 7;
    //cout << "Please enter the mask scale: " << endl;
    //cin >> scale;

    //边界扩充
    int radius = (scale - 1) / 2;
    Mat border;
    //由于要求最小值，所以扩充的边界可以用复制边界填充
    copyMakeBorder(rgbmin, border, radius, radius, radius, radius, BORDER_REPLICATE);

    //最小值滤波
    for (int i = 0; i < src.cols; i++)
    {
        for (int j = 0; j < src.rows; j++)
        {
            //选取兴趣区域
            Mat roi;
            roi = border(Rect(i, j, scale, scale));

            //求兴趣区域的最小值
            double minVal = 0; double maxVal = 0;
            Point minLoc;
            Point maxLoc;
            minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc, noArray());

            dark.at<uchar>(Point(i, j)) = (uchar)minVal;
        }
    }
    return dark;
}

uchar light(vector<uchar> inputIamgeMax)
{
    uchar maxA = 0;
    for (int i = 0; i < inputIamgeMax.size() - 1; i++)
    {

        if (maxA < inputIamgeMax[i + 1])
        {
            maxA = inputIamgeMax[i + 1];
        }
    }
    return maxA;
}
//通过暗通道算法实现图像去雾
Mat getSuccesMatwithDark(Mat image, Mat darkChannel1)
{
    //估计大气光
    Mat temp;
    darkChannel1.copyTo(temp);
    vector<Point> darkMaxPoint;
    vector<uchar> inputMax;
    for (long i = 0; i < ((darkChannel1.rows*darkChannel1.cols) / 1000); i++)
    {
        double minVal = 0; double maxVal = 0;
        Point minLoc; Point maxLoc;
        minMaxLoc(temp, &minVal, &maxVal, &minLoc, &maxLoc, noArray());

        darkMaxPoint.push_back(maxLoc);
        inputMax.push_back(image.at<uchar>(maxLoc));
        circle(temp, maxLoc, 5, Scalar(0), 1, 8, 0);
        temp.at<uchar>(maxLoc) = temp.at<uchar>(minLoc);
    }

    uchar A = light(inputMax);


    double w = 0.65;

    //createTrackbar("w1", "dehazed", &w1, 100, NULL);

    //求折射率
    Mat T = Mat::zeros(image.rows, image.cols, CV_8UC3);
    Scalar intensity;

    for (int m = 0; m < image.rows; m++)
    {
        for (int n = 0; n < image.cols; n++)
        {
            intensity = darkChannel1.at<uchar>(m, n);
            T.at<Vec3b>(m, n)[0] = (1 - w * intensity.val[0] / A) * 255;
            T.at<Vec3b>(m, n)[1] = (1 - w * intensity.val[0] / A) * 255;
            T.at<Vec3b>(m, n)[2] = (1 - w * intensity.val[0] / A) * 255;
        }
    }


    //去雾
    Mat J(image.rows, image.cols, CV_8UC3, Scalar(180, 120, 50));
    Mat temp1(image.rows, image.cols, CV_8UC3, Scalar(180, 120, 50));

    //subtract(image, Scalar(A, A, A), temp1);
    temp1 = abs(image - Scalar(A, A, A));
    double t0 = 0.1;

    Scalar T1;
    Vec3b intsrc;
    for (int i = 0; i < image.cols; i++)
    {
        for (int j = 0; j < image.rows; j++)
        {
            T1 = T.at<uchar>(Point(i, j));
            intsrc = image.at<Vec3b>(Point(i, j));
            double tmax = (T1.val[0] / 255) < t0 ? t0 : (T1.val[0] / 255);

            for (int k = 0; k < 3; k++)
            {
                J.at<Vec3b>(Point(i, j))[k] = abs((intsrc.val[k] - A) / tmax + A) > 255 ? 255 : abs((intsrc.val[k] - A) / tmax + A);
            }
        }
    }
    return J;
}
//3. end
