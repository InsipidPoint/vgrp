#include "Camera.h"
#include <cstdio>
#include <iostream>
#include <fstream>

const char *WINDOW_NAME = "Display Window";
const char *OUTPUT_FILE = "groundtruth.txt";
std::ofstream filestream;
int click_count;
IplImage *small_img;
static CvScalar colors[] = 
  {
      {{0,0,255}},
      {{0,128,255}},
      {{0,255,255}},
      {{0,255,0}},
      {{255,128,0}},
      {{255,255,0}},
      {{255,0,0}},
      {{255,0,255}}
  };

void RecordClick(int event, int x, int y, int flags, void *param) {
  if (event == CV_EVENT_LBUTTONDOWN) {
    std::cout << "," << x << "," << y;
    filestream << "," << x << "," << y;  
    click_count++;
    cvCircle(small_img, cvPoint(x,y), 1, colors[3], 3, 8, 0);
    cvShowImage(WINDOW_NAME, small_img);
  }
}

int main(int argc, char **argv) {
  const char *filename = NULL;
  if(argc > 1)
    filename = argv[1];

  filestream.open(OUTPUT_FILE);
  if (!filestream.is_open()) {
    std::cout << "FILE OPEN FAIL" << std::endl;
  }
  
  cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  cvSetMouseCallback(WINDOW_NAME, RecordClick);
  
  const IplImage *current_frame;
  //IplImage *gray = cvCreateImage(cvSize(640,480), 8, 1);
  small_img = cvCreateImage(cvSize(640,480), 8, 3);
  Camera cam(filename);

  int frame = 1;
  int corrupt = 0;
  while((current_frame = cam.GetFrame())) {

    click_count = 0;
    corrupt = 0;
    std::cout << frame;
    filestream << frame;

    cvResize(current_frame, small_img, CV_INTER_LINEAR);
    cvFlip(small_img, small_img, 1);
    //cvCvtColor(small_img, gray, CV_BGR2GRAY);
    //cvEqualizeHist(gray,gray);
    

    cvShowImage(WINDOW_NAME, small_img);
    int key;
    while (true) {
      key = cvWaitKey(10);
      if (key == 'n' || key == 'q') break;
      if (key == 'z') corrupt = 1;
      if (click_count >= 9) break;
    }
    if (key == 'q') break;
    
    frame++;
    std::cout << "," << corrupt << std::endl;
    filestream << "," << corrupt << std::endl;
  }
  
  filestream.close();
  //cvReleaseImage( &gray );
  cvReleaseImage( &small_img );
  return 0;
}
