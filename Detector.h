#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

typedef struct _features {
  int face_size;
  CvPoint face_position;
  CvPoint lip_positions[2];
} Features;

class Detector {
public:
  Detector();
  ~Detector();
  
  Features ColdStart(IplImage *img);
private:
  void FindFace(IplImage *img, Features& features);
  void FindLips(IplImage *face_img, Features& features);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
};