#include "Camera.h"
#include "Detector.h"
#include <cstdio>

const char *WINDOW_NAME = "Display Window";

void DrawFace(IplImage *img, Features &f, bool col = true) {
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
  
  int radius;
  radius = cvRound(f.face_size*0.5);
  
  if(col) {
    cvCircle( img, f.face_position, radius, colors[0], 3, 8, 0 ); // draw face

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
  } else {    
//    cvCircle( img, f.face_position, radius, colors[0], 3, 8, 0 ); // draw face

    // draw lips
    cvCircle(img, cvPoint(f.lip_positions[0].x,f.lip_positions[0].y), 1, colors[5], 3, 8, 0);
    cvCircle(img, cvPoint(f.lip_positions[1].x,f.lip_positions[1].y), 1, colors[5], 3, 8, 0);

  	// draw nostrils
  	cvCircle(img, cvPoint(f.nostril_positions[0].x,f.nostril_positions[0].y), 1, colors[5], 3, 8, 0);
  	cvCircle(img, cvPoint(f.nostril_positions[1].x,f.nostril_positions[1].y), 1, colors[5], 3, 8, 0);

  	// draw nose bridge
  	cvCircle(img, cvPoint(f.nose_bridge.x,f.nose_bridge.y), 1, colors[5], 3, 8, 0);

  	// draw pupils
  	cvCircle(img, cvPoint(f.pupils[0].x,f.pupils[0].y), 1, colors[5], 3, 8, 0);
  	cvCircle(img, cvPoint(f.pupils[1].x,f.pupils[1].y), 1, colors[5], 3, 8, 0);

  	// draw eyebrow ends
  	cvCircle(img, cvPoint(f.eyebrow_ends[0].x,f.eyebrow_ends[0].y), 1, colors[5], 3, 8, 0);
  	cvCircle(img, cvPoint(f.eyebrow_ends[1].x,f.eyebrow_ends[1].y), 1, colors[5], 3, 8, 0);
    
    // draw axes
    cvLine(img, cvPoint(f.face_position.x-50.0, f.face_position.y-(50.0*f.horiz_slope)), cvPoint(f.face_position.x+50.0, f.face_position.y+(50.0*f.horiz_slope)), colors[3], 1);
    cvLine(img, cvPoint(f.face_position.x-(50.0*f.vert_slope), f.face_position.y-50.0), cvPoint(f.face_position.x+(50.0*f.vert_slope), f.face_position.y+50.0), colors[3], 1);
  }
}

void draw_cross(IplImage *img, Features& f, double theta) {
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
  
  double phi = theta > 0 ? theta-1.57 : theta+1.57;
  cvLine(img, cvPoint(f.face_position.x+50.0, f.face_position.y+(50.0*phi)), cvPoint(f.face_position.x-50.0, f.face_position.y-(50.0*phi)), colors[0], 1);
  cvLine(img, cvPoint(f.face_position.x-(50.0*tan(theta)), f.face_position.y-50.0), cvPoint(f.face_position.x+(50.0*tan(theta)), f.face_position.y+50.0), colors[0], 1);
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
  double theta = 0;
  
  while((current_frame = cam.GetFrame())) {
    cvResize(current_frame, small_img, CV_INTER_LINEAR);
    cvFlip(small_img, small_img, 1);
    cvCvtColor(small_img, gray, CV_BGR2GRAY);
    cvEqualizeHist(gray,gray);
    
    if(track) {
      detector.TrackFeatures(gray, f, model, theta);
      DrawFace(small_img, f, false);
      detector.FitModel(f, model, theta);
      
      CvFont font;
      double hScale=0.5;
      double vScale=0.5;
      int    lineWidth=1;
      cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX, hScale,vScale,0,lineWidth);
      char buf[50];
      sprintf(buf,"tz:%f",theta);
      cvPutText (small_img,buf,cvPoint(200,400), &font, cvScalar(255,255,0));
      draw_cross(small_img,f,theta);
    } else {
      f = detector.ColdStart(gray);
    }
    
    if(f.face_size) {
      DrawFace(small_img, f);
    }

    cvShowImage(WINDOW_NAME, small_img);
	  if(cvWaitKey(10) == 't') {
		  track = true;
		  detector.GetModel(f, model);
		  detector.SetupTracking(gray,f);
	  }
  }
  
  cvReleaseImage( &gray );
  cvReleaseImage( &small_img );
  return 0;
}
