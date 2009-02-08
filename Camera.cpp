#include "Camera.h"

Camera::Camera(const char *filename) {
  if(filename)
    camera = cvCaptureFromAVI(filename);
  else
    camera = cvCreateCameraCapture (CV_CAP_ANY);
    
  if(!camera)
    abort();
}

Camera::~Camera() {
  cvReleaseCapture(&camera);
}

const IplImage * Camera::GetFrame() {
  return cvQueryFrame(camera);
}