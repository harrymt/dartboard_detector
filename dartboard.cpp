/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - dartboard.cpp
//
/////////////////////////////////////////////////////////////////////////////

// header inclusion
// header inclusion
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

class Closelines {
	public:
		vector<Vec4i> lines;
		vector<Vec2i> points;
};

/** Function Headers */
vector<Rect> detectAndDisplay(Mat in, vector<Rect> boards);
vector<Vec3f> detectCircles (Mat in, vector<Vec3f> circles);
vector<Vec4i> detectLines (Mat in, vector<Vec4i> lines);
void combineresults (Mat img, vector<Rect> boards, vector<Vec3f> circles, vector<Vec4i> lines);
Closelines clusteredlines (vector<Vec4i> lines);
int countsimilar (vector<Vec2i> in, Vec2i point, int give);

/** Global variables */
String cascade_name = "dartcascade/cascade.xml";
CascadeClassifier cascade;


/** @function main */
int main( int argc, char** argv )
{
       // 1. Read Input Image
	Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
//	string outputName = argv[2];
	// 2. Load the Strong Classifier in a structure called `Cascade'
	if( !cascade.load( cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

	vector<Rect> boards;
	vector<Vec3f> circles;
	vector<Vec4i> lines;
	
	Mat circlesimg = img.clone();
	Mat boardsimg = img.clone();
	Mat linesimg = img.clone();
	
	circles = detectCircles (circlesimg, circles);
	//imwrite( "circles" + outputName, circlesimg);
	
	boards = detectAndDisplay(boardsimg, boards);
	//imwrite( "boards" + outputName, boardsimg);	
	
	lines = detectLines (linesimg, lines);
	//imwrite( "lines" + outputName, linesimg);

	Mat finalimg = img.clone();

	combineresults (finalimg, boards, circles, lines);
	
	//imwrite( "final" + outputName, finalimg);
	imwrite ("detected.jpg", finalimg);

	return 0;
}

int countsimilar (vector<Vec2i> in, Vec2i point, int give) {
	int accu = 0;
	for (int i = 0; i < in.size(); i++) {
		if (point[0] - give <= in[i][0] && point[0] + give >= in[i][0]) {
			if (point[1] - give <= in[i][1] && point [1] + give >= in[i][1]) {
				accu += 1;
			}
		}
	}
	return accu;
}

Closelines clusteredlines (vector<Vec4i> lines) {
	vector<Vec2i> points;
	vector<Vec2i> clusteredpoints;
	vector<Vec4i> clusteredlines;
	
	for (int i = 0; i < lines.size(); i++) {
		Vec4i line = lines[i];
		Vec2i point1, point2;
		point1[0] = line[0];
		point1[1] = line[1];
		point2[0] = line[2];
		point2[1] = line[3];
		points.push_back(point1);
		points.push_back(point2);
	}

	for (int i = 0; i < points.size(); i++) {
		if (countsimilar(points, points[i], 10) > 5 && count (clusteredpoints.begin(), clusteredpoints.end(), points[i]) == 0) {
			clusteredpoints.push_back (points[i]);
		}
	}

	Vec2i point1, point2;

	for (int i = 0; i < lines.size(); i++) {

		Vec4i line = lines[i];
		point1[0] = line[0];
		point1[1] = line[1];
		point2[0] = line[2];
		point2[1] = line[3];

		if (find (clusteredpoints.begin(), clusteredpoints.end() , point1) != clusteredpoints.end()
			|| find (clusteredpoints.begin(), clusteredpoints.end(), point2) != clusteredpoints.end()) {
			clusteredlines.push_back (lines[i]);
		}
	}

	Closelines ret;

	ret.lines = clusteredlines;
	ret.points = clusteredpoints;

	return ret;

}

void combineresults (Mat img, vector<Rect> boards, vector<Vec3f> circles, vector<Vec4i> inlines) {
	vector<Rect> finalboards;
	vector<Vec3f> finalcircles;
	vector<Vec4i> lines;
	vector<Vec4i> finallines;
	vector<Vec2i> finalpoints;
	vector<Vec2i> points;

	lines = inlines;
	Closelines cluster;

	cluster = clusteredlines (lines);

	lines = cluster.lines;
	points = cluster.points;

	for(int i = 0; i < boards.size(); i++) {
		for(int j = 0; j < circles.size(); j++) {

			int centrex = circles[j][0];
			int centrey = circles[j][1];

			int bxmin = boards[i].x;
			int bxmax = boards[i].x + boards[i].width;
			int bymin = boards[i].y;
			int bymax = boards[i].y + boards[i].height;

			//printf ("circle %d: (%d, %d)\nboard %d:\n(%d, %d)\n(%d, %d)\n", j, centrex, centrey, i, bxmin, bymax, bxmax, bymin);

			if (centrex > boards[i].x && centrex < (boards[i].x + boards[i].width)) {
				if (centrey > boards[i].y && centrey < (boards[i].y + boards[i].height)) {
					finalboards.push_back (boards[i]);
					finalcircles.push_back (circles[j]);
				}
			}
		}

		for (int k = 0; k < points.size(); k++) {
			int bxmin = boards[i].x;
			int bxmax = boards[i].x + boards[i].width;
			int bymin = boards[i].y;
			int bymax = boards[i].y + boards[i].height;

			Vec2i point = points [k];

			//printf ("(%d, %d)point\n(%d, %d)bleft\n(%d, %d)bright\n", point[0], point[1], bxmin, bymin, bxmax, bymax);
			if (point[0] > boards[i].x && point[0] < (boards[i].x + boards[i].width)) {
				if (point[1] > boards[i].y && point[1] < (boards[i].y + boards[i].height)) {
					finalboards.push_back (boards[i]);
					finalpoints.push_back (points[k]);
				}
			}

		}
	} 

	Vec2i point1, point2;

	for (int i = 0; i < lines.size(); i++) {
		Vec4i line = lines[i];

		point1[0] = line[0];
		point1[1] = line[1];
		point2[0] = line[2];
		point2[1] = line[3];

		if (find (finalpoints.begin(), finalpoints.end() , point1) != finalpoints.end()
			|| find (finalpoints.begin(), finalpoints.end(), point2) != finalpoints.end()) {
			finallines.push_back (lines[i]);
		}
	}

	for(int i = 0; i < finalboards.size(); i++)
	{
		rectangle(img, Point(finalboards[i].x, finalboards[i].y), Point(finalboards[i].x + finalboards[i].width, finalboards[i].y + finalboards[i].height), Scalar( 0, 255, 0 ), 2);
	}

	/* display final circles and lines as well as boxes

	for(int i = 0; i < finalcircles.size(); i++) {
		Point center(cvRound(finalcircles[i][0]), cvRound(finalcircles[i][1]));
		int radius = cvRound(finalcircles[i][2]);
		// circle center
		circle(img, center, 3, Scalar(0,255,0), -1, 8, 0);
		// circle outline
		circle(img, center, radius, Scalar(0,0,255), 3, 8, 0);
	}

	for( int i = 0; i < finallines.size(); i++ ) {
  		Vec4i l = finallines[i];
  		line(img, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	}

	*/

}

vector<Vec3f> detectCircles (Mat in, vector<Vec3f> circles) {
	Mat in_gray;

	cvtColor(in, in_gray, CV_BGR2GRAY);

	HoughCircles(in_gray, circles, CV_HOUGH_GRADIENT, 1, in_gray.rows/8, 200, 100, 0, 0);

	for(int i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(in, center, 3, Scalar(0,255,0), -1, 8, 0);
		// circle outline
		circle(in, center, radius, Scalar(0,0,255), 3, 8, 0);
	}

	return circles;
}

vector<Vec4i> detectLines (Mat in, vector<Vec4i> lines) {
	Mat in_gray;

	cvtColor(in, in_gray, CV_BGR2GRAY);
	equalizeHist(in_gray, in_gray);

	Mat edges, coledges;
	Canny(in, edges, 100, 200, 3);
	cvtColor(edges, coledges, CV_GRAY2BGR);

	HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10 );

	for( size_t i = 0; i < lines.size(); i++ ) {
  		Vec4i l = lines[i];
  		line(coledges, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	}

	in = in + coledges;

	return lines;	

}

/** @function detectAndDisplay */
vector<Rect> detectAndDisplay(Mat in, vector<Rect> boards)
{
	Mat in_gray;

	// 1. Prepare Image by turning it into Grayscale and normalising lighting
	cvtColor(in, in_gray, CV_BGR2GRAY);
	equalizeHist(in_gray, in_gray);

	// 2. Perform Viola-Jones Object Detection 
	cascade.detectMultiScale(in_gray, boards, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500));

       // 3. Print number of Faces found
	//std::cout << boards.size() << std::endl;

       // 4. Draw box around faces found
	for(int i = 0; i < boards.size(); i++)
	{
		rectangle(in, Point(boards[i].x, boards[i].y), Point(boards[i].x + boards[i].width, boards[i].y + boards[i].height), Scalar( 0, 255, 0 ), 2);
	}

	return boards;
}
