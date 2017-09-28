#include "CannyEdge.h"
#include "MatArray.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat img = imread("valve.tif", IMREAD_GRAYSCALE);
	imshow("image", img);
	// waitKey();	

	int h = img.rows;
	int w = img.cols;
	
	float *imageArray = new float[w * h];
	CpyMatToArray(img, imageArray);
	
	float *blurred = new float[w*h];
	GaussianFilter(imageArray, blurred, w, h);

	Mat blurredM = img.clone();
	CpyArrayToMat(blurredM, blurred);
	imshow("gaussian blur", blurredM);
	waitKey();
	return 0;
}