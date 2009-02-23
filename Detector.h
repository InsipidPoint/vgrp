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
	
  void TrackLips(IplImage *img, Features& features);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
};