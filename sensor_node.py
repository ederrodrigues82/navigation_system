#!/usr/bin/env python3

import rospy
import time
import board
import busio
import math

from sensor_msgs.msg import LaserScan, Imu

try:
    from adafruit_vl53l0x import VL53L0X
    from adafruit_bno08x.i2c import BNO08X_I2C
    from adafruit_bno08x import BNO_REPORT_ROTATION_VECTOR
    physical_sensors = True
except ImportError:
    from sensor_simulator import VL53L0XSimulator, BNO08XSimulator
    physical_sensors = False

# Inicialização do I2C
i2c = busio.I2C(board.SCL, board.SDA)

# Inicialização dos sensores
if physical_sensors:
    vl53 = VL53L0X(i2c)
    imu = BNO08X_I2C(i2c)
    imu.enable_feature(BNO_REPORT_ROTATION_VECTOR)
else:
    vl53 = VL53L0XSimulator()
    imu = BNO08XSimulator()

# Configura ROS
rospy.init_node('sensor_node')

scan_pub = rospy.Publisher('/scan', LaserScan, queue_size=10)
imu_pub = rospy.Publisher('/imu', Imu, queue_size=10)

rate = rospy.Rate(10)  # 10 Hz

def publish_scan():
    scan_msg = LaserScan()
    scan_msg.header.stamp = rospy.Time.now()
    scan_msg.header.frame_id = "laser_frame"
    scan_msg.angle_min = -math.pi / 2
    scan_msg.angle_max = math.pi / 2
    scan_msg.angle_increment = math.radians(1)
    scan_msg.range_min = 0.05
    scan_msg.range_max = 2.0

    # Simulando leitura 180° (você deve mover o motor aqui externamente)
    readings = []
    for angle in range(180):
        dist = vl53.range / 100.0  # em metros
        if 0.05 <= dist <= 2.0:
            readings.append(dist)
        else:
            readings.append(float('inf'))
        time.sleep(0.005)  # tempo entre medições

    scan_msg.ranges = readings
    scan_pub.publish(scan_msg)

def publish_imu():
    quat = imu.rotation_vector  # (x, y, z, w)
    imu_msg = Imu()
    imu_msg.header.stamp = rospy.Time.now()
    imu_msg.header.frame_id = "base_link"

    imu_msg.orientation.x = quat[0]
    imu_msg.orientation.y = quat[1]
    imu_msg.orientation.z = quat[2]
    imu_msg.orientation.w = quat[3]

    imu_pub.publish(imu_msg)

# Loop principal
while not rospy.is_shutdown():
    try:
        if not physical_sensors:
            vl53.update()
            imu.update()
        publish_scan()
        publish_imu()
        rate.sleep()
    except Exception as e:
        rospy.logerr(f"Erro na leitura do sensor: {e}")
