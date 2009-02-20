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
} Features;

class Detector {
public:
  Detector();
  ~Detector();
  
  Features ColdStart(IplImage *img);
private:
  void FindFace(IplImage *img, Features& features);
  void FindLips(IplImage *face_img, Features& features);
	void FindNostrils(IplImage *face_img, Features &features);
	void FindNoseBridge(IplImage *face_img, Features& features);
	void FindPupils(IplImage *face_img, Features& features);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
};