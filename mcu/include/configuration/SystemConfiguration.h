/**
 * @file SystemConfiguration.h
 * @author TheRealKasumi
 * @brief Contains the (static) system configuration.
 * @version 0.0.1
 * @date 2022-06-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SYSTEM_CONFIGURATION_H
#define SYSTEM_CONFIGURATION_H

// SD configuration
#define SD_CS_PIN 5

// Logging configuration
#define SERIAL_BAUD_RATE 460800
#define LOG_FILE_NAME "/system_log.txt"
#define MIN_LOG_LEVEl TesLight::Logger::LogLevel::INFO

// Configuration of the runtime configuration
#define CONFIGURATION_FILE_NAME "/config.tli"

// LED driver and animator configuration
// When changing the driver number make sure
// to also adjust the default values in {@link TesLight::Configuration}
#define NUM_LED_DRIVERS 6
#define NUM_ANIMATOR_CUSTOM_FIELDS 10

// I2C configuration
#define SDA_PIN 32
#define SCL_PIN 33

// Switch sensor configuration
#define SWITCH_SENSOR_PIN 35
#define SWITCH_SENSOR_THRESHOLD 75

// Motion sensor configuration
#define MOTION_SENSOR_I2C_ADDRESS 0x68
#define MOTION_SENSOR_BUFFER_SIZE 5

// Webserver configuration
#define WEB_SERVER_PORT 80
#define WEB_SERVER_STATIC_CONTENT "/web-app"

// Timer configuration
#define LED_CYCLE_TIME 15
#define SWITCH_SENSOR_CYCLE_TIME 20
#define MOTION_SENSOR_CYCLE_TIME 15
#define FPS_CYCLE_TIME 10000

#endif