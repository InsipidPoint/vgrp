#include "Detector.h"
#include <cstdio>

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