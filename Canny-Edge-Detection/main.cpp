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
		
	float *edgeAmp = new float[w*h];
	int *edgeAngle = new int[w*h];
	SobelEdge(blurred, edgeAmp, edgeAngle, h, w);

	float *suppressed = new float[w*h];
	NonMaxSuppression(edgeAmp, edgeAngle, suppressed, h, w);
	
	// display images
	CImg<unsigned char> imageBlurred(blurred, w, h);
	CImg<unsigned char> imageEdgeAmp(edgeAmp, w, h);
	CImg<unsigned char> imageSuppressed(suppressed, w, h);

	// imageEdgeAmp.save("edge.bmp");

	CImgDisplay src_disp(img, "source");
	CImgDisplay blurred_disp(imageBlurred, "blurred");
	CImgDisplay edgeAmp_disp(imageEdgeAmp, "edge amp");
	CImgDisplay suppressed_disp(imageSuppressed, "suppressed");
	while (!src_disp.is_closed())
	{
		src_disp.wait();
	}
	
	delete blurred;
	delete edgeAmp;
	delete edgeAngle;
	delete suppressed;

	return 0;
}