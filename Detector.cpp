#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "math.h"

Detector::Detector() {
  cascade = (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0);
	storage = cvCreateMemStorage(0);
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
  printf("%d %d %d ***\n",features.eyebrow_ends[1].x,features.eyebrow_ends[1].y, features.face_size);
  
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
	
//	FitModel(features, model, theta);
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
	
	printf("%d %d %d **\n",features.eyebrow_ends[1].x,features.eyebrow_ends[1].y, features.face_size);
}
