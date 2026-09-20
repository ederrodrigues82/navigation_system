import math
import matplotlib.pyplot as plt


SIZE = 1412
SCAN_LENGHT = 100

def main():
    irda_x_values = []
    irda_y_values = []
    pos_x_values = []
    pos_y_values = []
    
    #Read irda and pos values from file
    with open("rasp_i2c_in_file.txt", "r") as file:
        for line in file:
            # Remove extra whitespace and split the line
            line = line.strip()

            if line.startswith("#irda_scans_x"):
                # Remove label and split numbers
                irda_x_values = [float(val) for val in line.replace("#irda_scans_x:", "").split()]

            elif line.startswith("#irda_scans_y"):
                irda_y_values = [float(val) for val in line.replace("#irda_scans_y:", "").split()]
                
            elif line.startswith("#pos_scans_x"):
                pos_x_values = [float(val) for val in line.replace("#pos_scans_x:", "").split()]
                
            elif line.startswith("#pos_scans_y"):
                pos_y_values = [float(val) for val in line.replace("#pos_scans_y:", "").split()]

    cartesian_plot(irda_x_values, irda_y_values, pos_x_values, pos_y_values)

def polar_plot(angles, radius):
    
    # generating graph
    plt.figure(figsize=(6, 6))
    ax = plt.subplot(111, polar=True)
    ax.plot(angles, radius)

    # Showing the graph
    plt.title("Polar cordinates graph")
    plt.show()
    
def cartesian_plot(irda_x_values, irda_y_values, pos_x_values, pos_y_values):
    # Plot IRDA points in green
    plt.scatter(irda_x_values, irda_y_values, color='blue', marker='o', label='IRDA')

    # Plot POS points in blue
    plt.scatter(pos_x_values, pos_y_values, color='red', marker='x', label='POS')

    # Labels, grid, legend
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.title("Dynamic Map")
    plt.grid(True)
    plt.legend()

    # Keep chart open
    plt.show(block=True)

if __name__ == "__main__":
    main()
