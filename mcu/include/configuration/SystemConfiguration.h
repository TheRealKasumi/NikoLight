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
#define SD_CS_PIN 5			 // CS pin for the SD card
#define SD_SPI_SPEED 4000000 // SPI data rate
#define SD_MOUNT_POINT "/sd" // Mount point for the SD card
#define SD_MAX_FILES 5		 // Maximum number of open files

// Logging configuration
#define SERIAL_BAUD_RATE 115200			// Serial baud rate
#define LOG_FILE_NAME "/system_log.txt" // File name of the log file
#define LOG_DEFAULT_LEVEL 1 			// Default log level

// Configuration of the runtime configuration
#define CONFIGURATION_FILE_NAME "/config.tli" // File name of the configuration file

// LED and animator configuration
#define LED_NUM_ZONES 8 											// Number of LED zones
#define LED_DEFAULT_OUTPUT_PINS {13, 14, 15, 16, 17, 21, 22, 25} 	// Pin mapping of zones to physical pins
#define LED_DEFAULT_COUNTS {2, 2, 2, 2, 2, 2, 2, 2}					// Default number of LEDs for each channel
#define LED_DEFAULT_CHANNEL_CURRENT 16 								// Default current per LED channel in mA
#define ANIMATOR_NUM_CUSTOM_FIELDS 15  								// Number of custom fields in the LED configuration
#define ANIMATOR_DEFAULT_TYPE 0		   								// Default animation type
#define ANIMATOR_DEFAULT_SPEED 50	   								// Default animation speed
#define ANIMATOR_DEFAULT_OFFSET 10	   								// Default animation offset
#define ANIMATOR_DEFAULT_BRIGHTNESS 50 								// Default zone brightness
#define ANIMATOR_DEFAULT_REVERSE false 								// Default reversal of the animation
#define ANIMATOR_DEFAULT_FADE_SPEED 30 								// Default fading speed

// Voltage regulator
#define REGULATOR_POWER_LIMIT 12																		// W per regulator
#define REGULATOR_COUNT 2																				// Number of regulators
#define REGULATOR_DEFAULT_VOLTAGE 5.0f																	// Output Voltage 
#define REGULATOR_ZONE_MAPPING {{13, 0}, {14, 1}, {15, 0}, {16, 1}, {17,0}, {21, 1}, {22, 0}, {25, 1}}	// Map a output pin to a regulator index
#define REGULATOR_HIGH_TEMP 70																			// Temp in °C where brightness is reduced
#define REGULATOR_CUT_OFF_TEMP 85																		// Temp in °C where LEDs are turned off

// Cooling fan
#define FAN_PWM_PIN 27			// Output pin for the fan
#define FAN_PWM_CHANNEL 0		// PWM channel for the fan control
#define FAN_PWM_FREQUENCY 60000	// PWM frequency of the fan in Hz
#define FAN_PWM_RESOLUTION 8 	// Resolution of the fan control in bits
#define FAN_PWM_MIN 75		 	// Minimum PWM value for the fan (stall guard)
#define FAN_PWM_MAX 255		 	// Maximum PWm value for the fan
#define FAN_TEMP_MIN 60		 	// Minimum temperature where the fan starts
#define FAN_TEMP_MAX 80		 	// Maximum temeprature where the fan runs at full speed

// I2C configuration
#define IIC_SDA_PIN 32 // SDA pin
#define IIC_SCL_PIN 33 // SCL pin

// OneWire configuration
#define ONE_WIRE_PIN 26 // Pin of the OneWire bus

// Light sensor configuration
#define LIGHT_SENSOR_DEFAULT_MODE 1 			// Default light sensor mode
#define LIGHT_SENSOR_DEFAULT_THRESHOLD 5		// Threshold for the output to turn on/off
#define LIGHT_SENSOR_DEFAULT_MIN_AMBIENT 5		// Minimum brightness of the ambient
#define LIGHT_SENSOR_DEFAULT_MAX_AMBIENT 255	// Maximum brightness of the ambient
#define LIGHT_SENSOR_DEFAULT_MIN_LED 0			// Minimum brightness of the LEDs for brightnes control
#define LIGHT_SENSOR_DEFAULT_MAX_LED 255		// Maximum brightness of the LEDs for brightness control
#define LIGHT_SENSOR_DEFAULT_DURATION 6 		// Time after which the lights are turning off when using the motion sensor (x5 seconds)
#define LIGHT_SENSOR_ADC_PIN 35					// Physical pin for the analog voltage sensor
#define IIC_ADDRESS_BH1750 0x23					// I²C Adress of the BH1750 brightness sensor

// Motion sensor configuration
#define IIC_ADDRESS_MPU6050 0x68 // I²C Adress of the MPU6050 motion sensor

// Temperature sensor
#define TEMP_SENSOR_RESOLUTION 127	// Resolution register of the temperature sensors

// WiFi configuration
#define AP_DEFAULT_SSID "TesLight"		 // Default SSID of the access point
#define AP_DEDAULT_PASSWORD "TesLightPW" // Default password of the access point
#define AP_DEFAULT_CHANNEL 1			 // Default channel of the access point
#define AP_DEFAULT_HIDDEN false			 // Hide or show the SSID by default
#define AP_DEFAULT_MAX_CONN 1			 // Default maximum number of connections to the access point
#define WIFI_DEFAULT_SSID ""			 // Default SSID of a WiFi network
#define WIFI_DEFAULT_PASSWORD ""		 // Default password of a WiFi network

// Webserver configuration
#define WEB_SERVER_PORT 80					   // Port of the web server
#define WEB_SERVER_STATIC_CONTENT "/ui/"  // Static content location for the UI
#define WEB_SERVER_SWAGGER_CONTENT "/swagger/" // Location for swagger api documentation

// Timer configuration
#define LED_FRAME_TIME 16666			// Cycle time for the LEDs in µs
#define TEMP_INTERVAL 250000			// Cycle time for reading temperatures and run fan controll in µs
#define LIGHT_SENSOR_INTERVAL 40000		// Cycle time for the light sensor in µs
#define MOTION_SENSOR_INTERVAL 20000	// Cycle time for the motion sensor in µs
#define WEB_SERVER_INTERVAL 20000		// Cycle time for the web server to accept conenctions in µs
#define STATUS_INTERVAL 5000000			// Cycle time for printing the current status in µs
#define WATCHDOG_RESET_TIME 2			// Time until a watchdog reset is triggered

// FSEQ configuration
#define FSEQ_DIRECTORY "/fseq" // Directory for fseq files

// Update configuration
#define UPDATE_DIRECTORY "/update"	  // Update folder
#define UPDATE_FILE_NAME "update.tup" // Update package file name

#endif
