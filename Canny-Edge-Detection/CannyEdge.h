#ifndef _CANNYEDGE_H
#define _CANNYEDGE_H
#endif

void GaussianFilter(const unsigned char *src, float *dst, const int w, const int h);
void SobelEdge(const float *image, float *edgeAmp, int *edgeAngle, const int numOfRows, const int numOfCols);
void NonMaxSuppression(const float *edgeAmp, const int *edgeAngle, float *imgSuppressed, const int numOfRows, const int numOfCols);
