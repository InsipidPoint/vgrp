#include "Detector.h"
#include <cstdio>

#define PIXEL(im,x,y) (((uchar *)((im)->imageData + (y)*(im)->widthStep))[(x)])

Detector::Detector() {
  cascade = (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0);
	storage = cvCreateMemStorage(0);
  assert(cascade);
}

Detector::~Detector() {

}

Features Detector::ColdStart(IplImage *img) {
  Features features;
  FindFace(img,features);
  if(features.face_size) {
    FindLips(img,features);
  }
  return features;
}

void Detector::FindFace(IplImage *img, Features& features) {
  cvClearMemStorage(storage);
  CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                      1.1, 2, CV_HAAR_DO_CANNY_PRUNING ,cvSize(30, 30) );
  if(faces && faces->total) {
      CvRect* r = (CvRect*)cvGetSeqElem(faces, 0);
      features.face_size = r->width;
      features.face_position = cvPoint(r->x,r->y);
  } else {
    features.face_size = 0;
  }
}

void Detector::FindLips(IplImage *img, Features& features) {
  int tlx = cvRound(features.face_position.x+features.face_size*0.25);
  int tly = cvRound(features.face_position.y+features.face_size-features.face_size*0.25);
  int width = cvRound(features.face_size*0.5), height = cvRound(features.face_size*0.25);
  CvRect roi = cvRect(tlx, tly, width, height);
  cvSetImageROI(img, roi);
  
  int min[width];
  int min_bin[height];
  for(int i = 0; i < height; i++) {
    min_bin[i] = 0;
  }
  
  // populate min array
  for(int x = 0; x < width; x++) {
    double cmin = 9999;
    for(int y = 0; y < height; y++) {
      if(cvGet2D(img,y,x).val[0] < cmin) {
        cmin = cvGet2D(img,y,x).val[0];
        min[x] = y;
      }
    }
    min_bin[min[x]]++;
  }
  
  // find mode
  int mode = 0;
  int ccount = 0;
  for(int i = 0; i < height; i++) {
    if(min_bin[i] > ccount) {
      mode = i;
      ccount = min_bin[i];
    }
  }
  
  // set noise points to 0
  for(int i = 0; i < width; i++) {
    if(abs(min[i]-mode) > 3)
      min[i] = 0;
  }
  
  // find left lip corner
  for(int i = 0; i < width; i++) {
    if(min[i] > 0) {
      if(min[i]+min[i+1]+min[i+2]+min[i+3]+min[i+4] > 3*mode) {
        features.lip_positions[0].x = i + tlx;
        features.lip_positions[0].y = min[i] + tly;
        break;
      }
    }
  }
  
  // find right lip corner
  for(int i = width-1; i >= 0; i--) {
    if(min[i] > 0) {
      if(min[i]+min[i-1]+min[i-2]+min[i-3]+min[i-4] > 3*mode) {
        features.lip_positions[1].x = i + tlx;
        features.lip_positions[1].y = min[i] + tly;
        break;
      }
    }
  }
  
  cvResetImageROI(img);
}