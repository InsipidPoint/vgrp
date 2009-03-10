#include "Detector.h"
#include <cstdio>

void rot_x(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = p0;
  pt[1] = cos(theta)*p1 + sin(theta)*p2;
  pt[2] = -sin(theta)*p1 + cos(theta)*p2;
}

void rot_y(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = cos(theta)*p0 - sin(theta)*p2;
  pt[1] = p1;
  pt[2] = sin(theta)*p0 + cos(theta)*p2;
}

void rot_z(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = cos(theta)*p0 + sin(theta)*p1;
  pt[1] = -sin(theta)*p0 + cos(theta)*p1;
  pt[2] = p2;
}

void Detector::GetModel(Features& features, double model[9][3]) {
  double center_x = cvRound(features.face_position.x + 0.5*features.face_size);
  double center_y = cvRound(features.face_position.y + 0.5*features.face_size);
  
  model[0][0] = features.nostril_positions[0].x - center_x;
  model[0][1] = features.nostril_positions[0].y - center_y;
  model[0][2] = 1;
  
  model[1][0] = features.nostril_positions[1].x - center_x;
  model[1][1] = features.nostril_positions[1].y - center_y;
  model[1][2] = 1;
  
  model[2][0] = features.lip_positions[0].x - center_x;
  model[2][1] = features.lip_positions[0].y - center_y;
  model[2][2] = 1;
  
  model[3][0] = features.lip_positions[1].x - center_x;
  model[3][1] = features.lip_positions[1].y - center_y;
  model[3][2] = 1;
  
  model[4][0] = features.nose_bridge.x - center_x;
  model[4][1] = features.nose_bridge.y - center_y;
  model[4][2] = 1;
  
  model[5][0] = features.pupils[0].x - center_x;
  model[5][1] = features.pupils[0].y - center_y;
  model[5][2] = 1;
  
  model[6][0] = features.pupils[1].x - center_x;
  model[6][1] = features.pupils[1].y - center_y;
  model[6][2] = 1;
  
  model[7][0] = features.eyebrow_ends[0].x - center_x;
  model[7][1] = features.eyebrow_ends[0].y - center_y;
  model[7][2] = 1;
  
  model[8][0] = features.eyebrow_ends[1].x - center_x;
  model[8][1] = features.eyebrow_ends[1].y - center_y;
  model[8][2] = 1;
}

void copy_arrays(double s[9][3], double d[9][3]) {
  for(size_t i = 0; i < 9; ++i)
  {
    for(size_t j = 0; j < 3; ++j)
    {
      d[i][j] = s[i][j];
    }
  }
}

int compare(const void * a, const void * b) {
  return int(*(double*)a - *(double*)b);
}

#define RANGE 0.3
void Detector::FitModel(Features& features, double model[9][3], double theta[3]) {
  double observed[9][3], new_theta[3], scores[9];
  double min_val = 9999999;
  GetModel(features,observed);
  
  //remove
  double center_x = cvRound(features.face_position.x + 0.5*features.face_size);
  double center_y = cvRound(features.face_position.y + 0.5*features.face_size);
  
  for(double tx = theta[0]-RANGE; tx <= theta[0]+RANGE; tx += 0.05) {
    for(double ty = theta[1]-RANGE; ty <= theta[1]+RANGE; ty += 0.05) {
      for(double tz = theta[2]-RANGE; tz <= theta[2]+RANGE; tz += 0.05) {
        double model_copy[9][3], score;
        copy_arrays(model,model_copy);
        for(size_t i = 0; i < 9; ++i) {
          rot_x(tx,model_copy[i]);
          rot_y(ty,model_copy[i]);
          rot_z(tz,model_copy[i]);
          scores[i] = abs(observed[i][0] - model_copy[i][0]) + abs(observed[i][1] - model_copy[i][1]);
        }
        qsort(scores, 9, sizeof(double), compare);
        score = scores[0]+scores[1]+scores[2]+scores[3];
        if(min_val > score) {
          min_val = score;
          new_theta[0] = tx;
          new_theta[1] = ty;
          new_theta[2] = tz;
          
          // remove
          features.eyebrow_ends[0].x = model[7][0] + center_x;
          features.eyebrow_ends[0].y = model[7][1] + center_y;
          features.eyebrow_ends[1].x = model[8][0] + center_x;
          features.eyebrow_ends[1].y = model[8][1] + center_y;
          features.lip_positions[0].x = model[2][0] + center_x;
          features.lip_positions[0].y = model[2][1] + center_y;
          features.lip_positions[1].x = model[3][0] + center_x;
          features.lip_positions[2].y = model[3][1] + center_y;
        }
      }
    }
    
//    printf("%f %f %f\n", new_theta[0], new_theta[1], new_theta[2]);
    theta[0] = new_theta[0];
    theta[1] = new_theta[1];
    theta[2] = new_theta[2];
  }
}
