#include "Detector.h"
#include <cstdio>
#include <iostream>
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
	  FindNostrils(img,features);
	  FindNoseBridge(img,features);
	  FindPupils(img,features);
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

double findMax(IplImage *image, int& x, int& y, int dir) {
	double max = -1*dir*123456789;
	x = -1;
	y = -1;
	for(int i=0;i<image->height;i++) {
		for(int j=0;j<image->width;j++) {
			double temp = cvGet2D( image, i, j).val[0];
			if(dir*temp>dir*max) {
				max = temp;
				x = j;
				y = i;
			}
		}
		
	}
	return max;
}

void Detector::FindNostrils(IplImage *img, Features& features) {
	
	int tlx = cvRound(features.face_position.x+features.face_size/3.0);
	int tly = cvRound(features.face_position.y+features.face_size/2.0);
	int width = cvRound(features.face_size/3.0);
	int halfWidth = width/2.0;
	int height = cvRound(features.face_size/5.0);
	CvRect roiLeft = cvRect(tlx, tly, halfWidth, height);
	CvRect roiRight = cvRect(tlx + halfWidth, tly, halfWidth, height);
	
	cvSetImageROI(img, roiLeft);
	double minValue = 9999;
	int minX = 0;
	int minY = 0;
	
	// locate min intensity
	for(int x = 0; x < halfWidth; x++) {
		for(int y = 0; y < height; y++) {
			if(cvGet2D(img,y,x).val[0] < minValue) {
				minValue = cvGet2D(img,y,x).val[0];
				minX = x;
				minY = y;
			}
		}
	}
	features.nostril_positions[0].x = tlx + minX;
	features.nostril_positions[0].y = tly + minY;
	
	
	cvSetImageROI(img, roiRight);
	minValue = 9999;
	minX = 0;
	minY = 0;
	
	// locate min intensity
	for(int x = 0; x < halfWidth; x++) {
		for(int y = 0; y < height; y++) {
			if(cvGet2D(img,y,x).val[0] < minValue) {
				minValue = cvGet2D(img,y,x).val[0];
				minX = x;
				minY = y;
			}
		}
	}
	features.nostril_positions[1].x = tlx + halfWidth + minX;
	features.nostril_positions[1].y = tly + minY;
	
	cvResetImageROI(img);
}


void Detector::FindNoseBridge(IplImage *img, Features& features) {
	int tlx = cvRound(features.face_position.x);
	int tly = cvRound(features.face_position.y);
	int width = cvRound(features.face_size), height = cvRound(features.face_size);

	CvRect roi = cvRect(tlx, tly, width, height);
	cvSetImageROI(img, roi);
	IplImage *image = cvCreateImage( cvSize(width,height), 8, 1 );
	cvCopy(img,image,0);
	int x,y;
	IplImage *blurredimage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	cvSmooth(image,blurredimage,CV_GAUSSIAN,9,9,5);
	CvRect r = cvRect(blurredimage->width/6,blurredimage->height/4,blurredimage->width*4/6,blurredimage->height/4);
	IplImage *croppedblurredimage = cvCreateImage(cvSize(r.width,r.height), 8, 1 );
	cvSetImageROI(blurredimage,r);
	cvCopy(blurredimage,croppedblurredimage,0);
	IplImage *blurredimage1 = cvCreateImage( cvSize(croppedblurredimage->width,croppedblurredimage->height), 8, 1 );
	cvSmooth(croppedblurredimage,blurredimage1,CV_BLUR,9,9,0);
	CvRect r1 = cvRect(blurredimage1->width/3,blurredimage1->height/3,blurredimage1->width/3,blurredimage1->height/3);
	IplImage *croppedblurredimage1 = cvCreateImage(cvSize(r1.width,r1.height), 8, 1 );
	cvSetImageROI(blurredimage1,r1);
	cvCopy(blurredimage1,croppedblurredimage1,0);
	findMax(croppedblurredimage1,x,y,1);
	features.nose_bridge.x = x + blurredimage1->width/3 + blurredimage->width/6 + tlx;
	features.nose_bridge.y = y + blurredimage1->height/3 + blurredimage->height/4 + tly;
	cvResetImageROI(img);
	return;	
}

void Detector::FindPupils(IplImage *img, Features& features) {
	int tlx = cvRound(features.face_position.x);
	int tly = cvRound(features.face_position.y);
	int width = cvRound(features.face_size), height = cvRound(features.face_size);
	
	CvRect roi = cvRect(tlx, tly, width, height);
	cvSetImageROI(img, roi);
	IplImage *image = cvCreateImage( cvSize(width,height), 8, 1 );
	cvCopy(img,image,0);
	int nbx = features.nose_bridge.x - tlx;
	int nby = features.nose_bridge.y - tly;
	int x1,y1,x2,y2;	
	CvRect r = cvRect(image->width/8,nby - image->height/20,image->width*6/8,image->height/10);
	IplImage *croppedimage = cvCreateImage(cvSize(r.width,r.height), 8, 1 );
	cvSetImageROI(image,r);
	cvCopy(image,croppedimage,0);
	IplImage *croppedblurredimage = cvCreateImage( cvSize(croppedimage->width,croppedimage->height), 8, 1 );
	cvSmooth(croppedimage,croppedblurredimage,CV_BLUR,9,9,0);
	
	CvRect rl = cvRect(0,0,croppedblurredimage->width/2,croppedblurredimage->height);
	IplImage *croppedimagel = cvCreateImage(cvSize(rl.width,rl.height), 8, 1 );
	cvSetImageROI(croppedblurredimage,rl);
	cvCopy(croppedblurredimage,croppedimagel,0);
	CvRect rr = cvRect(croppedblurredimage->width/2,0,croppedblurredimage->width/2,croppedblurredimage->height);
	IplImage *croppedimager = cvCreateImage(cvSize(rr.width,rr.height), 8, 1 );
	cvResetImageROI(croppedblurredimage);
	cvSetImageROI(croppedblurredimage,rr);
	cvCopy(croppedblurredimage,croppedimager,0);
	double min = findMax(croppedimagel,x1,y1,-1);
	double radius1 = 5.0;
	x1 = x1 + image->width/8;
	y1 = y1 + nby - image->height/20;
	findMax(croppedimager,x2,y2,-1);
	x2 = x2 + image->width/8 + croppedblurredimage->width/2;
	y2 = y2 + nby - image->height/20;
	
	features.pupils[0].x = x1+tlx;
	features.pupils[0].y = y1+tly;
	features.pupils[1].x = x2+tlx;
	features.pupils[1].y = y2+tly;
	cvResetImageROI(img);
	
	return;
}