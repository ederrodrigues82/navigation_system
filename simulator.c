// simulator.c
// step resolution is 1.8 degrees, 200 measures by scan
// frequency of IRDA is 30Hz -> each 33ms
// simulate a room with 1.41m x 1.41m

#include <math.h>
#include "simulator.h"

// Global variable initialized to 0
int step = 0;  
double x_position = 0;
double y_position = 0;
double pitch_orientation = 0;
double roll_orientation = 0;
double yaw_orientation = 0;

double get_next_r() {
    step = (step < SCAN_LENGHT) ? step : 0;   
    double theta = (step) * M_PI/100;    
    if (theta >= 0 && theta <= 0.785) {
        return 1410.0 / cos(theta);
    } else if ((theta >= 2.355 && theta <= 3.15)) {
        return 1410.0 / -cos(theta);
    } else if (theta > 0.785 && theta < 2.355) {
        return 1410.0 / sin(theta);
    } else {
        return 0.0;  // Outside defined intervals
    }
}

int get_position(double* pos_x, double* pos_y, double* pitch, double* roll, double* yaw) {
    *pos_x += 50;
    *pos_y = 0;
    *pitch = 0;
    *roll = 0;
    *yaw = 0;
    
    return 0;
}