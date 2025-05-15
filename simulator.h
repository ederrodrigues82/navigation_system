// simulator.h
#ifndef SIMULATOR_H
#define SIMULATOR_H

extern int step;  // global step variable

#define SCAN_LENGHT  100
int get_next_r_theta(double* r, double* theta);
int get_position(double* pos_x, double* pos_y, double* pitch, double* roll, double* yaw);

#endif // SIMULATOR_H