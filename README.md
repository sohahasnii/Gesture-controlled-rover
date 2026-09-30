# Gesture-Controlled Smart Rover

A wireless **gesture-controlled smart rover** based on **ESP32** that allows the user to control the rover's movement through hand gestures. The system also includes **ultrasonic obstacle detection** and a **buzzer alert** to improve safety during operation.

---

## Overview

Traditional remote-controlled robots require physical buttons or joysticks for navigation. This project provides a more intuitive control method by using **hand gestures**.

An **MPU6050 motion sensor** detects the orientation of the user's hand. The gesture information is processed by an ESP32 transmitter and sent wirelessly to a second ESP32 mounted on the rover.

The rover receives the movement command and controls its motors through an **L298N motor driver**.

At the same time, an **ultrasonic sensor** monitors the path ahead. When an obstacle is detected within a defined distance, the system activates a **buzzer alert** to warn the user.

---
