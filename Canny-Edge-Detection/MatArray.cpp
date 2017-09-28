#include "MatArray.h"

void CpyArrayToMat(Mat& I, const float* const array)
{
	// accept only grayscale char type matrices
	CV_Assert(I.depth() == CV_8U);
	CV_Assert(I.channels() == 1);

	MatIterator_<uchar> it, end;
	int i = 0;
	for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it, ++i)
		*it = (uchar)array[i];
}

void CpyMatToArray(Mat I, float *array)
{
	// accept only grayscale char type matrices
	CV_Assert(I.depth() == CV_8U);
	CV_Assert(I.channels() == 1);
	MatIterator_<uchar> it, end;
	int i = 0;
	for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it, ++i)
		array[i]  = (uchar)*it;
}

