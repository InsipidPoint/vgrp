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
} Features;

class Detector {
public:
  Detector();
  ~Detector();
  
  Features ColdStart(IplImage *img);
  void TrackFeatures(IplImage *img, Features& features);
private:
  void FindFace(IplImage *img, Features& features);
  void FindLips(IplImage *face_img, Features& features);
	void FindNostrils(IplImage *face_img, Features &features);
	void FindNoseBridge(IplImage *face_img, Features& features);
	void FindPupils(IplImage *face_img, Features& features);
	void FindEyebrowEnds(IplImage *face_img, Features& features);
	
  void GetModel(Features& features, double model[9][3]);
  void FitModel(Features& features, double model[9][3], double theta[3]);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
};