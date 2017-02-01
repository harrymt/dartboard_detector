// header inclusion
#include <stdio.h>
#include <opencv2/opencv.hpp>


using namespace cv;

void convolve(
	cv::Mat &input,
	cv::Mat kernel,
	cv::Mat &blurredOutput);

void getMag (
	cv::Mat dx,
	cv::Mat dy,
	cv::Mat &output);

void getDir (
	cv::Mat dx,
	cv::Mat dy,
	cv::Mat &output);

void normalizeMat (
	cv::Mat &in,
	double factor);

int main( int argc, char** argv )
{

 // LOADING THE IMAGE
 char* imageName = argv[1];
 char* dxOutputName = argv[2];
 char* dyOutputName = argv[3];
 char* magOutputName = argv[4];
 char* dirOutputName = argv[5];

 Mat image;
 image = imread( imageName, CV_LOAD_IMAGE_GRAYSCALE );

 // CONVERT COLOUR, BLUR AND SAVE
 Mat _64fimage;
 image.convertTo(_64fimage, CV_64F);

 Mat dxkernel = (Mat_<int>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
 Mat dykernel = (Mat_<int>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

 Mat dx;
 Mat dy;
 Mat mag;
 Mat dir;
 convolve(_64fimage, dxkernel, dx);
 convolve(_64fimage, dykernel, dy);
 getMag (dx, dy, mag);
 getDir (dx, dy, dir);


 normalizeMat (dx, 128.0);
 normalizeMat (dy, 128.0);
 normalizeMat (dir, CV_PI/2);
 dir = dir*(255/CV_PI);
 imwrite( dxOutputName, dx);
 imwrite( dyOutputName, dy);
 imwrite( magOutputName, mag);
 imwrite( dirOutputName, dir);

 return 0;
}

void convolve(cv::Mat &input, cv::Mat kernel, cv::Mat &output)
{
	// intialise the output using the input
	output.create(input.size(), CV_64F);

	// we need to create a padded version of the input
	// or there will be border effects
	int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
	int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

	cv::Mat paddedInput;
	cv::copyMakeBorder( input, paddedInput, 
		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
		cv::BORDER_REPLICATE );

	// now we can do the convoltion
	for ( int i = 0; i < input.rows; i++ )
	{	
		for( int j = 0; j < input.cols; j++ )
		{
			double sum = 0.0;
			for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ )
			{
				for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ )
				{
					// find the correct indices we are using
					int imagex = i + m + kernelRadiusX;
					int imagey = j + n + kernelRadiusY;
					int kernelx = m + kernelRadiusX;
					int kernely = n + kernelRadiusY;

					// get the values from the padded image and the kernel
					double imageval = ( double ) paddedInput.at<double>( imagex, imagey );
					int kernalval = kernel.at<int>( kernelx, kernely );

					// do the multiplication
					sum += imageval * kernalval;							
				}
			}
			// set the output value as the sum of the convolution
			output.at<double>(i, j) = (double) sum;
		}
	}
}

void getDir(cv::Mat dx, cv::Mat dy, cv::Mat &output) {
	output.create(dx.size(), dx.type());
	for ( int i = 0; i < dx.rows; i++ )
	{	
		for( int j = 0; j < dy.cols; j++ )
		{
			if (dy.at<double>(i,j) != 0 && dx.at<double>(i,j) != 0) {
				output.at<double>(i,j) = atan2 (dy.at<double>(i,j), dx.at<double>(i,j));
			} else {
				output.at<double>(i,j) = (double) atan (0);
			}
		}
	}
}

void getMag (cv::Mat dx, cv::Mat dy, cv::Mat &output) {
	output.create(dx.size(), dx.type());
	for ( int i = 0; i < dx.rows; i++ )
	{	
		for( int j = 0; j < dy.cols; j++ )
		{
			output.at<double>(i,j) = sqrt ((dx.at<double> (i,j) * dx.at<double> (i,j)) + (dy.at<double>(i,j) * dy.at<double>(i,j)));
		}
	}
}

void normalizeMat (cv::Mat &in, double factor) {
	for (int i=0;i<in.rows;i++) {
		for (int j=0;j<in.cols;j++) {
			in.at<double>(i,j) = (double) in.at<double>(i,j)/2 + factor;
		}
	}
}