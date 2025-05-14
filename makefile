# Compiler and flags
CC = gcc
CFLAGS = -Wall -O0 -g   # -g enables debugging, -O0 disables optimizations for easier debugging

# Sources and output
OBJ = main.o simulator.o
TARGET = program

# Build target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET)
