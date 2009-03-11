#pragma once

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

typedef struct _features {
  int face_size;
  CvPoint face_position;
  CvPoint nostril_positions[2];
  CvPoint lip_positions[2];
  CvPoint nose_bridge;
  CvPoint pupils[2];
  CvPoint eyebrow_ends[2];

  double horiz_slope;
  double vert_slope;
  double horiz_lengths[2];
  double vert_lengths[2];
  double horiz_rotation;
  double vert_rotation;
  double past_horiz_rotations[5];
  double past_vert_rotations[5];

  double theta;
	int nthetas;
	double past_thetas[5];
	CvPoint* past_centers[5];

	int rot_dir[2];
  double z;
	
	CvPoint centers[2];
	CvSize sizes[2];
} Features;

class Detector {
public:
  Detector();
  ~Detector();
  
  Features ColdStart(IplImage *img);
  void TrackFeatures(IplImage *img, Features& features, double model[9][3]);
	void SetupTracking(IplImage *img, Features& features);
  void GetModel(Features& features, double model[9][3]);
	void FitGlasses(IplImage *img, Features& features, double model[9][3]);
  // move to private later
  void FitModel(Features& features, double model[9][3], Features *model_features = NULL);
	
	double speed[2];

private:
  void FindFace(IplImage *img, Features& features);
  void FindLips(IplImage *face_img, Features& features);
	void FindNostrils(IplImage *face_img, Features &features);
	void FindNoseBridge(IplImage *face_img, Features& features);
	void FindPupils(IplImage *face_img, Features& features);
	void FindEyebrowEnds(IplImage *face_img, Features& features);
	
  void FindFaceCenter(Features& features);
  void FindInitialLengths(Features& features);
  void FindRotation(Features& features);
  
  CvMemStorage* storage;
  CvHaarClassifierCascade* cascade;
	
	IplImage *grey, *prev_grey, *pyramid, *prev_pyramid, *swap_temp;
	
	int win_size;
	int MAX_COUNT;
	CvPoint2D32f* points[2], *swap_points;
	char* status;
	int count;
	int flags;
	
	CvPoint* glasses[3];
};
