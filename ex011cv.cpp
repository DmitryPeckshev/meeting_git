
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <ctime>
#include <omp.h>
using namespace cv;

int main(int argc, char** argv)
{
	unsigned int start_time = clock();
	
	IplImage* model_image = 0;
	IplImage* current_image = 0;
	IplImage* show_image = 0;
	char* model_filename = argc == 2 ? argv[1] : "model.jpg";
	char* current_filename = argc == 2 ? argv[1] : "current.jpg";
	model_image = cvLoadImage(model_filename, 0);
	current_image = cvLoadImage(current_filename, 0);
	show_image = cvLoadImage(current_filename, 1);

	Mat model_matrix = model_image;
	Mat current_matrix = current_image;

	if (!model_matrix.data || !current_matrix.data)
	{
		std::cout << "Could not open or find the images!\n" << std::endl;
		return 0;
	}
	else {
		std::cout << "Images load successfully!" << std::endl;
		std::cout << "\nModel image width = " << model_matrix.cols << std::endl;
		std::cout << "Model image height = " << model_matrix.rows << std::endl;
		std::cout << "Current image width = " << current_matrix.cols << std::endl;
		std::cout << "Current image height = " << current_matrix.rows << std::endl;
	}

	double model_sum = 0;								
	double max = 0;										
	int xx, yy;											
	int n = current_matrix.cols - model_matrix.cols;	
	int m = current_matrix.rows - model_matrix.rows;	
	double *result = new double[n + 1, m + 1];			

	int step_x = 15;		
	int step_y = 15;		
	
	for (int i = 1; i < model_matrix.cols; i++)
	{
		for (int j = 1; j < model_matrix.rows; j++)
		{
			int mod_elem_square = (model_matrix.at<uchar>(j, i))*(model_matrix.at<uchar>(j, i));

			model_sum += mod_elem_square;
		}
	}	

	for (int x = 1; x <= n; x += step_x) {
		for (int y = 1; y <= m; y += step_y)
		{
			double numerator = 0;				
			double second_denominator = 0;		
			
			for (int i = 1; i < model_matrix.cols; i++) {
				for (int j = 1; j < model_matrix.rows; j++)
				{
					numerator = numerator + ((model_matrix.at<uchar>(j, i))*(current_matrix.at<uchar>(j + y, i + x)));
				}
			}		

			for (int i = 1; i < model_matrix.cols; i++) {
				for (int j = 1; j < model_matrix.rows; j++)
				{
					second_denominator = second_denominator + ((current_matrix.at<uchar>(j + y, i + x))*(current_matrix.at<uchar>(j + y, i + x)));
				}
			}
	
			result[x, y] = numerator / (sqrt(model_sum * second_denominator));

			if (result[x, y] > max)
			{
				max = result[x, y];
				xx = x;
				yy = y;
			}
		}
	}  

	unsigned int end_time = clock();
	float run_time = end_time - start_time;	

	std::cout << "Step x = " << step_x << " Step y = " << step_y << std::endl;
	std::cout << "\nCalculation ended successfully!" << std::endl;
	std::cout << "\nx = " << xx << "; y = " << yy << std::endl;
	std::cout << "overlap = " << round(max * 10000) / 100 << "%" << std::endl;
	std::cout << "runtime = " << run_time/1000 << " sec" << std::endl;

	cvRectangle(show_image, cvPoint(xx, yy), cvPoint(xx + model_matrix.cols , yy + model_matrix.rows), CV_RGB(255, 0, 0), 1, CV_AA);
	cvNamedWindow("Result", CV_WINDOW_AUTOSIZE |CV_WINDOW_KEEPRATIO);
	cvShowImage("Result", show_image);
	cvShowImage("model", model_image);

	waitKey();
	return 0;
}