/**
 * @file SystemConfiguration.h
 * @author TheRealKasumi
 * @brief Contains the (static) system configuration.
 *
 * @copyright Copyright (c) 2023 TheRealKasumi
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef SYSTEM_CONFIGURATION_H
#define SYSTEM_CONFIGURATION_H

// Version configuration
#define FW_VERSION "1.1.0" // Firmware version of the MCU
#define HW_VERSION_2_1     // Hardware version (HW_VERSION_1_0, HW_VERSION_2_0, HW_VERSION_2_1, HW_VERSION_2_2 [!experimental!])

#if defined(HW_VERSION_1_0)
	#define HW_VERSION "1.0"
#elif defined(HW_VERSION_2_0)
	#define HW_VERSION "2.0"
#elif defined(HW_VERSION_2_1)
	#define HW_VERSION "2.1"
#elif defined(HW_VERSION_2_2)
	#define HW_VERSION "2.2"
#else
	#error "A valid hardware version must be defined."
#endif

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
#define CONFIGURATION_FILE_VERSION 14		  // Version of the configuration file
#define CONFIGURATION_FILE_NAME "/config.nli" // File name of the configuration file
#define CONFIGURATION_MAX_PROFILES 50		  // Maximum number of profiles

// LED and effect configuration
#define LED_NUM_ZONES 8 											  // Number of LED zones
#if defined(HW_VERSION_1_0)											  // Pin mapping of zones to physical pins depending on hardware version
	#define LED_DEFAULT_OUTPUT_PINS {13, 14, 15, 16, 17, 21, 22, 25}
#elif defined(HW_VERSION_2_0)
	#define LED_DEFAULT_OUTPUT_PINS {13, 17, 14, 21, 15, 22, 16, 25}
#elif defined(HW_VERSION_2_1)
	#define LED_DEFAULT_OUTPUT_PINS {13, 14, 15, 16, 17, 21, 22, 25}
#elif defined(HW_VERSION_2_2)
	#define LED_DEFAULT_OUTPUT_PINS {4, 12, 13, 14, 15, 16, 17, 21}
#endif
#define LED_DEFAULT_COUNTS {2, 2, 2, 2, 2, 2, 2, 2}					  // Default number of LEDs for each channel
#define LED_DEFAULT_CHANNEL_CURRENT 16 								  // Default current per LED channel in mA
#define LED_MAX_COUNT_PER_ZONE 250									  // Maximum number of LEDs per channel
#define ANIMATOR_NUM_ANIMATION_SETTINGS 25  						  // Number of custom fields in the LED configuration
#define ANIMATOR_DEFAULT_TYPE 0		   								  // Default animation type
#define ANIMATOR_DEFAULT_DATA_SOURCE 0								  // Default data source of the animation
#define ANIMATOR_DEFAULT_SPEED 50	   								  // Default animation speed
#define ANIMATOR_DEFAULT_OFFSET 10	   								  // Default animation offset
#define ANIMATOR_DEFAULT_BRIGHTNESS 50 								  // Default zone brightness
#define ANIMATOR_DEFAULT_REVERSE false 								  // Default reversal of the animation
#define ANIMATOR_DEFAULT_FADE_SPEED 30 								  // Default fading speed

// Voltage regulator
#if defined(HW_VERSION_1_0)																				      
	#define REGULATOR_COUNT 1																				// Number of regulators depending on hardware version
	#define REGULATOR_VOLTAGE 5.0f																			// Output Voltage in V
	#define REGULATOR_CURRENT_LIMIT 2.0f																	// Current limit per regulator in A
	#define REGULATOR_POWER_LIMIT 10																		// Output power limit per regulator in W
	#define REGULATOR_HIGH_TEMP_LIMIT 90																	// Temp limit in °C where brightness is reduced
	#define REGULATOR_CUT_OFF_TEMP_LIMIT 100																// Temp limit in °C where LEDs are turned off
	#define REGULATOR_DEFAULT_POWER_LIMIT 8 																// Default output power per regulator in W
	#define REGULATOR_DEFAULT_HIGH_TEMP 70																	// Temp in °C where brightness is reduced
	#define REGULATOR_DEFAULT_CUT_OFF_TEMP 80																// Temp in °C where LEDs are turned off
	#define REGULATOR_ZONE_MAPPING {{13, 0}, {14, 0}, {15, 0}, {16, 0}, {17,0}, {21, 0}, {22, 0}, {25, 0}}	// Map a output pin to a regulator index depending on hardware version
#elif defined(HW_VERSION_2_0)
	#define REGULATOR_COUNT 2																					// Number of regulators depending on hardware version
	#define REGULATOR_VOLTAGE 5.0f																				// Output Voltage in V
	#define REGULATOR_CURRENT_LIMIT 3.0f																		// Current limit per regulator in A
	#define REGULATOR_POWER_LIMIT 15																			// Output power limit per regulator in W
	#define REGULATOR_HIGH_TEMP_LIMIT 90																		// Temp limit in °C where brightness is reduced
	#define REGULATOR_CUT_OFF_TEMP_LIMIT 100																	// Temp limit in °C where LEDs are turned off
	#define REGULATOR_DEFAULT_POWER_LIMIT 12																	// Default output power per regulator in W
	#define REGULATOR_DEFAULT_HIGH_TEMP 70																		// Temp in °C where brightness is reduced
	#define REGULATOR_DEFAULT_CUT_OFF_TEMP 80																	// Temp in °C where LEDs are turned off
	#define REGULATOR_ZONE_MAPPING {{13, 0}, {17, 1}, {14, 0}, {21, 1}, {15, 0}, {22, 1}, {16, 0}, {25, 1}}		// Map a output pin to a regulator index depending on hardware version
#elif defined(HW_VERSION_2_1)
	#define REGULATOR_COUNT 2																					// Number of regulators depending on hardware version
	#define REGULATOR_VOLTAGE 5.0f																				// Output Voltage in V
	#define REGULATOR_CURRENT_LIMIT 3.0f																		// Current limit per regulator in A
	#define REGULATOR_POWER_LIMIT 15																			// Output power limit per regulator in W
	#define REGULATOR_HIGH_TEMP_LIMIT 90																		// Temp limit in °C where brightness is reduced
	#define REGULATOR_CUT_OFF_TEMP_LIMIT 100																	// Temp limit in °C where LEDs are turned off
	#define REGULATOR_DEFAULT_POWER_LIMIT 12																	// Default output power per regulator in W
	#define REGULATOR_DEFAULT_HIGH_TEMP 70																		// Temp in °C where brightness is reduced
	#define REGULATOR_DEFAULT_CUT_OFF_TEMP 80																	// Temp in °C where LEDs are turned off
	#define REGULATOR_ZONE_MAPPING {{13, 0}, {14, 1}, {15, 0}, {16, 1}, {17, 0}, {21, 1}, {22, 0}, {25, 1}}		// Map a output pin to a regulator index depending on hardware version
#elif defined(HW_VERSION_2_2)
	#define REGULATOR_COUNT 1																					// Number of regulators depending on hardware version
	#define REGULATOR_VOLTAGE 5.0f																				// Output Voltage in V
	#define REGULATOR_CURRENT_LIMIT 8.0f																		// Current limit per regulator in A
	#define REGULATOR_POWER_LIMIT 40																			// Output power limit per regulator in W
	#define REGULATOR_HIGH_TEMP_LIMIT 65																		// Temp limit in °C where brightness is reduced
	#define REGULATOR_CUT_OFF_TEMP_LIMIT 75																		// Temp limit in °C where LEDs are turned off
	#define REGULATOR_DEFAULT_POWER_LIMIT 30																	// Default output power per regulator in W
	#define REGULATOR_DEFAULT_HIGH_TEMP 60																		// Temp in °C where brightness is reduced
	#define REGULATOR_DEFAULT_CUT_OFF_TEMP 70																	// Temp in °C where LEDs are turned off
	#define REGULATOR_ZONE_MAPPING {{4, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0}, {16, 0}, {17, 0}, {21, 0}}		// Map a output pin to a regulator index depending on hardware version
#endif

// I²C configuration
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)	
	#define IIC_SDA_PIN 32			// SDA pin
	#define IIC_SCL_PIN 33			// SCL pin
	#define IIC_FREQUENCY 400000	// I²C bus frequency
	#define AUDIO_UNIT_ADDRESS 42	// I²C address of the NikoLight Audio Unit
	#define BH1750_ADDRESS 0x23		// I²C address of the BH1750
	#define DS3231M_ADDRESS 0x00	// I²C address of the DS3231M
	#define ICM20602_ADDRESS 0x00	// I²C address of the ICM20602
	#define INA3221_ADDRESS 0x00	// I²C address of the INA3221
	#define LM75BD_ADDRESS 0x00		// I²C address of the LM75BD
	#define MPU6050_ADDRESS 0x68	// I²C address of the MPU6050
#elif defined(HW_VERSION_2_2)
	#define IIC_SDA_PIN 32			// SDA pin
	#define IIC_SCL_PIN 33			// SCL pin
	#define IIC_FREQUENCY 400000	// I²C bus frequency
	#define AUDIO_UNIT_ADDRESS 42	// I²C address of the NikoLight Audio Unit
	#define BH1750_ADDRESS 0x23		// I²C address of the BH1750
	#define DS3231M_ADDRESS 0x68	// I²C address of the DS3231M
	#define ICM20602_ADDRESS 0x69	// I²C address of the ICM20602
	#define INA3221_ADDRESS 0x40	// I²C address of the INA3221
	#define LM75BD_ADDRESS 0x48		// I²C address of the LM75BD
	#define MPU6050_ADDRESS 0x69	// I²C address of the MPU6050
#endif

// OneWire configuration
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)	
	#define ONE_WIRE_PIN 26		// Pin of the OneWire bus
#elif defined(HW_VERSION_2_2)
	#define ONE_WIRE_PIN 0		// Pin of the OneWire bus
#endif

// Analog input configuration
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)	
	#define ANALOG_INPUT_PIN 35			 // Physical pin for the analog voltage sensor
	#define ANALOG_INPUT_MODE INPUT		 // Input mode of the analog input
	#define ANALOG_INPUT_MAX_VOLTAGE 3.3 // Maximum input voltage of the analog input
#elif defined(HW_VERSION_2_2)
	#define ANALOG_INPUT_PIN 36			 // Physical pin for the analog voltage sensor
	#define ANALOG_INPUT_MODE INPUT		 // Input mode of the analog input
	#define ANALOG_INPUT_MAX_VOLTAGE 3.3 // Maximum input voltage of the analog input
#endif

// Audio unit configuration
#define AUDIO_UNIT_NUM_BANDS 8																								// Number of frequency bands the audio unit must provide
#define AUDIO_UNIT_DEFAULT_NOISE_THESHOLD 0																					// Peak2Peak Threshold under which analog values are set to the average
#define AUDIO_UNIT_DEFAULT_FREQ_BAND_INDEX {{1, 3}, {4, 6}, {7, 13}, {14, 28}, {29, 62}, {63, 136}, {137, 298}, {299, 450}} // Indices of the frequency bins to map them into the frequency bands
#define AUDIO_UNIT_DEFAULT_PD_HIST_SIZE 60																					// History size of the peak detectors
#define AUDIO_UNIT_DEFAULT_PD_THRESHOLD 1.5																					// Threshold of the peak detectors
#define AUDIO_UNIT_DEFAULT_PD_INFLUENCE 0.75																				// Influence of the peak values on the peak detector
#define AUDIO_UNIT_DEFAULT_PD_NOISE_GATE 1500																				// Noise gate of the peak detector 

// Light sensor configuration
#define LIGHT_SENSOR_DEFAULT_MODE 1 			// Default light sensor mode
#define LIGHT_SENSOR_DEFAULT_THRESHOLD 5		// Threshold for the output to turn on/off
#define LIGHT_SENSOR_DEFAULT_MIN_AMBIENT 5		// Minimum brightness of the ambient
#define LIGHT_SENSOR_DEFAULT_MAX_AMBIENT 255	// Maximum brightness of the ambient
#define LIGHT_SENSOR_DEFAULT_MIN_LED 0			// Minimum brightness of the LEDs for brightnes control
#define LIGHT_SENSOR_DEFAULT_MAX_LED 255		// Maximum brightness of the LEDs for brightness control
#define LIGHT_SENSOR_DEFAULT_DURATION 6 		// Time after which the lights are turning off when using the motion sensor (x5 seconds)

// Temperature sensor configuration
#define TEMP_SENSOR_RESOLUTION 127	// Resolution register of the temperature sensors

// Cooling fan configuration
#if defined(HW_VERSION_1_0) || defined(HW_VERSION_2_0) || defined(HW_VERSION_2_1)	
	#define FAN_PWM_PIN 27			// Output pin for the fan
	#define FAN_DEFAULT_TEMP_MIN 60	// Minimum temperature where the fan starts
	#define FAN_DEFAULT_TEMP_MAX 80	// Maximum temeprature where the fan runs at full speed
#elif defined(HW_VERSION_2_2)
	#define FAN_PWM_PIN 2			// Output pin for the fan
	#define FAN_DEFAULT_TEMP_MIN 45	// Minimum temperature where the fan starts
	#define FAN_DEFAULT_TEMP_MAX 60	// Maximum temeprature where the fan runs at full speed
#endif
#define FAN_PWM_CHANNEL 0		// PWM channel for the fan control
#define FAN_PWM_FREQUENCY 50000	// PWM frequency of the fan in Hz
#define FAN_PWM_RESOLUTION 8 	// Resolution of the fan control in bits
#define FAN_DEFAULT_MODE 0 		// Fan mode, 0 = automatic
#define FAN_DEFAULT_PWM_MIN 75	// Minimum PWM value for the fan (stall guard)
#define FAN_DEFAULT_PWM_MAX 255	// Maximum PWm value for the fan

// WiFi configuration
#define AP_DEFAULT_SSID "NikoLight"		 // Default SSID of the access point
#define AP_DEDAULT_PASSWORD "NikoLightPW" // Default password of the access point
#define AP_DEFAULT_CHANNEL 1			 // Default channel of the access point
#define AP_DEFAULT_HIDDEN false			 // Hide or show the SSID by default
#define AP_DEFAULT_MAX_CONN 1			 // Default maximum number of connections to the access point
#define WIFI_DEFAULT_SSID ""			 // Default SSID of a WiFi network
#define WIFI_DEFAULT_PASSWORD ""		 // Default password of a WiFi network

// Webserver configuration
#define WEB_SERVER_PORT 80					   // Port of the web server
#define WEB_SERVER_STATIC_CONTENT "/ui/"  // Static content location for the UI

// Timer configuration
#define FRAME_INTERVAL 16666			// Interval for outputting to the LEDs in µs
#define FAN_INTERVAL 500000				// Interval for running the fan controll in µs
#define LIGHT_SENSOR_INTERVAL 40000		// Interval for the light sensor in µs
#define MOTION_SENSOR_INTERVAL 20000	// Interval for the motion sensor in µs
#define AUDIO_UNIT_INTERVAL 16666		// Interval for the audio unit in µs
#define WEB_SERVER_INTERVAL 20000		// Interval for the web server to accept conenctions in µs
#define STATUS_INTERVAL 500000			// Interval for collecting new status information in µs
#define STATUS_PRINT_INTERVAL 5000000	// Interval for printing the current status in µs
#define WATCHDOG_RESET_TIME 3			// Time until a watchdog reset is triggered

// FSEQ configuration
#define FSEQ_DIRECTORY "/fseq" // Directory for fseq files

// Update configuration
#define UPDATE_DIRECTORY "/update"	  // Update folder
#define UPDATE_FILE_NAME "update.nup" // Update package file name

// UI configuration
#define UI_DEFAULT_LANGUAGE "en" // Default language of the UI
#define UI_DEFAULT_THEME "dark"	 // Default theme of the UI
#define UI_DEFAULT_EXPERT false  // Deafult setting for the expert mode of the UI

#endif
