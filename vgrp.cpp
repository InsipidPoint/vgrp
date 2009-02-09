#include "Camera.h"

const char *WINDOW_NAME = "Display Window";

int main(int argc, char **argv) {
  const char *filename = NULL;
  if(argc > 1)
    filename = argv[1];
  
  cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  
  const IplImage *current_frame;
  Camera cam(filename);
  
  while((current_frame = cam.GetFrame())) {
    cvShowImage(WINDOW_NAME, current_frame);
    cvWaitKey(50);
  }
  
  return 0;
}