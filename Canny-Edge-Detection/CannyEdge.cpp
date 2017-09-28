#include "CannyEdge.h"
#include <cmath>
using namespace std;
void GaussianFilter(const unsigned char *src, float *dst, const int w, const int h)
{
	// Gaussian filter using separable convolution
	// https://blogs.mathworks.com/steve/2006/10/04/separable-convolution/
	
	float *tmp = new float[w*h];

	float filter[] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f }; //Gaussian filter, width=5, sigma=1

	// convolve along horizontal direction
	int index = 0;
	float value = 0;
	for(int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			value = 0;
			for (int k = -2; k <= 2; k++)
			{
				if (j + k >= 0 && j+k < w)
				{
					index = i*w + j + k;
					value += filter[k + 2] * src[index];
				}
			}
			tmp[i*w + j] = value;
		}

	// convolve along vertical direction
	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			value = 0;
			for (int k = -2; k <= 2; k++)
			{
				if (i + k >= 0 && i + k < h)
				{
					index = (i + k)*w + j;
					value += filter[k + 2] * tmp[index];
				}
			}
			dst[i*w + j] = value;
		}
	delete tmp;
}

void SobelEdge(const float *image, float *edgeAmp, float *edgeAngle, const int numOfRows, const int numOfCols)
{
	// This function convolve the image with a Sobel filter in one dimention using
	// separable convolution, see(https://blogs.mathworks.com/steve/2006/10/04/separable-convolution/)
	// and (https://en.wikipedia.org/wiki/Sobel_operator)
	// image is numOfRows*numOfCols matrix rearranged to 1D array in row-major order	

	float *Gx = new float[numOfRows * numOfCols]; // gradient along x
	float *Gy = new float[numOfRows * numOfCols]; // gradient along y
	
	float Gdiv = 0;

	int A[3] = { 1, 0, -1 }; // filter kernel
	int B[3] = { 1, 2, 1 };

	float *g_1 = new float[numOfRows * numOfCols]; // temporary array
	float *g_2 = new float[numOfRows * numOfCols];

	int r = 0, c = 0; // row and column index

	int index = 0;
	for (int i = 0; i < numOfRows; i++) // first and last columns
	{
		index = i * numOfCols + 0;
		g_1[index] = A[1] * image[index] + A[2] * image[index + 1];
		g_2[index] = B[1] * image[index] + B[2] * image[index + 1];

		index = i * numOfCols + numOfCols - 1;
		g_1[index] = A[0] * image[index - 1] + A[1] * image[index];
		g_2[index] = B[0] * image[index - 1] + B[1] * image[index];
	}
	for (int i = 1; i < numOfRows - 1; i++) // 2 to numOfClos -1 columns
	{
		for (int j = 1; j < numOfCols - 1; j++)
		{
			index = i * numOfCols + j;
			g_1[index] = A[0] * image[index - 1] + A[1] * image[index] + A[2] * image[index + 1];
			g_2[index] = B[0] * image[index - 1] + B[1] * image[index] + B[2] * image[index + 1];
		}
	}

	for (int j = 0; j < numOfCols; j++) // first and last rows
	{
		index = j;
		Gx[index] = B[1] * g_1[index] + B[2] * g_1[index + numOfCols];
		Gy[index] = A[1] * g_2[index] + A[2] * g_2[index + numOfCols];

		index = (numOfRows - 1) * numOfCols + j;
		Gx[index] = B[0] * g_1[index - numOfCols] + B[1] * g_1[index];
		Gy[index] = A[0] * g_2[index - numOfCols] + A[1] * g_2[index];

		edgeAmp[index] = sqrt(Gx[index]* Gx[index] + Gy[index]* Gy[index]);

		Gdiv = Gy[index] / Gx[index];
		if (Gdiv < 0) {
			if (Gdiv < -2.41421356237) 
			{
				edgeAngle[index] = 0;
			}
			else {
				if (Gdiv < -0.414213562373) 
				{
					edgeAngle[index] = 1;
				}
				else {
					edgeAngle[index] = 2;
				}
			}
		}
		else 
		{
			if (Gdiv > 2.41421356237) {
				edgeAngle[index] = 0;
			}
			else {
				if (Gdiv > 0.414213562373) {
					edgeAngle[index] = 3;
				}
				else {
					edgeAngle[index] = 2;
				}
			}
		}

	}
	for (int i = 1; i < numOfRows - 1; i++) // 2 to numOfRows - 1 rows
	{
		for (int j = 1; j < numOfCols - 1; j++)
		{
			index = i * numOfCols + j;
			Gx[index] = B[0] * g_1[index - numOfCols] + B[1] * g_1[index] + B[2] * g_1[index + numOfCols];
			Gy[index] = A[0] * g_2[index - numOfCols] + A[1] * g_2[index] + A[2] * g_2[index + numOfCols];

			edgeAmp[index] = sqrt(Gx[index] * Gx[index] + Gy[index] * Gy[index]);

			Gdiv = Gy[index] / Gx[index];
			if (Gdiv < 0) {
				if (Gdiv < -2.41421356237)
				{
					edgeAngle[index] = 0;
				}
				else {
					if (Gdiv < -0.414213562373)
					{
						edgeAngle[index] = 1;
					}
					else {
						edgeAngle[index] = 2;
					}
				}
			}
			else
			{
				if (Gdiv > 2.41421356237) {
					edgeAngle[index] = 0;
				}
				else {
					if (Gdiv > 0.414213562373) {
						edgeAngle[index] = 3;
					}
					else {
						edgeAngle[index] = 2;
					}
				}
			}
		}
	}

	delete Gx;
	delete Gy;
	delete g_1;
	delete g_2;
}



