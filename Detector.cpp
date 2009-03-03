#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>

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
	  FindNostrils(img,features);
	  FindNoseBridge(img,features);
	  FindPupils(img,features);
	  FindEyebrowEnds(img,features);
  }
  return features;
}

void Detector::TrackFeatures(IplImage *img, Features& features) {
  
}
