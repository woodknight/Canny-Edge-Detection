#include "CannyEdge.h"
#include "CImg.h"

using namespace std;
using namespace cimg_library;

int main()
{
	CImg<unsigned char> img("valve.bmp");
	
	int h = img.height();
	int w = img.width();
	
	unsigned char *imageArray = img.data();
	
	float *blurred = new float[w*h];
	GaussianFilter(imageArray, blurred, w, h);
	
	// display images
	CImg<unsigned char> imageBlurred(blurred, w, h);
	CImgDisplay src_disp(img, "source");
	CImgDisplay blurred_disp(imageBlurred, "blurred");
	while (!src_disp.is_closed() || !blurred_disp.is_closed())
	{
		src_disp.wait();
	}
	
	return 0;
}