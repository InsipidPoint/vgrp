#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

typedef struct _features {
  int face_size;
  CvPoint face_position;
  CvPoint nostril_positions[2];
  CvPoint lip_positions[2];
  CvPoint nose_bridge;
  CvPoint pupils[2];
  CvPoint eyebrow_ends[2];
  double horiz_slope;
  double vert_slope;
} Features;

class Detector {
public:
  Detector();
  ~Detector();
  
  Features ColdStart(IplImage *img);
  void TrackFeatures(IplImage *img, Features& features, double model[9][3], double theta[3]);
	void SetupTracking(IplImage *img, Features& features);
  void GetModel(Features& features, double model[9][3]);
private:
  void FindFace(IplImage *img, Features& features);
  void FindLips(IplImage *face_img, Features& features);
	void FindNostrils(IplImage *face_img, Features &features);
	void FindNoseBridge(IplImage *face_img, Features& features);
	void FindPupils(IplImage *face_img, Features& features);
	void FindEyebrowEnds(IplImage *face_img, Features& features);
	
  void FitModel(Features& features, double model[9][3], double theta[3]);
  void FindFaceCenter(Features& features);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
	
	IplImage *grey, *prev_grey, *pyramid, *prev_pyramid, *swap_temp;
	
	int win_size;
	int MAX_COUNT;
	CvPoint2D32f* points[2], *swap_points;
	char* status;
	int count;
	int flags;
	
	
};
