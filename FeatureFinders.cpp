#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>

void Detector::FindFace(IplImage *img, Features& features) {
  cvClearMemStorage(storage);
  CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                      1.1, 2, CV_HAAR_DO_CANNY_PRUNING ,cvSize(80, 80) );
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

void findLeftBottom(IplImage *image, int nlabels, int& posx, int& posy) {
	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> valid;
	for(int i=0;i<nlabels;i++) {
		x.push_back(image->width);
		y.push_back(-1);
		valid.push_back(1);
	}
	for(int i=0;i<image->height;i++) {
		for(int j=0;j<image->width;j++) {
			int temp = CV_IMAGE_ELEM( image, uchar, i, j);
			
			if(temp>0) {
				temp = temp-1;
				if(x[temp] > j) {
					x[temp] = j;
					y[temp] = i;
				}
				else if(x[temp] == j && y[temp] < i) {
					y[temp] = i;
				}
			}
		}
	}
	
	double xmean=0,ymean=0;
	int num = 0;
	for(int i=0;i<valid.size();i++) {
		if(valid[i]>0) {
			xmean+=x[i];
			ymean+=y[i];
			num++;
		}
	}
	posx = (int)(xmean/num);
	posy = (int)(ymean/num);
//	std::cout<<"Left Bottom"<<std::endl;
}

void findRightBottom(IplImage *image, int nlabels, int& posx, int& posy) {
	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> valid;
	for(int i=0;i<nlabels;i++) {
		x.push_back(0);
		y.push_back(-1);
		valid.push_back(1);
	}
	
	for(int i=0;i<image->height;i++) {
		for(int j=0;j<image->width;j++) {
			int temp = CV_IMAGE_ELEM( image, uchar, i, j);
			if(temp>0) {
				temp = temp-1;
				
				if(x[temp] < j) {
					x[temp] = j;
					y[temp] = i;
				}
				else if(x[temp] == j && y[temp] < i) {
					y[temp] = i;
				}
			}
		}
	}
	double xmean=0,ymean=0;
	int num = 0;
	for(int i=0;i<valid.size();i++) {
		if(valid[i]>0) {
			xmean+=x[i];
			ymean+=y[i];
			num++;
		}
	}
	posx = (int)(xmean/num);
	posy = (int)(ymean/num);
//	std::cout<<"Right Bottom"<<std::endl;

}

double findMax(IplImage *image, int& x, int& y, int dir) {
	double max = -1*dir*123456789;
	x = -1;
	y = -1;
	for(int i=0;i<image->height;i++) {
		for(int j=0;j<image->width;j++) {
			double temp = CV_IMAGE_ELEM( image, uchar, i, j);
			if(dir*temp>dir*max) {
				max = temp;
				x = j;
				y = i;
			}
		}
		
	}
	return max;
}

double min(double x, double y) {
	if(x<y) return x;
	return y;
}

double max(double x, double y) {
	if(x>y) return x;
	return y;
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

void Detector::FindEyebrowEnds(IplImage *img, Features& features) {
	int tlx = cvRound(features.face_position.x);
	int tly = cvRound(features.face_position.y);
	int width = cvRound(features.face_size), height = cvRound(features.face_size);
	
	CvRect roi = cvRect(tlx, tly, width, height);
	cvSetImageROI(img, roi);
	IplImage *image = cvCreateImage( cvSize(width,height), 8, 1 );
	cvCopy(img,image,0);
	int x = features.nose_bridge.x - tlx;
	int y = features.nose_bridge.y - tly;
	int x1 = features.pupils[0].x - tlx;	
	int y1 = features.pupils[0].y - tly;	
	int x2 = features.pupils[1].x - tlx;	
	int y2 = features.pupils[1].y - tly;	
	int a1,a2,b1,b2;
	IplImage *blurredimage1 = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	cvSmooth(image,blurredimage1,CV_GAUSSIAN,41,41,10);
	IplImage *blurredimage2 = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	cvSmooth(image,blurredimage2,CV_GAUSSIAN,41,41,5);
	IplImage *logimagetemp = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	IplImage *logimage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	cvAbsDiff(blurredimage1,blurredimage2,logimagetemp);
	int posx,posy;
	double maximum = findMax(logimagetemp,posx,posy,1);
	cvConvertScaleAbs(logimagetemp,logimage,255.0/maximum,0);
	IplImage *cannyimage = cvCreateImage( cvSize(image->width,image->height), 8, 1 );
	cvCanny(logimage,cannyimage,400,1400,5);
	int ymin = min(y1,y2);
	int xmin = min(x1,x2);
	int xmax = max(x1,x2);
	int leftx = max(0,xmin - image->width/5);
	int lefty = max(0,image->height/4);
	int lefth =  max(ymin - image->height/20 - image->height/4,image->height/20);
	int leftw = min((xmax + image->width/5 - leftx)/8,image->width-leftx);
	if(leftw==0) {
		if(leftx>0) {
			leftx--;
			leftw=1;
		}
		else {
			leftx = image->width - 1;
			leftw = 1;
		}
	}
	if(lefth==0) {
		if(lefty>0) {
			lefty--;
			lefth=1;
		}
		else {
			lefty = image->height - 1;
			lefth = 1;
		}
	}
	
	CvRect rl = cvRect( leftx,lefty,leftw,lefth );
	IplImage *croppedcannyimagel = cvCreateImage(cvSize(rl.width,rl.height), 8, 1 );
	cvSetImageROI(cannyimage,rl);
	cvCopy(cannyimage,croppedcannyimagel,0);
	cvResetImageROI(cannyimage);
	int rightx = max(0,leftx + 7*(xmax + image->width/5 - leftx)/8);
	int righty = max(0,image->height/4);
	int righth =  max(ymin - image->height/20 - image->height/4,image->height/20);
	int rightw = min((xmax + image->width/5 - rightx)/8,image->width-rightx);
	if(rightw==0) {
		if(rightx>0) {
			rightx--;
			rightw=1;
		}
		else {
			rightx = image->width - 1;
			rightw = 1;
		}
	}
	if(righth==0) {
		if(righty>0) {
			righty--;
			righth=1;
		}
		else {
			righty = image->height - 1;
			righth = 1;
		}
	}
	CvRect rr = cvRect(rightx,righty,rightw,righth);
	IplImage *croppedcannyimager = cvCreateImage(cvSize(rr.width,rr.height), 8, 1 );
	cvSetImageROI(cannyimage,rr);
	cvCopy(cannyimage,croppedcannyimager,0);
	cvResetImageROI(cannyimage);
	CvMemStorage* storagel = cvCreateMemStorage(0);
	CvMemStorage* storager = cvCreateMemStorage(0);
	CvSeq* contourl = 0;
	CvSeq* contourr = 0;
	IplImage *labelimagel = cvCreateImage(cvSize(croppedcannyimagel->width,croppedcannyimagel->height), 8, 1 );
	IplImage *labelimager = cvCreateImage(cvSize(croppedcannyimager->width,croppedcannyimager->height), 8, 1 );
	cvFindContours( croppedcannyimagel, storagel, &contourl);
	cvZero( labelimagel );
	int counter = 1;
	for( ; contourl != 0; contourl = contourl->h_next ) {
		CvScalar color = CV_RGB(counter,counter,counter );
		cvDrawContours( labelimagel, contourl, color, color, -1, 1, 8 );
		counter++;
	}
	findLeftBottom(labelimagel,--counter,a1,b1);
	
	cvFindContours( croppedcannyimager, storager, &contourr);
	cvZero( labelimager );
	counter = 1;
	for( ; contourr != 0; contourr = contourr->h_next ) {
		CvScalar color = CV_RGB(counter,counter,counter );
		cvDrawContours( labelimager, contourr, color, color, -1, 1, 8 );
		counter++;
	}
	findRightBottom(labelimager,--counter,a2,b2);
	a1 = a1 + leftx;
	b1 = b1 + lefty;
	a2 = a2 + rightx;
	b2 = b2 + righty;
	features.eyebrow_ends[0].x = a1+tlx;
	features.eyebrow_ends[0].y = b1+tly;
	features.eyebrow_ends[1].x = a2+tlx;
	features.eyebrow_ends[1].y = b2+tly;

	cvResetImageROI(img);
	
	return;
	
}