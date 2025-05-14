// main.c
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "simulator.h"

int send_i2c(char* message);
void print_irda(double *irda);
int send_irda_scan(double* irda);

int main() {
    step = 0;
    double irda[SCAN_LENGHT] = {0};

    do {
        irda[step] = get_next_r();
        step++;
        if (step > SCAN_LENGHT - 1) {
            send_irda_scan(irda);
        }
        
    } while (step < SCAN_LENGHT);

    return 0;
}

int send_irda_scan(double* irda) {
    char message[2000] = "#scans:";
    char buffer[32];  // Temporary buffer for each string fragment
    for (int i = 0; i < SCAN_LENGHT; i++)
    {
        sprintf(buffer, " %8.3f", irda[i]);  // Format each index
        strcat(message, buffer);          // Append to result string
    }
    print_irda(irda);
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

void print_irda(double *irda) {
    for (int i = 0; i < SCAN_LENGHT; i++)
    {
        double theta_deg = i*1.8;
        double theta_rad = i * M_PI/100;   
        printf("\n%-8s %-8s %-8s %-8s %-8s %-8s", "step", "rad", "deg", "cos", "sen", "r");
        printf("\n%-8d %-8.3f %-8.3f %-8.3f %-8.3f %-8.3f", i, theta_rad, theta_deg, cos(theta_rad), sin(theta_rad), irda[i]);
    }
}