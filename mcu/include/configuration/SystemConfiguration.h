/**
 * @file SystemConfiguration.h
 * @author TheRealKasumi
 * @brief Contains the (static) system configuration.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SYSTEM_CONFIGURATION_H
#define SYSTEM_CONFIGURATION_H

// SD configuration
#define SD_CS_PIN 5
#define SD_SPI_SPEED 4000000
#define SD_MOUNT_POINT "/sd"
#define SD_MAX_FILES 15

// Logging configuration
#define SERIAL_BAUD_RATE 460800
#define LOG_FILE_NAME "/system_log.txt"

// Configuration of the runtime configuration
#define CONFIGURATION_FILE_NAME "/config.tli"

// LED and animator configuration
// When changing the strip number make sure
// to also adjust the default values in {@link TesLight::Configuration}
#define NUM_LED_STRIPS 6
#define NUM_ANIMATOR_CUSTOM_FIELDS 10

// I2C configuration
#define SDA_PIN 32
#define SCL_PIN 33

// Light sensor configuration
#define LIGHT_SENSOR_PIN 35
#define LIGHT_SENSOR_BUFFER_SIZE 10

// Motion sensor configuration
#define MOTION_SENSOR_I2C_ADDRESS 0x68
#define MOTION_SENSOR_BUFFER_SIZE 5

// Webserver configuration
#define WEB_SERVER_PORT 80
#define WEB_SERVER_STATIC_CONTENT "/web-app"

// Timer configuration
#define LED_FRAME_TIME 16666
#define LIGHT_SENSOR_CYCLE_TIME 20000
#define MOTION_SENSOR_CYCLE_TIME 20000
#define FPS_CYCLE_TIME 10000000

// FSEQ configuration
#define FSEQ_DIRECTORY "/fseq"

#endif
