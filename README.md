# IoT-Based Medical Monitoring System

## Overview
This repository contains the source code for an IoT-based medical monitoring system. The system continuously monitors patient proximity and environmental conditions, triggering local alarms and sending cloud-based MATLAB analysis alerts when critical safety thresholds are exceeded.

## Hardware Components
* **Microcontroller:** ESP32
* **Sensors:** DHT22 (Temperature & Humidity), HC-SR04 (Proximity), MQ Gas Sensor (Air Quality)
* **Output:** I2C LCD (16x2), LEDs, and Piezo Buzzer

## Software & Cloud Platforms
* **Firmware:** Arduino IDE (C/C++)
* **IoT Cloud:** ThingSpeak Platform for real-time data logging
* **Cloud Processing:** MATLAB Analysis for threshold evaluation and email notifications

## Files Included
* `sketch_ESP32_patient_monitor_project.ino`: The main firmware for the ESP32 microcontroller.
* `MATLAB_Alert.m`: The MATLAB script executed on ThingSpeak to generate the 'Integrated Patient Status Report'.
