#include <stdio.h>  
#include <opencv/highgui.h>  
#include <time.h>  
#include <opencv2/opencv.hpp>  
#include <opencv/cv.h>  
#include <iostream> 

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{

	Mat srcImage;
	Mat resizeImage;
	Mat grayImage;
	Mat equalizeHistImage;
	Mat contrastandbrightImage;
	Mat sobelImage;
	Mat thresholdImage;
	Mat dilateImage;
	Mat areaImage;
	Mat ellipseImage;
	srcImage = imread("3.jpg");
	Size dsize = Size(srcImage.cols*0.3, srcImage.rows*0.3);
	resize(srcImage, resizeImage, dsize);
	cvtColor(resizeImage, grayImage, CV_BGR2GRAY);
	equalizeHist(grayImage, equalizeHistImage);
	////���ȡ��Աȶ���ǿ
	contrastandbrightImage = Mat::zeros(equalizeHistImage.size(), equalizeHistImage.type());
	for (int y = 0; y < equalizeHistImage.rows; y++)
	{
		for (int x = 0; x <equalizeHistImage.cols; x++)
		{
			contrastandbrightImage.at<uchar>(y, x) = saturate_cast<uchar>(6 * (equalizeHistImage.at<uchar>(y, x)));
		}
	}
	blur(contrastandbrightImage, contrastandbrightImage, Size(3, 3));
	//Sobel(contrastandbrightImage, sobelImage, CV_8U, 0,1 ,1, 1, 0, BORDER_DEFAULT);
	threshold(contrastandbrightImage, thresholdImage, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	//�������Ͳ���  
	dilate(thresholdImage, dilateImage, element);
	dilate(dilateImage, dilateImage, element);
	dilate(dilateImage, dilateImage, element);
	dilate(dilateImage, dilateImage, element);
	dilate(dilateImage, dilateImage, element);

	//dilateImage.copyTo(areaImage);
	//vector< vector< Point> > contours;  
	//findContours(areaImage,contours,CV_RETR_TREE,  CV_CHAIN_APPROX_NONE); 
	//vector<vector<Point> >::iterator itc= contours.begin();  
	//while (itc!=contours.end()) 
	//{  
	//  if( itc->size()<100)
	//  {  
	//      itc= contours.erase(itc);  
	//  }
	//   else
	//   {
	//     ++itc;  
	//  }
	//}  
	//drawContours(areaImage, contours, -1, Scalar(255), CV_FILLED);

	//������ �����ж�  һ���Ǵ�С  һ���ǳ�����ȱ�  ��Ϊ�����60�󣬳�����ȱ�1.3С�������Ƕ�ά������  ������һ���ж�
	dilateImage.copyTo(ellipseImage);
	vector<vector<Point> > twocontours;
	vector<Vec4i>twohierarchy;
	findContours(ellipseImage, twocontours, twohierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	vector<Moments> mu(twocontours.size());
	vector<Point2f> mc(twocontours.size());
	for (int k = 0; k < (int)twocontours.size(); k++)    //��������
	{
		if (int(twocontours.at(k).size()) <= 60)//�������С��60�Ĳ�����
		{
			drawContours(ellipseImage, twocontours, k, Scalar(0), CV_FILLED);
		}
		else
		{
			RotatedRect rRect = fitEllipse(twocontours.at(k));
			double majorAxis = rRect.size.height > rRect.size.width ? rRect.size.height : rRect.size.width;
			double minorAxis = rRect.size.height > rRect.size.width ? rRect.size.width : rRect.size.height;
			float rate = majorAxis / minorAxis;
			if (rate<1.3)   //������֮��С��1.4������
			{
				//����Ϊ��ά��������ж��Ƿ�Ϊ��ά������
				printf("%f\n", rate);
				//�����������
				mu[k] = moments(twocontours[k], false);
				mc[k] = Point2d(mu[k].m10 / mu[k].m00, mu[k].m01 / mu[k].m00);
				//��ӡ����
				printf("x=%f,y=%f\n", mc[k].x, mc[k].y);
				//��ӡͼ��ĳ���
				printf("ͼ��ĳ�%d��ͼ��Ŀ�%d\n", resizeImage.cols, resizeImage.rows);
				//��������
				Point center = Point(mc[k].x, mc[k].y);
				int r = 10;
				circle(resizeImage, center, r, Scalar(255, 0, 0));
				//�ж������ǲ�����ͼ���м�   ����֮һ < ���� < ����֮��
				if ((int)mc[k].x<(int)2 * (resizeImage.cols / 3) && (int)mc[k].x>(int)(resizeImage.cols / 3))
				{
					if ((int)mc[k].y<(int)2 * (resizeImage.rows / 3) && (int)mc[k].y>(int)(resizeImage.rows / 3))
					{
						drawContours(resizeImage, twocontours, k, Scalar(255, 0, 0), CV_FILLED);//������Ϳ����ɫ
						printf("��ɫ�Ƕ�ά������\n");
					}
				}
			}
			else  //������֮�ȴ���1.4������  ���Ƕ�ά������
			{
				drawContours(resizeImage, twocontours, k, Scalar(0, 0, 255), CV_FILLED);//������Ϳ�ɺ�ɫ
			}
		}
	}
	imshow("��Сͼ", resizeImage);
	imshow("�Ҷ�ͼ", grayImage);
	imshow("�ԱȶȺ�������ǿ", contrastandbrightImage);
	//imshow("soble���",sobelImage);
	imshow("��ֵ�����", thresholdImage);
	imshow("����", dilateImage);
	//imshow("ɾ��С���",areaImage);
	imshow("������", ellipseImage);
	waitKey(0);
}
