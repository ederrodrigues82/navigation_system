// main.c
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "simulator.h"

int send_i2c(char* message);
void print_irda(double *irda);
int send_irda_scan(double* irda_x_values, double* irda_y_values, int size);
int convert_to_polar(double x, double y, double* r, double* theta);
int convert_to_cartesian(double r, double theta, double* x, double* y );
int get_position(double* pos_x, double* pos_y, double* pitch, double* roll, double* yaw);

int main() {
    step = 0;
    double irda_x_values[SCAN_LENGHT] = {0};
    double irda_y_values[SCAN_LENGHT] = {0};
    double irda_r;
    double irda_theta;
    double pos_x;
    double pos_y;
    double pitch;
    double roll;
    double yaw;
    double irda_pos_x;
    double irda_pos_y;
    int index = 0;

    do {
        get_next_r_theta(&irda_r, &irda_theta);
        if (irda_r != -1) {
            get_position(&pos_x, &pos_y, &pitch, &roll, &yaw);        
            convert_to_cartesian(irda_r, irda_theta, &irda_pos_x, &irda_pos_y);
            irda_x_values[index] = pos_x + irda_pos_x;
            irda_y_values[index] = pos_y+ irda_pos_y;
            index++;            
        }
        
        step++;
        
        if (step > SCAN_LENGHT - 1) {
            send_irda_scan(irda_x_values, irda_y_values, index);
        }
        
    } while (step < SCAN_LENGHT);

    return 0;
}

int convert_to_polar(double x, double y, double* r, double* theta) {
    *r = sqrt(x * x + y * y);  // Calculate radius
    *theta = atan2(y, x);      // Calculate angle (in radians)
    return 0;
}

int convert_to_cartesian(double r, double theta, double* x, double* y ) {
    *x = r * cos(theta);
    *y = r * sin(theta);
    return 0;
}

int send_irda_scan(double* irda_x_values, double* irda_y_values, int size) {
    char message[4000] = "#irda_scans_x:";
    char buffer[32];  // Temporary buffer for each string fragment
    
    for (int i = 0; i < size; i++)
    {
        if (irda_x_values)
        sprintf(buffer, " %8.3f", irda_x_values[i]);  // Format each index
        strcat(message, buffer);          // Append to result string
    }

    strcat(message, "\n#irda_scans_y: ");
    for (int i = 0; i < size; i++)
    {
        sprintf(buffer, " %8.3f", irda_y_values[i]);  // Format each index
        strcat(message, buffer);          // Append to result string
    }
    send_i2c(message);
    print_irda(irda_x_values);
    
    return(0);
}

int send_position(double pos_x, double pos_y, double pitch, double roll, double yaw) {
    char message[500] = "#position:";    
    for (int i = 0; i < SCAN_LENGHT; i++)
    {
        sprintf(message, " %8.3f %8.3f %8.3f %8.3f %8.3f", pos_x, pos_y, pitch, roll, yaw);  // Format each index        
    }    
    send_i2c(message);
    return(0);
}

int send_i2c(char* message) {
    FILE *file = fopen("rasp_i2c_in_file.txt", "w");  // Open file for writing
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fprintf(file, "%s\n", message);  // Write string to file
    fclose(file);  // Close file
    return(0);
}

void print_irda(double *irda_r) {
    for (int i = 0; i < SCAN_LENGHT; i++)
    {
        double theta_deg = i*1.8;
        double theta_rad = i * M_PI/100;   
        printf("\n%-8s %-8s %-8s %-8s %-8s %-8s", "step", "rad", "deg", "cos", "sen", "r");
        printf("\n%-8d %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f", i, theta_rad, theta_deg, cos(theta_rad), sin(theta_rad), irda_r[i]);
    }
}