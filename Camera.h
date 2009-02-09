#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

class Camera {
public:
  Camera(const char *filename = NULL);
  ~Camera();
  
  const IplImage *GetFrame();

private:
  CvCapture *camera;
};