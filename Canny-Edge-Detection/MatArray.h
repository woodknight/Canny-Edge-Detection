#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

void CpyArrayToMat(Mat& I, const float* const array);
void CpyMatToArray(Mat I, float *array);