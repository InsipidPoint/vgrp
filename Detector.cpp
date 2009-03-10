#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "math.h"

Detector::Detector() {
  cascade = (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0);
	storage = cvCreateMemStorage(0);
	
	cvNamedWindow( "result", 0 );
	assert(cascade);
}

Detector::~Detector() {

}

Features Detector::ColdStart(IplImage *img) {
  Features features;
  FindFace(img,features);
  if(features.face_size) {
	  FindLips(img,features);
	  FindNostrils(img,features);
	  FindNoseBridge(img,features);
	  FindPupils(img,features);
	  FindEyebrowEnds(img,features);
    FindFaceCenter(features);
  }
  return features;
}

void Detector::TrackFeatures(IplImage *img, Features& features, double model[9][3], double theta[3]) {  
  grey = cvCloneImage(img);
	cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
						   points[0], points[1], count, cvSize(win_size,win_size), 3, status, 0,
						   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
	flags |= CV_LKFLOW_PYR_A_READY;
	int i,k;
	for( i = k = 0; i < count; i++ )
	{
		if( !status[i] )
			continue;
		points[1][k++] = points[1][i];
		
	}
	count = k;
	
	CV_SWAP( prev_grey, grey, swap_temp );
	CV_SWAP( prev_pyramid, pyramid, swap_temp );
	CV_SWAP( points[0], points[1], swap_points );	
	
	features.lip_positions[0] = cvPointFrom32f(points[0][0]);
	features.lip_positions[1] = cvPointFrom32f(points[0][1]);
	features.nostril_positions[0] = cvPointFrom32f(points[0][2]);
	features.nostril_positions[1] = cvPointFrom32f(points[0][3]);
	features.nose_bridge = cvPointFrom32f(points[0][4]);
	features.pupils[0] = cvPointFrom32f(points[0][5]);
	features.pupils[1] = cvPointFrom32f(points[0][6]);
	features.eyebrow_ends[0] = cvPointFrom32f(points[0][7]);
	features.eyebrow_ends[1] = cvPointFrom32f(points[0][8]);

  FindFaceCenter(features);
	
//	FitModel(features, model, theta);
//	FitGlasses(img,features,model,theta);
}

void Detector::SetupTracking(IplImage *img, Features& features) {	
	win_size = 10;
	MAX_COUNT = 500;
	flags = 0;
	
	/* allocate all the buffers */
	prev_grey = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	pyramid = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	prev_pyramid = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	status = (char*)cvAlloc(MAX_COUNT);
	prev_grey = cvCloneImage(img);
	
	points[0][0] =  cvPointTo32f(features.lip_positions[0]);
	points[0][1] =  cvPointTo32f(features.lip_positions[1]);
	points[0][2] =  cvPointTo32f(features.nostril_positions[0]);
	points[0][3] =  cvPointTo32f(features.nostril_positions[1]);
	points[0][4] =  cvPointTo32f(features.nose_bridge);
	points[0][5] =  cvPointTo32f(features.pupils[0]);
	points[0][6] =  cvPointTo32f(features.pupils[1]);
	points[0][7] =  cvPointTo32f(features.eyebrow_ends[0]);
	points[0][8] =  cvPointTo32f(features.eyebrow_ends[1]);
	count=9;
}

void Detector::FitGlasses(IplImage *img, Features& features, double model[9][3], double theta[3]) {
	//LR = +-theta[0]
	//UD = +-theta[1]
	//L/R rotate = +-theta[2]
	IplImage *out = cvCloneImage(img);
	CvPoint face_center;
	face_center.x = (features.nostril_positions[0].x + features.nostril_positions[1].x)/2;
	face_center.y = (features.nostril_positions[0].y + features.nostril_positions[1].y)/2;
	double delta[3] = {0.35,0.18,0.07};
	double w = delta[0]*features.face_size;
	double h = delta[1]*features.face_size;
	double l = delta[2]*features.face_size;
	std::cout<<features.horiz_slope<<std::endl;
	double scale1[2] = {1/0.11,1/0.11};
	//theta[0] = 1;
	theta[0] = (cvSqrt((features.nostril_positions[0].x-features.nostril_positions[1].x)*(features.nostril_positions[0].x-features.nostril_positions[1].x) + (features.nostril_positions[0].y-features.nostril_positions[1].y)*(features.nostril_positions[0].y-features.nostril_positions[1].y)))/features.face_size;
//	std::cout<<theta[0]<<std::endl;
	CvMat *perspective_projection = cvCreateMat(3,3,CV_32FC1);
	CvPoint nb;
	nb.x = (features.nose_bridge.x + (features.eyebrow_ends[0].x + features.eyebrow_ends[1].x)/2 + (features.nostril_positions[0].x + features.nostril_positions[1].x)/2 + (features.lip_positions[0].x + features.lip_positions[1].x)/2)/4;
	nb.y = (features.nose_bridge.y + (features.pupils[0].y + features.pupils[1].y)/2)/2;
	
	CvMat *src = cvCreateMat(2,4,CV_32FC1);
	CvMat *dst = cvCreateMat(2,4,CV_32FC1);
	cvmSet(src,0,0,nb.x-l-w);
	cvmSet(src,1,0,(features.eyebrow_ends[0].y+nb.y)/2);
	cvmSet(src,0,1,nb.x-l-w);
	cvmSet(src,1,1,(features.eyebrow_ends[0].y+nb.y)/2+h);
	cvmSet(src,0,2,nb.x+l+w);
	cvmSet(src,1,2,(features.eyebrow_ends[1].y+nb.y)/2);
	cvmSet(src,0,3,nb.x+l+w);
	cvmSet(src,1,3,(features.eyebrow_ends[1].y+nb.y)/2+h);
	
	cvmSet(dst,0,0,nb.x-l-scale1[1]*w);
	cvmSet(dst,1,0,scale1[0]*(features.eyebrow_ends[0].y+nb.y)/2);
	cvmSet(dst,0,1,nb.x-l-scale1[1]*w);
	cvmSet(dst,1,1,scale1[1]*(features.eyebrow_ends[0].y+nb.y)/2+h);
	cvmSet(dst,0,2,nb.x+l+scale1[0]*w);
	cvmSet(dst,1,2,scale1[1]*(features.eyebrow_ends[1].y+nb.y)/2);
	cvmSet(dst,0,3,nb.x+l+scale1[0]*w);
	cvmSet(dst,1,3,scale1[0]*(features.eyebrow_ends[1].y+nb.y)/2+h);
	cvFindHomography(src,dst,perspective_projection);
	
//	printf("%f\n",theta[0]);
	cvRectangle(out, cvPoint(nb.x-scale1[1]*theta[0]*l-w,scale1[0]*theta[0]*(features.eyebrow_ends[0].y+nb.y)/2), cvPoint(nb.x-scale1[1]*theta[0]*l,(features.eyebrow_ends[0].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
	cvRectangle(out, cvPoint(nb.x+scale1[0]*theta[0]*l,(features.eyebrow_ends[1].y+nb.y)/2), cvPoint(nb.x+scale1[0]*theta[0]*l+scale1[0]*theta[0]*w,scale1[1]*theta[0]*(features.eyebrow_ends[1].y+nb.y)/2+scale1[0]*theta[0]*h), cvScalar(0,0,255), 1, 8, 0);
	cvLine(out,cvPoint(nb.x-scale1[1]*theta[0]*l,(features.eyebrow_ends[0].y+nb.y)/2+h/2),cvPoint(nb.x+scale1[0]*theta[0]*l,(features.eyebrow_ends[1].y+nb.y)/2+h/2), cvScalar(0,0,255), 1, 8, 0);
	
	//	cvRectangle(out, cvPoint(nb.x-l-w,(features.eyebrow_ends[0].y+nb.y)/2), cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
	//	cvRectangle(out, cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2), cvPoint(nb.x+l+w,(features.eyebrow_ends[1].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
	//	cvLine(out,cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2+h/2),cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2+h/2), cvScalar(0,0,255), 1, 8, 0);
	
	cvShowImage("result",out);
	return;
}
