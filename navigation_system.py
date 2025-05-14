import math
import numpy as np
import matplotlib.pyplot as plt


SIZE = 1412
SCAN_LENGHT = 100

def main():
    # 1. Read irda values from file
    with open("rasp_i2c_in_file.txt", "r") as f:
        line = f.readline()

    # 2. Extract values after "#scans:"
    parts = line.strip().split()
    irda_values = list(map(float, parts[1:]))

    # 3. Initialize empty grid with '.'
    grid = [['.' for _ in range(SIZE)] for _ in range(SIZE)]

    # 4. Compute x, y and plot on grid
    angles = [i * (math.pi / 100) for i in range(100)]
    polar_plot(angles, irda_values)

    for i in range(SCAN_LENGHT):
        angle = i * math.pi/100
        
        try:
            x = irda_values[i] * abs(math.cos(angle))
            y = math.sqrt(abs(irda_values[i]*irda_values[i] - x*x))
        except ZeroDivisionError:
            continue  # Skip divide-by-zero

        xi = round(x)
        yi = round(y)

        # Flip y-axis for bottom-left origin
        flipped_yi = SIZE - yi - 1

        # Check bounds
        if 0 <= xi < SIZE and 0 <= flipped_yi < SIZE:
            grid[xi][flipped_yi] = 'X'
        print(f"{xi}, {flipped_yi}")

    # 5. Write grid to file
    with open("grid_output.txt", "w") as f:
        for row in grid:
            f.write(''.join(row) + '\n')

    print("Grid saved to grid_output.txt with (0,0) at bottom-left")

# 6. Plot graph in polar cordinates

# Suponha que estas listas contenham 100 valores cada
def polar_plot(angles, radius):
    
    # generating graph
    plt.figure(figsize=(6, 6))
    ax = plt.subplot(111, polar=True)
    ax.plot(angles, radius)

    # Showing the graph
    plt.title("Polar cordinates graph")
    plt.show()

if __name__ == "__main__":
    main()
