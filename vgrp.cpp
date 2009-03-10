#include "Camera.h"
#include "Detector.h"
#include <cstdio>

const char *WINDOW_NAME = "Display Window";

void DrawFace(IplImage *img, Features &f) {
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
  
  CvPoint center;
  int radius;
  center.x = cvRound(f.face_position.x + 0.5*f.face_size);
  center.y = cvRound(f.face_position.y + 0.5*f.face_size);
  radius = cvRound(f.face_size*0.5);
  cvCircle( img, center, radius, colors[0], 3, 8, 0 ); // draw face
  
  // draw lips
  cvCircle(img, cvPoint(f.lip_positions[0].x,f.lip_positions[0].y), 1, colors[1], 3, 8, 0);
  cvCircle(img, cvPoint(f.lip_positions[1].x,f.lip_positions[1].y), 1, colors[1], 3, 8, 0);
  
	// draw nostrils
	cvCircle(img, cvPoint(f.nostril_positions[0].x,f.nostril_positions[0].y), 1, colors[0], 3, 8, 0);
	cvCircle(img, cvPoint(f.nostril_positions[1].x,f.nostril_positions[1].y), 1, colors[0], 3, 8, 0);
	
	// draw nose bridge
	cvCircle(img, cvPoint(f.nose_bridge.x,f.nose_bridge.y), 1, colors[2], 3, 8, 0);

	// draw pupils
	cvCircle(img, cvPoint(f.pupils[0].x,f.pupils[0].y), 1, colors[3], 3, 8, 0);
	cvCircle(img, cvPoint(f.pupils[1].x,f.pupils[1].y), 1, colors[3], 3, 8, 0);

	// draw eyebrow ends
	cvCircle(img, cvPoint(f.eyebrow_ends[0].x,f.eyebrow_ends[0].y), 1, colors[4], 3, 8, 0);
	cvCircle(img, cvPoint(f.eyebrow_ends[1].x,f.eyebrow_ends[1].y), 1, colors[4], 3, 8, 0);

}

int main(int argc, char **argv) {
  const char *filename = NULL;
  if(argc > 1)
    filename = argv[1];
  
  cvNamedWindow (WINDOW_NAME, CV_WINDOW_AUTOSIZE);
  
  const IplImage *current_frame;
  IplImage *gray = cvCreateImage(cvSize(640,480), 8, 1);
  IplImage *small_img = cvCreateImage(cvSize(640,480), 8, 3);
  Camera cam(filename);
  Detector detector;
  Features f;
  bool track = false;
  double model[9][3];
  double theta[3] = {0,0,0};
  
  while((current_frame = cam.GetFrame())) {
    cvResize(current_frame, small_img, CV_INTER_LINEAR);
    cvFlip(small_img, small_img, 1);
    cvCvtColor(small_img, gray, CV_BGR2GRAY);
    cvEqualizeHist(gray,gray);
    
    if(track) {
      // tmp
//      f = detector.ColdStart(gray);
      detector.TrackFeatures(gray, f, model, theta);
		printf("%d %d\n",f.horiz_slope,f.vert_slope);
    } else {
      f = detector.ColdStart(gray);
      detector.GetModel(f, model);
//		printf("%d %d\n",f.eyebrow_ends[1].x,f.eyebrow_ends[1].y);
    }
    
    if(f.face_size) {
      DrawFace(small_img, f);
    }

    cvShowImage(WINDOW_NAME, small_img);
	  if(cvWaitKey(10) == 't') {
		  printf("Here\n");
		  track = true;
		  detector.SetupTracking(gray,f);
	  }
  }
  
  cvReleaseImage( &gray );
  cvReleaseImage( &small_img );
  return 0;
}
