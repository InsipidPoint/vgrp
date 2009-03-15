#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>

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
    FindFaceCenter(features);
    FindInitialLengths(features);
	  features.nthetas=0;
  }
  return features;
}

void Detector::TrackFeatures(IplImage *img, Features& features, double model[9][3]) {  
  grey = cvCloneImage(img);
	cvCalcOpticalFlowPyrLK( prev_grey, grey, prev_pyramid, pyramid,
						   points[0], points[1], count, cvSize(win_size,win_size), 3, status, 0,
						   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), flags );
	flags |= CV_LKFLOW_PYR_A_READY;
	int i,k;
	for( i = k = 0; i < count; i++ )
	{
		if( !status[i] )
			continue;
		points[1][k++] = points[1][i];
		
	}
	count = k;
	
	speed[0] = 0;
	speed[1] = 0;
	for(int i=0;i<9;i++) {
		speed[0]+=points[1][i].x - points[0][i].x;
		speed[1]+=points[1][i].y - points[0][i].y;
	}
	speed[0]/=9;
	speed[1]/=9;
//	std::cout<<speed[0]<<" "<<speed[1]<<std::endl;
	
	CV_SWAP( prev_grey, grey, swap_temp );
	CV_SWAP( prev_pyramid, pyramid, swap_temp );
	CV_SWAP( points[0], points[1], swap_points );	
	
	features.lip_positions[0] = cvPointFrom32f(points[0][0]);
	features.lip_positions[1] = cvPointFrom32f(points[0][1]);
	features.nostril_positions[0] = cvPointFrom32f(points[0][2]);
	features.nostril_positions[1] = cvPointFrom32f(points[0][3]);
	features.nose_bridge = cvPointFrom32f(points[0][4]);
	features.pupils[0] = cvPointFrom32f(points[0][5]);
	features.pupils[1] = cvPointFrom32f(points[0][6]);
	features.eyebrow_ends[0] = cvPointFrom32f(points[0][7]);
	features.eyebrow_ends[1] = cvPointFrom32f(points[0][8]);

  FindFaceCenter(features);
  FindRotation(features);
	
//	std::cout<<features.horiz_rotation<<" "<<features.vert_rotation<<std::endl;
//	FitModel(features, model, theta);
//	FitGlasses(img,features,model,theta);
}

void Detector::SetupTracking(IplImage *img, Features& features) {	
	win_size = 10;
	MAX_COUNT = 500;
	flags = 0;
	
	/* allocate all the buffers */
	prev_grey = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	pyramid = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	prev_pyramid = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT*sizeof(points[0][0]));
	status = (char*)cvAlloc(MAX_COUNT);
	prev_grey = cvCloneImage(img);
	
	points[0][0] =  cvPointTo32f(features.lip_positions[0]);
	points[0][1] =  cvPointTo32f(features.lip_positions[1]);
	points[0][2] =  cvPointTo32f(features.nostril_positions[0]);
	points[0][3] =  cvPointTo32f(features.nostril_positions[1]);
	points[0][4] =  cvPointTo32f(features.nose_bridge);
	points[0][5] =  cvPointTo32f(features.pupils[0]);
	points[0][6] =  cvPointTo32f(features.pupils[1]);
	points[0][7] =  cvPointTo32f(features.eyebrow_ends[0]);
	points[0][8] =  cvPointTo32f(features.eyebrow_ends[1]);
	count=9;
	
	CvPoint nb;
	nb.x = (features.nose_bridge.x + (features.eyebrow_ends[0].x + features.eyebrow_ends[1].x)/2 + (features.nostril_positions[0].x + features.nostril_positions[1].x)/2 + (features.lip_positions[0].x + features.lip_positions[1].x)/2)/4;
	nb.y = (features.nose_bridge.y + (features.pupils[0].y + features.pupils[1].y)/2)/2;
	double delta[3] = {0.35,0.18,0.07};
	double w = delta[0]*features.face_size;
	double h = delta[1]*features.face_size;
	double l = delta[2]*features.face_size;
	
	glasses[0] = (CvPoint*)cvAlloc(4*sizeof(glasses[0][0]));
	glasses[1] = (CvPoint*)cvAlloc(4*sizeof(glasses[0][0]));
	glasses[2] = (CvPoint*)cvAlloc(2*sizeof(glasses[0][0]));
	
	glasses[0][0] = cvPoint(-l-w,(features.eyebrow_ends[0].y-nb.y)/2);
	glasses[0][1] = cvPoint(-l,(features.eyebrow_ends[0].y-nb.y)/2);
	glasses[0][2] = cvPoint(-l,(features.eyebrow_ends[0].y-nb.y)/2+h);
	glasses[0][3] = cvPoint(-l-w,(features.eyebrow_ends[0].y-nb.y)/2+h);
	glasses[1][0] = cvPoint(l,(features.eyebrow_ends[1].y-nb.y)/2);
	glasses[1][1] = cvPoint(l+w,(features.eyebrow_ends[1].y-nb.y)/2);
	glasses[1][2] = cvPoint(l+w,(features.eyebrow_ends[1].y-nb.y)/2+h);
	glasses[1][3] = cvPoint(l,(features.eyebrow_ends[1].y-nb.y)/2+h);
	glasses[2][0] = cvPoint(-l,(features.eyebrow_ends[0].y-nb.y)/2+h/2);
	glasses[2][1] = cvPoint(-l,(features.eyebrow_ends[1].y-nb.y)/2+h/2);
	
	features.nthetas=0;
	features.past_centers[0] = (CvPoint*)cvAlloc(5*sizeof(features.past_centers[0][0]));
	features.past_centers[1] = (CvPoint*)cvAlloc(5*sizeof(features.past_centers[0][0]));

	features.rot_dir[0] = 0;
	features.rot_dir[1] = 0;
}

void rot(double theta, CvPoint& pt) {
	double p1 = pt.x, p2 = pt.y;
	pt.x = cos(theta)*p1 + sin(theta)*p2;
	pt.y = -sin(theta)*p1 + cos(theta)*p2;
}

void Detector::FitGlasses(IplImage *img, Features& features, double model[9][3]) {
	//LR = +-theta[0]
	//UD = +-theta[1]
	//L/R rotate = +-theta[2]
	IplImage *out = cvCloneImage(img);
	CvPoint face_center = features.face_position;
//	face_center.x = (features.nostril_positions[0].x + features.nostril_positions[1].x)/2;
//	face_center.y = (features.nostril_positions[0].y + features.nostril_positions[1].y)/2;
	double delta[3] = {0.35,0.18,0.07};
	double w = delta[0]*features.face_size;
	double h = delta[1]*features.face_size;
	double l = delta[2]*features.face_size;
//	std::cout<<features.horiz_slope<<std::endl;
	double scale1[2] = {1,1};
	double scale2[2] = {1,1};
	//theta[0] = 1;
//	theta[0] = (cvSqrt((features.nostril_positions[0].x-features.nostril_positions[1].x)*(features.nostril_positions[0].x-features.nostril_positions[1].x) + (features.nostril_positions[0].y-features.nostril_positions[1].y)*(features.nostril_positions[0].y-features.nostril_positions[1].y)))/features.face_size;
//	std::cout<<theta[0]<<std::endl;
//	CvMat *perspective_projection = cvCreateMat(3,3,CV_32FC1);
	CvPoint nb = features.nose_bridge;
//	nb.x = (features.nose_bridge.x + (features.eyebrow_ends[0].x + features.eyebrow_ends[1].x)/2 + (features.nostril_positions[0].x + features.nostril_positions[1].x)/2 + (features.lip_positions[0].x + features.lip_positions[1].x)/2)/4;
//	nb.y = (features.nose_bridge.y + (features.pupils[0].y + features.pupils[1].y)/2)/2;
	
//	CvMat *src = cvCreateMat(2,4,CV_32FC1);
//	CvMat *dst = cvCreateMat(2,4,CV_32FC1);
//	cvmSet(src,0,0,nb.x-l-w);
//	cvmSet(src,1,0,(features.eyebrow_ends[0].y+nb.y)/2);
//	cvmSet(src,0,1,nb.x-l-w);
//	cvmSet(src,1,1,(features.eyebrow_ends[0].y+nb.y)/2+h);
//	cvmSet(src,0,2,nb.x+l+w);
//	cvmSet(src,1,2,(features.eyebrow_ends[1].y+nb.y)/2);
//	cvmSet(src,0,3,nb.x+l+w);
//	cvmSet(src,1,3,(features.eyebrow_ends[1].y+nb.y)/2+h);
//	
//	cvmSet(dst,0,0,nb.x-l-scale1[1]*w);
//	cvmSet(dst,1,0,scale1[0]*(features.eyebrow_ends[0].y+nb.y)/2);
//	cvmSet(dst,0,1,nb.x-l-scale1[1]*w);
//	cvmSet(dst,1,1,scale1[1]*(features.eyebrow_ends[0].y+nb.y)/2+h);
//	cvmSet(dst,0,2,nb.x+l+scale1[0]*w);
//	cvmSet(dst,1,2,scale1[1]*(features.eyebrow_ends[1].y+nb.y)/2);
//	cvmSet(dst,0,3,nb.x+l+scale1[0]*w);
//	cvmSet(dst,1,3,scale1[0]*(features.eyebrow_ends[1].y+nb.y)/2+h);
//	cvFindHomography(src,dst,perspective_projection);
	
//	nb.x -= face_center.x;
//	nb.y -= face_center.y;
//	
//	features.eyebrow_ends[0].x -= face_center.x;
//	features.eyebrow_ends[0].y -= face_center.y;
//	features.eyebrow_ends[1].x -= face_center.x;
//	features.eyebrow_ends[1].y -= face_center.y;
//	
//	nb.x *= cos(theta[2]);
//	nb.y *= sin(theta[2]);
//	features.eyebrow_ends[0].x *= cos(theta[2]);
//	features.eyebrow_ends[0].y *= sin(theta[2]);
//	features.eyebrow_ends[1].x *= cos(theta[2]);
//	features.eyebrow_ends[1].y *= sin(theta[2]);
//	
	double deltas[6] = {10,10,10,10,0,0};
	double hor = std::max(0.0,features.horiz_rotation-0.2);
	double vert = std::max(0.0,features.vert_rotation-0.2);
	if(features.rot_dir[0]<0) {
		deltas[0]=10*(1-hor/0.6); //30 -> 20
		deltas[2]=10+5*(hor/0.6); //30 -> 35
		deltas[4]+=l*(hor/0.6);
		std::cout<<"deltas[4]: "<<deltas[4]<<std::endl;
	}
	else if(features.rot_dir[0]>0) {
		deltas[0]=10+5*(hor/0.6); //30 -> 35
		deltas[2]=10*(1-hor/0.6); //30 -> 20		
		deltas[5]-=l*0.5*(hor/0.6);
		std::cout<<"deltas[5]: "<<deltas[5]<<std::endl;
	}
	if(features.rot_dir[1]!=0) {
		deltas[1] = 10*(1-vert/0.6); //30 -> 20
		deltas[3] = 10*(1-vert/0.6); //30 -> 20
	}

	
	CvPoint* points[2] = {0,0};
	points[0] = (CvPoint*)cvAlloc(4*sizeof(points[0][0]));
	points[1] = (CvPoint*)cvAlloc(4*sizeof(points[0][0]));

	CvPoint* new_glasses[3] = {0,0,0};
	new_glasses[0] = (CvPoint*)cvAlloc(4*sizeof(new_glasses[0][0]));
	new_glasses[1] = (CvPoint*)cvAlloc(4*sizeof(new_glasses[0][0]));
	new_glasses[2] = (CvPoint*)cvAlloc(2*sizeof(new_glasses[0][0]));

	for(int i=0;i<2;i++) {
		for(int j=0;j<4;j++) {
			if(i==0) new_glasses[i][j].x = glasses[i][j].x + deltas[4];
			else if(i==1) new_glasses[i][j].x = glasses[i][j].x + deltas[5];
			new_glasses[i][j].y = glasses[i][j].y;
//			new_glasses[i][j] = glasses[i][j];
			rot(features.theta,new_glasses[i][j]);
		}
	}
	
	double theta = 0;
	if(features.nthetas == 5) {
	for(int i=1;i<5;i++) {
		features.past_thetas[i-1] = features.past_thetas[i];
		theta += features.past_thetas[i];
	}
	features.past_thetas[4] = features.theta;
	theta+=features.past_thetas[4];
	theta/=5;
	}
	else {
		features.past_thetas[features.nthetas] = features.theta;
		theta = features.theta;
//		features.nthetas++;
	}
	
	new_glasses[2][0] = glasses[2][0];
	rot(theta,new_glasses[2][0]);
	new_glasses[2][1] = glasses[2][1];
	rot(theta,new_glasses[2][1]);
	points[0][0] = cvPoint(nb.x+new_glasses[0][0].x,nb.y+new_glasses[0][0].y);
	points[0][1] = cvPoint(nb.x+new_glasses[0][1].x,nb.y+new_glasses[0][1].y);
	points[0][2] = cvPoint(nb.x+new_glasses[0][2].x,nb.y+new_glasses[0][2].y);
	points[0][3] = cvPoint(nb.x+new_glasses[0][3].x,nb.y+new_glasses[0][3].y);
	points[1][0] = cvPoint(nb.x+new_glasses[1][0].x,nb.y+new_glasses[1][0].y);
	points[1][1] = cvPoint(nb.x+new_glasses[1][1].x,nb.y+new_glasses[1][1].y);
	points[1][2] = cvPoint(nb.x+new_glasses[1][2].x,nb.y+new_glasses[1][2].y);
	points[1][3] = cvPoint(nb.x+new_glasses[1][3].x,nb.y+new_glasses[1][3].y);
//	points[0][1] = cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2);
//	points[0][2] = cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2+h);
//	points[0][3] = cvPoint(nb.x-l-w,(features.eyebrow_ends[0].y+nb.y)/2+h);
//	points[1][0] = cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2);
//	points[1][1] = cvPoint(nb.x+l+w,(features.eyebrow_ends[1].y+nb.y)/2);
//	points[1][2] = cvPoint(nb.x+l+w,(features.eyebrow_ends[1].y+nb.y)/2+h);
//	points[1][3] = cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2+h);
	
	
	//	printf("%f\n",theta[0]);
//	cvRectangle(out, cvPoint(nb.x-scale1[1]*theta[0]*l-w,scale1[0]*theta[0]*(features.eyebrow_ends[0].y+nb.y)/2), cvPoint(nb.x-scale1[1]*theta[0]*l,(features.eyebrow_ends[0].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
//	cvRectangle(out, cvPoint(nb.x+scale1[0]*theta[0]*l,(features.eyebrow_ends[1].y+nb.y)/2), cvPoint(nb.x+scale1[0]*theta[0]*l+scale1[0]*theta[0]*w,scale1[1]*theta[0]*(features.eyebrow_ends[1].y+nb.y)/2+scale1[0]*theta[0]*h), cvScalar(0,0,255), 1, 8, 0);
//	cvLine(out,cvPoint(nb.x-scale1[1]*theta[0]*l,(features.eyebrow_ends[0].y+nb.y)/2+h/2),cvPoint(nb.x+scale1[0]*theta[0]*l,(features.eyebrow_ends[1].y+nb.y)/2+h/2), cvScalar(0,0,255), 1, 8, 0);
	
//	cvRectangle(out, cvPoint(nb.x-l-w,(features.eyebrow_ends[0].y+nb.y)/2), cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
//	cvRectangle(out, cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2), cvPoint(nb.x+l+w,(features.eyebrow_ends[1].y+nb.y)/2+h), cvScalar(0,0,255), 1, 8, 0);
//	cvLine(out,cvPoint(nb.x-l,(features.eyebrow_ends[0].y+nb.y)/2+h/2),cvPoint(nb.x+l,(features.eyebrow_ends[1].y+nb.y)/2+h/2), cvScalar(0,0,255), 1, 8, 0);

	int npts[1] = {4};
//	cvPolyLine(out,&points[0], npts, 1, 1,cvScalar(0,0,0));
//	cvPolyLine(out,&points[1], npts, 1, 1,cvScalar(0,0,0));
//	cvLine(out,cvPoint(nb.x+new_glasses[2][0].x,nb.y+new_glasses[2][0].y),cvPoint(nb.x+new_glasses[2][1].x,nb.y+new_glasses[2][1].y), cvScalar(0,0,255), 1, 8, 0);
	
//	cvCircle(out,cvPoint((nb.x+features.eyebrow_ends[0].x)/2,(nb.y+features.eyebrow_ends[0].y)/2),30,cvScalar(0,0,0),1,8,0);
//	cvCircle(out,cvPoint((nb.x+features.eyebrow_ends[1].x)/2,(nb.y+features.eyebrow_ends[1].y)/2),30,cvScalar(0,0,0),1,8,0);

	CvPoint center1;
	center1.x = (new_glasses[0][0].x + new_glasses[0][1].x + new_glasses[0][2].x + new_glasses[0][3].x)/4;
	center1.y = (new_glasses[0][0].y + new_glasses[0][1].y + new_glasses[0][2].y + new_glasses[0][3].y)/4;

	CvPoint center2;
	center2.x = (new_glasses[1][0].x + new_glasses[1][1].x + new_glasses[1][2].x + new_glasses[1][3].x)/4;
	center2.y = (new_glasses[1][0].y + new_glasses[1][1].y + new_glasses[1][2].y + new_glasses[1][3].y)/4;
	
	CvPoint newcenter1,newcenter2;
	newcenter1.x = 0;
	newcenter1.y = 0;
	newcenter2.x = 0;
	newcenter2.y = 0;
	if(features.nthetas == 5) {
	for(int i=1;i<5;i++) {
		features.past_centers[0][i-1].x = features.past_centers[0][i].x;
		features.past_centers[0][i-1].y = features.past_centers[0][i].y;
		newcenter1.x+=features.past_centers[0][i].x;
		newcenter1.y+=features.past_centers[0][i].y;

		features.past_centers[1][i-1].x = features.past_centers[1][i].x;
		features.past_centers[1][i-1].y = features.past_centers[1][i].y;
		newcenter2.x+=features.past_centers[1][i].x;
		newcenter2.y+=features.past_centers[1][i].y;
		
	}
		features.past_centers[0][4].x = center1.x;
		features.past_centers[0][4].y = center1.y;
		newcenter1.x+=features.past_centers[0][4].x;
		newcenter1.y+=features.past_centers[0][4].y;

		
		features.past_centers[1][4].x = center2.x;
		features.past_centers[1][4].y = center2.y;
		newcenter2.x+=features.past_centers[1][4].x;
		newcenter2.y+=features.past_centers[1][4].y;
		
		newcenter1.x/=5;
		newcenter1.y/=5;
		newcenter2.x/=5;
		newcenter2.y/=5;
	}
	else {
		newcenter1 = center1;
		newcenter2 = center2;
		features.past_centers[0][features.nthetas] = center1;
		features.past_centers[1][features.nthetas] = center2;
		features.nthetas++;
	}
//	std::cout<<features.nthetas<<std::endl;
//	cvCircle(out,cvPoint(nb.x+center1.x,nb.y+center1.y),30,cvScalar(0,0,0),4,8,0);
//	cvCircle(out,cvPoint(nb.x+center2.x,nb.y+center2.y),30,cvScalar(0,0,0),4,8,0);
//	std::cout<<features.rot_dir[0]<<" "<<features.rot_dir[1]<<std::endl;
	std::cout<<features.horiz_rotation<<" "<<features.vert_rotation<<std::endl;
	features.centers[0] = cvPoint(nb.x+newcenter1.x,nb.y+newcenter1.y);
	features.centers[1] = cvPoint(nb.x+newcenter2.x,nb.y+newcenter2.y);
	features.sizes[0] = cvSize((20+deltas[0]),(20+deltas[1]));
	features.sizes[1] = cvSize((20+deltas[2]),(20+deltas[3]));
	
//	cvEllipse(out,cvPoint(nb.x+newcenter1.x,nb.y+newcenter1.y),cvSize((20+deltas[0]),(20+deltas[1])),theta*180/3.14,0,360,cvScalar(0,0,0),4,8,0);
//	cvEllipse(out,cvPoint(nb.x+newcenter2.x,nb.y+newcenter2.y),cvSize((20+deltas[2]),(20+deltas[3])),theta*180/3.14,0,360,cvScalar(0,0,0),4,8,0);
//	
//	cvShowImage("result",out);
	return;
}
