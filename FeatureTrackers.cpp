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

void Detector::FitModel(Features& features, Features& model) {
  
}
