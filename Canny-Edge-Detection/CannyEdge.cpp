#include <cmath>
#include <iostream>

#include "CannyEdge.h"

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

void SobelEdge(const float *image, float *G, int *edgeAngle, const int numOfRows, const int numOfCols)
{
	// This function convolve the image with a Sobel filter in one dimention using
	// separable convolution, see(https://blogs.mathworks.com/steve/2006/10/04/separable-convolution/)
	// and (https://en.wikipedia.org/wiki/Sobel_operator)
	// image is numOfRows*numOfCols matrix rearranged to 1D array in row-major order	

	float *Gx = new float[numOfRows * numOfCols]; // gradient along x
	float *Gy = new float[numOfRows * numOfCols]; // gradient along y	

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
	for (int i = 0; i < numOfRows; i++) // 2 to numOfClos -1 columns
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
		G[index] = abs(Gx[index]) + abs(Gy[index]);

		index = (numOfRows - 1) * numOfCols + j;
		Gx[index] = B[0] * g_1[index - numOfCols] + B[1] * g_1[index];
		Gy[index] = A[0] * g_2[index - numOfCols] + A[1] * g_2[index];
		G[index] = abs(Gx[index]) + abs(Gy[index]);
	}
	for (int i = 1; i < numOfRows - 1; i++) // 2 to numOfRows - 1 rows
	{
		for (int j = 0; j < numOfCols; j++)
		{
			index = i * numOfCols + j;
			Gx[index] = B[0] * g_1[index - numOfCols] + B[1] * g_1[index] + B[2] * g_1[index + numOfCols];
			Gy[index] = A[0] * g_2[index - numOfCols] + A[1] * g_2[index] + A[2] * g_2[index + numOfCols];

			G[index] = abs(Gx[index]) + abs(Gy[index]);
		}
	}

	float Gdiv = 0;
	for (int i = 0; i < numOfCols * numOfRows; i++)
	{
		/* directions
		0: 90 degree
		1: 135 degree
		2: 0 degree
		3: 45 degree
		*/
		Gdiv = Gy[i] / Gx[i];
		if (Gdiv < 0)
		{
			if (Gdiv < -2.41421356237)
				edgeAngle[i] = 0;
			else if (Gdiv < -0.414213562373)
				edgeAngle[i] = 1;
			else
				edgeAngle[i] = 2;
		}
		else
		{
			if (Gdiv > 2.41421356237)
				edgeAngle[i] = 0;
			else if (Gdiv > 0.414213562373)
				edgeAngle[i] = 3;
			else
				edgeAngle[i] = 2;
		}
	}

	delete Gx;
	delete Gy;
	delete g_1;
	delete g_2;
}

void NonMaxSuppression(const float *edgeAmp, const int *edgeAngle, float *imgSuppressed, const int h, const int w)
{
	int index = 0;
	for(int y = 1; y < h -1; y++)
		for (int x = 1; x < w - 1; x++)
		{
			index = y*w + x;
			switch (edgeAngle[index])
			{
			case 0: // 90 degree
				if (edgeAmp[index] > edgeAmp[index - w] && edgeAmp[index] > edgeAmp[index + w])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 1: // 135 degree
				if (edgeAmp[index] > edgeAmp[index - w - 1] && edgeAmp[index] > edgeAmp[index + w + 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 2: // 0 degree
				if (edgeAmp[index] > edgeAmp[index - 1] && edgeAmp[index] > edgeAmp[index + 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			case 3: // 45 degree
				if (edgeAmp[index] > edgeAmp[index - w + 1] && edgeAmp[index] > edgeAmp[index + w - 1])
					imgSuppressed[index] = edgeAmp[index];
				else
					imgSuppressed[index] = 0;
				break;
			default:
				cout << "error, edge angle out of range!" << endl;
				break;
			}

			if (imgSuppressed[index] > 255)
				imgSuppressed[index] = 255;
		}
	// set boundaries to zero
	for (int i = 0; i < w; i++)
	{
		imgSuppressed[i] = 0;
		imgSuppressed[(h - 1)*w + i] = 0;
	}
	for (int i = 1; i < h - 1; i++)
	{
		imgSuppressed[i*w] = 0;
		imgSuppressed[i*w - 1] = 0;
	}

}


