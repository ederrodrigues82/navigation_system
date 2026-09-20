// main.c
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "simulator.h"

int send_i2c(char* message);
void print_irda(double *irda);
int erase_i2c_file();
int convert_to_polar(double x, double y, double* r, double* theta);
int convert_to_cartesian(double r, double theta, double* x, double* y );
int get_position(double* pos_x, double* pos_y, double* pitch, double* roll, double* yaw);
int send_measures(double* x_values, double* y_values, int size, char* x_tag, char* y_tag);


int main() {
    double irda_x_values[SCAN_LENGHT] = {0};
    double irda_y_values[SCAN_LENGHT] = {0};
    double pos_x_values[SCAN_LENGHT] = {0};
    double pos_y_values[SCAN_LENGHT] = {0};
    double pos_pitch_values[SCAN_LENGHT] = {0};
    double pos_roll_values[SCAN_LENGHT] = {0};
    double pos_yaw_values[SCAN_LENGHT] = {0};        
    double irda_r;
    double irda_theta;
    double irda_pos_x;
    double irda_pos_y;
    int index = 0;

    step = 0;
    erase_i2c_file();
    
    do {
        get_next_r_theta(&irda_r, &irda_theta);
        get_position(&pos_x_values[step], &pos_y_values[step], &pos_pitch_values[step], &pos_roll_values[step], &pos_yaw_values[step]); 
        if (irda_r != -1) {                   
            convert_to_cartesian(irda_r, irda_theta, &irda_pos_x, &irda_pos_y);
            irda_x_values[index] = pos_x_values[step] + irda_pos_x;
            irda_y_values[index] = pos_x_values[step] + irda_pos_y;
            index++;            
        }
        
        step++;
        
        if (step > SCAN_LENGHT - 1) {
            send_measures(irda_x_values, irda_y_values, index, "#irda_scans_x:", "#irda_scans_y:");
            send_measures(pos_x_values, pos_y_values, index, "#pos_scans_x:", "#pos_scans_y:");
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

int send_measures(double* x_values, double* y_values, int size, char* x_tag, char* y_tag) {
    char message[4000] = "";
    char buffer[32];  // Temporary buffer for each string fragment
    
    strcpy(message, x_tag);
    for (int i = 0; i < size; i++)
    {
        if (x_values)
        sprintf(buffer, " %8.3f", x_values[i]);  // Format each index
        strcat(message, buffer);          // Append to result string
    }

    strcat(message, "\n");
    strcat(message, y_tag);
    for (int i = 0; i < size; i++)
    {
        sprintf(buffer, " %8.3f", y_values[i]);  // Format each index
        strcat(message, buffer);          // Append to result string
    }
    send_i2c(message);
    print_irda(x_values);
    
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
    FILE *file = fopen("rasp_i2c_in_file.txt", "a");  // Open file for writing
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fprintf(file, "%s\n", message);  // Write string to file
    fclose(file);  // Close file
    return(0);
}

int erase_i2c_file() {
    FILE *file = fopen("rasp_i2c_in_file.txt", "w");  // Truncate the file
    if (file == NULL) {
        perror("Error erasing file");
        return 1;
    }

    fclose(file);  // Immediately close to apply truncation
    return 0;
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