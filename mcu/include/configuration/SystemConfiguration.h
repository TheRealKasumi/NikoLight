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
#define SD_MAX_FILES 5

// Logging configuration
#define SERIAL_BAUD_RATE 460800
#define LOG_FILE_NAME "/system_log.txt"
#define LOG_DEFAULT_LEVEL TesLight::Logger::LogLevel::INFO

// Configuration of the runtime configuration
#define CONFIGURATION_FILE_NAME "/config.tli"
#define CONFIGURATION_FILE_VERSION 3

// LED and animator configuration
#define LED_NUM_ZONES 8
#define LED_OUTPUT_PINS {13, 17, 14, 21, 15, 22, 16, 25} // Should balance the laod between the regulators well
#define LED_COUNTS {2, 2, 2, 2, 2, 2, 2, 2}
#define LED_DEFAULT_CHANNEL_CURRENT 14
#define ANIMATOR_NUM_CUSTOM_FIELDS 15
#define ANIMATOR_DEFAULT_TYPE 0
#define ANIMATOR_DEFAULT_SPEED 50
#define ANIMATOR_DEFAULT_OFFSET 10
#define ANIMATOR_DEFAULT_BRIGHTNESS 50
#define ANIMATOR_DEFAULT_REVERSE false
#define ANIMATOR_DEFAULT_FADE_SPEED 30

// Voltage regulator
#define REGULATOR_POWER_LIMIT 10 // W per regulator
#define REGULATOR_COUNT 2
#define REGULATOR_DEFAULT_VOLTAGE 50 // Voltage x10
#define REGULATOR_ZONE_MAPPING {0, 1, 0, 1, 0, 1, 0, 1} // Maps zone index to regulator number

// I2C configuration
#define SDA_PIN 32
#define SCL_PIN 33

// Light sensor configuration
#define LIGHT_SENSOR_PIN 35
#define LIGHT_SENSOR_BUFFER_SIZE 10
#define LIGHT_SENSOR_DEFAULT_MODE TesLight::LightSensor::LightSensorMode::ALWAYS_ON
#define LIGHT_SENSOR_DEFAULT_THRESHOLD 30
#define LIGHT_SENSOR_DEFAULT_MIN 30
#define LIGHT_SENSOR_DEFAULT_MAX 2048

// Motion sensor configuration
#define MOTION_SENSOR_I2C_ADDRESS 0x68
#define MOTION_SENSOR_BUFFER_SIZE 5

// WiFi configuration
#define AP_DEFAULT_SSID "TesLight"
#define AP_DEDAULT_PASSWORD "TesLightPW"
#define AP_DEFAULT_CHANNEL 1
#define AP_DEFAULT_HIDDEN false
#define AP_DEFAULT_MAX_CONN 1
#define WIFI_DEFAULT_SSID ""
#define WIFI_DEFAULT_PASSWORD ""

// Webserver configuration
#define WEB_SERVER_PORT 80
#define WEB_SERVER_STATIC_CONTENT "/web-app/"

// Timer configuration
#define LED_FRAME_TIME 16666
#define LIGHT_SENSOR_CYCLE_TIME 20000
#define MOTION_SENSOR_CYCLE_TIME 20000
#define FPS_CYCLE_TIME 10000000
#define WEB_SERVER_CYCLE_TIME 20000

// FSEQ configuration
#define FSEQ_DIRECTORY "/fseq"

// Update configuration
#define UPDATE_DIRECTORY "/update"
#define UPDATE_FILE_NAME "update.tup"

#endif
