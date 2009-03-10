#include "Detector.h"
#include <cstdio>

void inline rot_x(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = p0;
  pt[1] = cos(theta)*p1 + sin(theta)*p2;
  pt[2] = -sin(theta)*p1 + cos(theta)*p2;
}

void inline rot_y(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = cos(theta)*p0 - sin(theta)*p2;
  pt[1] = p1;
  pt[2] = sin(theta)*p0 + cos(theta)*p2;
}

void inline rot_z(double theta, double pt[3]) {
  double p0 = pt[0], p1 = pt[1], p2 = pt[2];
  pt[0] = cos(theta)*p0 + sin(theta)*p1;
  pt[1] = -sin(theta)*p0 + cos(theta)*p1;
  pt[2] = p2;
}

void Detector::GetModel(Features& features, double model[9][3]) {
  double center_x = features.face_position.x;
  double center_y = features.face_position.y;
  
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

void inline copy_arrays(double s[9][3], double d[9][3], double scale = 1) {
  for(size_t i = 0; i < 9; ++i)
  {
    for(size_t j = 0; j < 3; ++j)
    {
      d[i][j] = scale*s[i][j];
    }
  }
}

int compare(const void * a, const void * b) {
  return int(*(double*)a - *(double*)b);
}

#define RANGE 0.75
void Detector::FitModel(Features& features, double model[9][3], Features *model_features) {    
  double observed[9][3], best_fit[9][3], new_theta, scores[9], new_center[2];
  double min_val = 9999999;
  GetModel(features,observed);
  
  double center_x = features.face_position.x;
  double center_y = features.face_position.y;
  double min_z = 1;
  
  for(double tz = features.theta-RANGE; tz <= features.theta+RANGE; tz += 0.025) {
    for(double cx = -30; cx <= 30; cx+=3) {
      for(double cy = -30; cy <= 30; cy+=3) {
      for(double z = features.z-0.1; z <= features.z+0.1; z+=0.05) {
          double model_copy[9][3], score;
          copy_arrays(model,model_copy);
          for(int i = 0; i < 9; i++) {
            rot_z(tz,model_copy[i]);
//          scores[i] = pow(observed[i][0]-cx - model_copy[i][0],2) + pow(observed[i][1]-cy - model_copy[i][1],2);
            scores[i] = pow(observed[i][0]-cx - z*model_copy[i][0],2) + pow(observed[i][1]-cy - z*model_copy[i][1],2);
          }
          qsort(scores, 9, sizeof(double), compare);

          score = scores[0]+scores[1]+scores[2]+scores[3]+scores[4]+scores[5];
          if(min_val > score) {              
            min_val = score;
            new_theta = tz;
            new_center[0] = center_x + cx;
            new_center[1] = center_y + cy;
            copy_arrays(model_copy,best_fit);
            min_z = z;
         }
        }
      }
    }
  }
  
//printf("min_z: %f\n", min_z);

  features.theta = new_theta;
  features.z = min_z;
  features.face_position.x = new_center[0];
  features.face_position.y = new_center[1];

  if(model_features) {
    model_features->theta = new_theta;
    model_features->z = min_z;
    model_features->face_position.x = new_center[0];
    model_features->face_position.y = new_center[1];
    model_features->nostril_positions[0].x = min_z*best_fit[0][0] + new_center[0];
    model_features->nostril_positions[0].y = min_z*best_fit[0][1] + new_center[1];
    model_features->nostril_positions[1].x = min_z*best_fit[1][0] + new_center[0];
    model_features->nostril_positions[1].y = min_z*best_fit[1][1] + new_center[1];
    model_features->lip_positions[0].x = min_z*best_fit[2][0] + new_center[0];
    model_features->lip_positions[0].y = min_z*best_fit[2][1] + new_center[1];
    model_features->lip_positions[1].x = min_z*best_fit[3][0] + new_center[0];
    model_features->lip_positions[1].y = min_z*best_fit[3][1] + new_center[1];
    model_features->nose_bridge.x = min_z*best_fit[4][0] + new_center[0];
    model_features->nose_bridge.y = min_z*best_fit[4][1] + new_center[1];
    model_features->pupils[0].x = min_z*best_fit[5][0] + new_center[0];
    model_features->pupils[0].y = min_z*best_fit[5][1] + new_center[1];
    model_features->pupils[1].x = min_z*best_fit[6][0] + new_center[0];
    model_features->pupils[1].y = min_z*best_fit[6][1] + new_center[1];
    model_features->eyebrow_ends[0].x = min_z*best_fit[7][0] + new_center[0];
    model_features->eyebrow_ends[0].y = min_z*best_fit[7][1] + new_center[1];
    model_features->eyebrow_ends[1].x = min_z*best_fit[8][0] + new_center[0];
    model_features->eyebrow_ends[1].y = min_z*best_fit[8][1] + new_center[1];
  }
}