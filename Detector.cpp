#include "Detector.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include "math.h"

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
  }
  return features;
}

void Detector::TrackFeatures(IplImage *img, Features& features, double model[9][3], double theta[3]) {
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
}

void Detector::FindFaceCenter(Features& features) {
  CvPoint centers[4];
  double slopes[3];
  double angles[3];

  centers[0].x = (features.nostril_positions[0].x+features.nostril_positions[1].x)/2.0;
  centers[0].y = (features.nostril_positions[0].y+features.nostril_positions[1].y)/2.0;

  centers[1].x = (features.lip_positions[0].x+features.lip_positions[1].x)/2.0;
  centers[1].y = (features.lip_positions[0].y+features.lip_positions[1].y)/2.0;

  centers[2].x = (features.pupils[0].x+features.pupils[1].x)/2.0;
  centers[2].y = (features.pupils[0].y+features.pupils[1].y)/2.0;

  centers[3].x = features.nose_bridge.x;
  centers[3].y = features.nose_bridge.y;

  slopes[0] = double(features.nostril_positions[0].y-features.nostril_positions[1].y)/double(features.nostril_positions[1].x-features.nostril_positions[0].x);

  slopes[1] = double(features.lip_positions[0].y-features.lip_positions[1].y)/double(features.lip_positions[1].x-features.lip_positions[0].x);

  slopes[2] = double(features.pupils[0].y-features.pupils[1].y)/double(features.pupils[1].x-features.pupils[0].x);

  angles[0] = atan(slopes[0]);
  angles[1] = atan(slopes[1]);
  angles[2] = atan(slopes[2]);
  
  int counts[3];
  int maxCount = 0, maxIndex = 0;
  for (int i = 0; i < 3; i++) {
    counts[i] = 0;
    for (int j = 0; j < 3; j++) {
      if (i != j && fabs(angles[i]-angles[j]) < 0.1) {
        counts[i]++;
      }
    }
    if (counts[i] > maxCount) {
      maxIndex = i;
      maxCount = counts[i];
    }
  }

  double horiz_ang = angles[maxIndex];
  std::cout << "horz ang: " << horiz_ang << std::endl;
  std::cout << "nostrils: " << angles[0] << std::endl;
  std::cout << "mouth: " << angles[1] << std::endl;
  std::cout << "eyes: " << angles[2] << std::endl;


  double slopes2[5];
  double angles2[5];

  double denom = double(centers[1].x-centers[0].x);
  if (denom != 0) {
    slopes2[0] = double(centers[0].y-centers[1].y)/denom;
  } else {
    slopes2[0] = 11;
  }
  slopes2[0] = denom/double(centers[0].y-centers[1].y);

  denom = double(centers[2].x-centers[0].x);
  if (denom != 0) {
    slopes2[1] = double(centers[0].y-centers[2].y)/denom;
  } else {
    slopes2[1] = 11;
  }
  slopes2[1] = denom/double(centers[0].y-centers[2].y);

  denom = double(centers[3].x-centers[0].x);
  if (denom != 0) {
    slopes2[2] = double(centers[0].y-centers[3].y)/denom;
  } else {
    slopes2[2] = 11;
  }
  slopes2[2] = denom/double(centers[0].y-centers[3].y);

  denom = double(centers[2].x-centers[1].x);
  if (denom != 0) {
    slopes2[3] = double(centers[1].y-centers[2].y)/denom;
  } else {
    slopes2[3] = 11;
  }
  slopes2[3] = denom/double(centers[1].y-centers[2].y);

  denom = double(centers[3].x-centers[1].x);
  if (denom != 0) {
    slopes2[4] = double(centers[1].y-centers[3].y)/denom;
  } else {
    slopes2[4] = 11;
  }
  slopes2[4] = denom/double(centers[1].y-centers[3].y);


  angles2[0] = atan(slopes2[0]);
  angles2[1] = atan(slopes2[1]);
  angles2[2] = atan(slopes2[2]);
  angles2[3] = atan(slopes2[3]);
  angles2[4] = atan(slopes2[4]);

  int counts2[5];
  maxCount = 0, maxIndex = 0;
  for (int i = 0; i < 5; i++) {
    counts2[i] = 0;
    for (int j = 0; j < 5; j++) {
      if (i != j && fabs(angles2[i]-angles2[j]) < 0.1) {
        counts2[i]++;
      }
    }
    if (counts2[i] > maxCount) {
      maxIndex = i;
      maxCount = counts2[i];
    }
  }

  double vert_ang = angles2[maxIndex];
  std::cout << "vert ang: " << vert_ang << std::endl;
  std::cout << "1: " << angles2[0] << std::endl;
  std::cout << "2: " << angles2[1] << std::endl;
  std::cout << "3: " << angles2[2] << std::endl;
  std::cout << "4: " << angles2[3] << std::endl;
  std::cout << "5: " << angles2[4] << std::endl;
  std::cout << "#####" << std::endl;

  
//  votes[0][0] = features.nostril_positions[0].x - center_x;
//  votes[0][1] = features.nostril_positions[0].y - center_y;
  
//  votes[1][0] = features.nostril_positions[1].x - center_x;
// votes[1][1] = features.nostril_positions[1].y - center_y;
  
//  votes[2][0] = features.lip_positions[0].x - center_x;
//  votes[2][1] = features.lip_positions[0].y - center_y;
  
//  votes[3][0] = features.lip_positions[1].x - center_x;
//  votes[3][1] = features.lip_positions[1].y - center_y;
  
//  votes[4][0] = features.nose_bridge.x - center_x;
//  votes[4][1] = features.nose_bridge.y - center_y;
  
//  votes[5][0] = features.pupils[0].x - center_x;
//  votes[5][1] = features.pupils[0].y - center_y;
  
//  votes[6][0] = features.pupils[1].x - center_x;
//  votes[6][1] = features.pupils[1].y - center_y;
  
//  votes[7][0] = features.eyebrow_ends[0].x - center_x;
//  votes[7][1] = features.eyebrow_ends[0].y - center_y;
  
//  votes[8][0] = features.eyebrow_ends[1].x - center_x;
//  votes[8][1] = features.eyebrow_ends[1].y - center_y;

}


