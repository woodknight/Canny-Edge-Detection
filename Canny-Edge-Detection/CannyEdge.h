#pragma once
void GaussianFilter(const float *src, float *dst, const int w, const int h);
void SobelEdge(const float *image, float *edgeAmp, float *edgeAngle, const int numOfRows, const int numOfCols);