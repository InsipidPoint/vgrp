#include "Detector.h"
#include <cstdio>

void Detector::TrackLips(IplImage *img, Features& features) {
  
  
  // real code
  return;
  static bool first = true;
  static double feature[15][15];
  if(first) {
    for(int x = 0; x < 15; x++) {
      for(int y = 0; y < 15; y++) {
        feature[x][y] = cvGet2D(img,y+features.lip_positions[0].y-7,x+features.lip_positions[0].x-7).val[0];
      }
    }
    first = false;
  }
  
  CvPoint min_point;
  double min_norm = 999999;
  for(int img_x = features.lip_positions[0].x-30; img_x <= features.lip_positions[0].x+30; img_x++) {
    for(int img_y = features.lip_positions[0].y-30; img_y <= features.lip_positions[0].y+30; img_y++) {
      double current_val = 0;
      for(int x = 0; x < 15; x++) {
        for(int y = 0; y < 15; y++) {
          current_val += pow(feature[x][y] - cvGet2D(img, img_y+y-7, img_x+x-7).val[0], 2);
        }
      }
      if(current_val < min_norm) {
        min_norm = current_val;
        min_point.x = img_x;
        min_point.y = img_y;
      }
    }
  }
  
  features.lip_positions[0] = min_point;
  
  for(int x = 0; x < 15; x++) {
    for(int y = 0; y < 15; y++) {
      feature[x][y] = cvGet2D(img,y+features.lip_positions[0].y-7,x+features.lip_positions[0].x-7).val[0];
    }
  }
}