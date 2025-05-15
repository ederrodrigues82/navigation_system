import math
import matplotlib.pyplot as plt


SIZE = 1412
SCAN_LENGHT = 100

def main():
    x_values = []
    y_values = []
    #Read irda values from file
    with open("rasp_i2c_in_file.txt", "r") as file:
        for line in file:
            # Remove extra whitespace and split the line
            line = line.strip()

            if line.startswith("#irda_scans_x"):
                # Remove label and split numbers
                x_values = [float(val) for val in line.replace("#irda_scans_x:", "").split()]

            elif line.startswith("#irda_scans_y"):
                y_values = [float(val) for val in line.replace("#irda_scans_y:", "").split()]

    cartesian_plot(x_values, y_values)

def polar_plot(angles, radius):
    
    # generating graph
    plt.figure(figsize=(6, 6))
    ax = plt.subplot(111, polar=True)
    ax.plot(angles, radius)

    # Showing the graph
    plt.title("Polar cordinates graph")
    plt.show()
    
def cartesian_plot(x_values, y_values):
    # Plot the points
    plt.plot(x_values, y_values, marker='o')  # Line plot with points

    # Labels and grid
    plt.xlabel("X-axis")
    plt.ylabel("Y-axis")
    plt.title("Cartesian Graph")
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    main()
