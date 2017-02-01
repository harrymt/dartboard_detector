// header inclusion
#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;

void threshold (
	cv::Mat &input,
	double value);

void makeHoughSpace (
	cv::Mat threshMag,
	cv::Mat dir,
	int rmin,
	int rmax,
	cv::Mat &output,
	cv::Mat &output2);

void denormalise(
	cv::Mat &in);

int main( int argc, char** argv )
{

 // LOADING THE IMAGE
 char* magInputName = argv[1];
 char* dirInputName = argv[2];
 char* houghOutputName = argv[3];

 Mat mag;
 mag = imread( magInputName, CV_LOAD_IMAGE_GRAYSCALE );
 mag.convertTo(mag, CV_64F);

 Mat dir;
 dir = imread( dirInputName, CV_LOAD_IMAGE_GRAYSCALE );
 dir.convertTo(dir, CV_64F);
 denormalise (dir);

 threshold (mag, 200);

 int rmin = 10;
 int rmax = 200;
 Mat houghSpace;
 Mat squashedSpace;
 makeHoughSpace (mag, dir, rmin, rmax, houghSpace, squashedSpace);

 Mat houghOut;
 houghOut = mag;
 imwrite( houghOutputName, houghOut);

 return 0;
}

void threshold(cv::Mat &input, double value)
{
	for ( int i = 0; i < input.rows; i++ )
	{	
		for( int j = 0; j < input.cols; j++ )
		{
			if (input.at<double>(i,j) < value) {input.at<double>(i,j) = (double) 0;}
			else {input.at<double>(i,j) = (double) 255;}
		}
	}
}

void denormalise(cv::Mat &in) {

	in = in * (CV_PI/255);

	for (int i=0;i<in.rows;i++) {
		for (int j=0;j<in.cols;j++) {
			in.at<double>(i,j) = (double) in.at<double>(i,j) * 2 - CV_PI;
		}
	}
}

void makeHoughSpace(cv::Mat threshMag, cv::Mat dir, int rmin, int rmax, cv::Mat &output, cv::Mat &output2) {
	int sizes[] = {threshMag.rows,threshMag.cols,rmax-rmin};
	output.create(3, sizes, CV_32S);
	output2.create (threshMag.rows, threshMag.cols, CV_32S);
	for (int x=0;x<threshMag.rows;x++) {
		for (int y=0;y<threshMag.cols;y++) {
			if (threshMag.at<double>(x,y) > 0) {
				for (int r=0;r<(rmax-rmin);r++) {
					/*
					increment H(x0,y0,r) for every x0 and y0 taken from:
					1. x0 = x + rcos(), y0 = y + rsin()
					2. x0 = x - rcos(), y0 = y - rsin()
					3. x0 = x + rcos(), y0 = y - rsin()
					4. x0 = x - rcos(), y0 = y + rsin()
					*/

					//printf("%d,%d,%d\n", x,y,r);

					for (int i=-1;i<2;i+=2) {
						for (int j=-1;j<2;j+=2) {
							int x0 = (int) (x + i*r*cos(dir.at<double>(x,y)));
							int y0 = (int) (y + j*r*sin(dir.at<double>(x,y)));
							if ((x0 > 0 && x0 < threshMag.rows) && (y0 > 0 && y0 < threshMag.cols)) {
								output.at<int>(x0,y0,r) += 1;
							}
						}
					}
				}
			}
		}
	}

	for (int x=0;x<threshMag.rows;x++) {
		for (int y=0;y<threshMag.cols;y++) {
			for (int r=0;r<(rmax-rmin);r++) {
				output2.at<int>(x,y) += output.at<int>(x,y,r);
			}
		}
	}
}