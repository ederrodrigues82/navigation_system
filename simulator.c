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

int get_next_r_theta(double* r, double* theta) {
    step = (step < SCAN_LENGHT) ? step : 0;   
    *theta = (step) * M_PI/100;    
    
//integration with position system
    if (step % 3 == 0) {
        y_position += 100; //each 3 irda measure increment 100mm on the position
    }

    //simulating an linear obstacle on right side
    if (*theta >= 0 && *theta <= 0.785) {
        *r = 1410.0 / cos(*theta);
    } else {
        *r = -1;
    }

    return 0;
}

int get_position(double* pos_x, double* pos_y, double* pitch, double* roll, double* yaw) {
    *pos_x = 0;
    *pos_y = y_position;
    *pitch = 0;
    *roll = 0;
    *yaw = 0;
    
    return 0;
}