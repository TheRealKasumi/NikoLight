/**
 * @file TesLight.h
 * @author TheRealKasumi
 * @brief Static class encapsulating all TesLight functionality.
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "TesLight.h"

std::unique_ptr<TL::Configuration> TesLight::configuration = nullptr;
std::unique_ptr<TL::FanController> TesLight::fanController = nullptr;
std::unique_ptr<TL::LedManager> TesLight::ledManager = nullptr;
std::unique_ptr<TL::TemperatureSensor> TesLight::temperatureSensor = nullptr;
std::unique_ptr<TL::LightSensor> TesLight::lightSensor = nullptr;
std::unique_ptr<TL::MotionSensor> TesLight::motionSensor = nullptr;
std::unique_ptr<TL::WiFiManager> TesLight::wifiManager = nullptr;
std::unique_ptr<TL::WebServerManager> TesLight::webServerManager = nullptr;
unsigned long TesLight::ledInterval = LED_FRAME_TIME;
unsigned long TesLight::lightSensorInterval = LIGHT_SENSOR_INTERVAL;
unsigned long TesLight::motionSensorInterval = MOTION_SENSOR_INTERVAL;
unsigned long TesLight::webServerInterval = WEB_SERVER_INTERVAL;
unsigned long TesLight::statusInterval = STATUS_INTERVAL;
unsigned long TesLight::temperatureInterval = TEMP_INTERVAL;
unsigned long TesLight::ledTimer = 0;
unsigned long TesLight::lightSensorTimer = 0;
unsigned long TesLight::motionSensorTimer = 0;
unsigned long TesLight::webServerTimer = 0;
unsigned long TesLight::statusTimer = 0;
unsigned long TesLight::temperatureTimer = 0;
uint16_t TesLight::ledFrameCounter = 0;
float TesLight::ledPowerCounter = 0.0f;

/**
 * @brief Initialize the TesLight software and hardware.
 */
void TesLight::begin()
{
	initializeLogger(false);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight starting up."));
	printLogo();

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize SD card."));
	if (initializeSdCard())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("SD card initialized."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize SD card. Rebooting..."));
		TL::Updater::reboot("SD mount failure.", 0);
		while (true)
		{
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switching to SD card logger."));
	delay(500);
	if (initializeLogger(true))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Switched to SD card logger."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to switch to SD card logger. Rebooting..."));
		TL::Updater::reboot("SD logger failure.", 0);
		while (true)
		{
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Check if system update is available."));
	if (updateAvilable())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System update package was found. Installing..."));
		handleUpdate();
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("No available system update found."));
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize, load and save configuration."));
	if (initializeConfiguration())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Runtime configuration loaded."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::WARN, SOURCE_LOCATION, F("Failed to load configuration. Loading defaults and continue."));
		configuration->loadDefaults();
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Updating log level from configuration."));
	const TL::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
	TL::Logger::setMinLogLevel((TL::Logger::LogLevel)systemConfig.logLevel);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Log level updated from configuration."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize I²C bus."));
	intializeI2C();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("I²C bus initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize Fan Controller."));
	initializeFanController();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Fan Controller initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize LED Manager."));
	initializeLedManager();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED Manager initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize temperature sensor."));
	initializeTemperatureSensor();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Temperature sensor initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize light sensor."));
	initializeLightSensor();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Light sensor initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize motion sensor."));
	if (initializeMotionSensor())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Motion sensor initialized."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize motion sensor. Continue without motion sensor data."));
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize WiFiManager."));
	initializeWiFiManager();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi manager initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Starting Webserver."));
	initializeWebServerManager();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("Webserver started on port ") + WEB_SERVER_PORT + F("."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize REST api."));
	initializeRestApi();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("REST api initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Load LEDs and animators from configuration using the LED Manager."));
	if (ledManager->reloadAnimations())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators loaded."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to load LEDs and animators. Continue without rendering LEDs."));
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Creating to WiFi network."));
	if (createtWiFiNetwork())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi Network created."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize timers."));
	initializeTimers();
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Timers initialized."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize watchdog."));
	if (TL::WatchDog::initializeTaskWatchdog())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Watchdog initialized."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize watchdog. Trying to reboot."));
		TL::Updater::reboot("Watchdog failed to initialize.", 0);
		while (true)
		{
		}
	}

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("TesLight initialized successfully, going into work mode."));
}

/**
 * @brief Function should be called continuously from the main loop.
 */
void TesLight::run()
{
	bool skipFrame = false;

	// Handle the LEDs
	if (checkTimer(ledTimer, ledInterval, skipFrame))
	{
		ledInterval = ledManager->getTargetFrameTime();
		ledManager->render();
		if (!skipFrame)
		{
			ledManager->show();
			ledFrameCounter++;
			ledPowerCounter += ledManager->getLedPowerDraw();
		}
	}

	// Handle the light sensor
	if (checkTimer(lightSensorTimer, lightSensorInterval, skipFrame) && !skipFrame)
	{
		lightSensorInterval = LIGHT_SENSOR_INTERVAL;
		float brightness;
		if (lightSensor->getBrightness(brightness, motionSensor.get()))
		{
			ledManager->setAmbientBrightness(brightness);
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read light sensor data. Delaying next read by 10s."));
			lightSensorInterval += 10000000;
		}
	}

	// Handle the motion sensor
	if (checkTimer(motionSensorTimer, motionSensorInterval, skipFrame) && !skipFrame)
	{
		motionSensorInterval = MOTION_SENSOR_INTERVAL;
		if (motionSensor->run())
		{
			ledManager->setMotionSensorData(motionSensor->getMotion());
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read motion sensor data. Delaying next read by 10s"));
			motionSensorInterval += 10000000;
		}
	}

	// Handle web server requests
	if (checkTimer(webServerTimer, webServerInterval, skipFrame) && !skipFrame)
	{
		webServerInterval = WEB_SERVER_INTERVAL;
		if (!TL::WatchDog::deleteTaskWatchdog())
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to delete task watchdog before handling webserver."));
		}
		webServerManager->handleRequest();
		if (!TL::WatchDog::initializeTaskWatchdog())
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize task watchdog after handling webserver."));
		}
	}

	// Measure the FPS and print status message
	if (checkTimer(statusTimer, statusInterval, skipFrame) && !skipFrame)
	{
		statusInterval = STATUS_INTERVAL;
		const float fps = (float)ledFrameCounter / (STATUS_INTERVAL / 1000000);
		const float powerDraw = ledPowerCounter / ledFrameCounter;
		ledFrameCounter = 0;
		ledPowerCounter = 0.0f;
		float temperature;
		if (!temperatureSensor->getMaxTemperature(temperature))
		{
			temperature = 0.0f;
		}
		uint32_t freeHeap = ESP.getFreeHeap() / 1024;
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, (String)F("LEDs: ") + fps + F("FPS      Average Power: ") + powerDraw + F("W      Regulators: ") + temperature + F("°C      Memory: ") + freeHeap + F("kB"));
	}

	// Handle the temperature measurement and fan controller
	if (checkTimer(temperatureTimer, temperatureInterval, skipFrame) && !skipFrame)
	{
		temperatureInterval = TEMP_INTERVAL;
		float temp;
		if (temperatureSensor->getMaxTemperature(temp))
		{
			ledManager->setRegulatorTemperature(temp);
		}
		else
		{
			TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to read temperature sensor. Delaying next read by 5s"));
			temperatureInterval += 5000000;
		}
		fanController->setTemperature(temp > -75.0f ? temp : configuration->getSystemConfig().fanMaxTemperature);
	}

	// Reset the task watchdog timer
	TL::WatchDog::resetTaskWatchdog();
}

/**
 * @brief Print the TesLight logo and FW version.
 */
void TesLight::printLogo()
{
	Serial.println();
	Serial.println(F("████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗"));
	Serial.println(F("╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝"));
	Serial.println(F("   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║   "));
	Serial.println(F("   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║   "));
	Serial.println(F("   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║   "));
	Serial.println(F("   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   "));
	Serial.println(F("Firmware version 1.0.0"));
	Serial.println();
}

/**
 * @brief Initialize the serial and SD card logger.
 * @return true when initialized successfully
 * @return false when error
 */
bool TesLight::initializeLogger(bool sdLogging)
{
	TL::Logger::setMinLogLevel(TL::Logger::LogLevel::INFO);
	if (!sdLogging)
	{
		return TL::Logger::begin(SERIAL_BAUD_RATE);
	}
	else
	{
		return TL::Logger::begin(SERIAL_BAUD_RATE, &SD, LOG_FILE_NAME);
	}
}

/**
 * @brief Initialize the SD card.
 * @return true when initialized successfully
 * @return false when error
 */
bool TesLight::initializeSdCard()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize MicroSD card."));
	return SD.begin(SD_CS_PIN, SPI, SD_SPI_SPEED, SD_MOUNT_POINT, SD_MAX_FILES, false);
}

/**
 * @brief Initialize the configuration and load it from the SD card or default.
 * @return true when the configuration was loaded
 * @return false when there was an error
 */
bool TesLight::initializeConfiguration()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize runtime configuration from MicroSD card."));
	configuration.reset(new TL::Configuration(&SD, CONFIGURATION_FILE_NAME));
	if (configuration->load() && configuration->save())
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Runtime configuration initialized."));
		return true;
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize runtime configuration from MicroSD card."));
		return false;
	}
}

/**
 * @brief Initialize the I²C bus.
 */
void TesLight::intializeI2C()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize I²C bus."));
	Wire.begin((int)IIC_SDA_PIN, (int)IIC_SCL_PIN);
}

/**
 * @brief Initialize the {@link TL::FanController} to handle the cooling fan.
 */
void TesLight::initializeFanController()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize fan controller."));
	fanController.reset(new TL::FanController(FAN_PWM_PIN, configuration.get()));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Fan controller initialized."));
}

/**
 * @brief Initialize the {@link TL::LedManager} to handle the LEDs and animators.
 */
void TesLight::initializeLedManager()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize LED manager."));
	ledManager.reset(new TL::LedManager(configuration.get()));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("LED manager initialized."));
}

/**
 * @brief Initialize the {@link TL::TemperatureSensor} to read all detected sensors on the bus.
 */
void TesLight::initializeTemperatureSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize temperatuer sensor."));
	temperatureSensor.reset(new TL::TemperatureSensor());
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("There are ") + temperatureSensor->getNumSensors() + F(" sensors on the OneWire bus."));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Temperature sensor initialized."));
}

/**
 * @brief Initialize the {@link TL::LightSensor} to read the status of the car's lights.
 */
void TesLight::initializeLightSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize light sensor."));
	lightSensor.reset(new TL::LightSensor(configuration.get()));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Light sensor initialized."));
}

/**
 * @brief Initialize the motion sensor.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::initializeMotionSensor()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, (String)F("Initialize motion sensor at with I²C address") + String(IIC_ADDRESS_MPU6050) + F("."));
	motionSensor.reset(new TL::MotionSensor(IIC_ADDRESS_MPU6050, configuration.get()));
	if (motionSensor->begin())
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Motion sensor initialized."));
		return true;
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to initialize motion sensor."));
		return false;
	}
}

/**
 * @brief Initialize the wifi manager.
 */
void TesLight::initializeWiFiManager()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize WiFi manager."));
	wifiManager.reset(new TL::WiFiManager());
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi manager initialized."));
}

/**
 * @brief Initialize the webserver to serve static files.
 */
void TesLight::initializeWebServerManager()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize webserver."));
	webServerManager.reset(new TL::WebServerManager(&SD));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Webserver initialized."));
}

/**
 * @brief Initialize the REST api.
 */
void TesLight::initializeRestApi()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Initialize REST API."));
	TL::ConnectionTestEndpoint::init(webServerManager.get(), F("/api/"));
	TL::ConnectionTestEndpoint::begin();
	TL::SystemConfigurationEndpoint::init(webServerManager.get(), F("/api/"));
	TL::SystemConfigurationEndpoint::begin(configuration.get(), applySystemConfig);
	TL::LedConfigurationEndpoint::init(webServerManager.get(), F("/api/"));
	TL::LedConfigurationEndpoint::begin(configuration.get(), applyLedConfig);
	TL::WiFiConfigurationEndpoint::init(webServerManager.get(), F("/api/"));
	TL::WiFiConfigurationEndpoint::begin(configuration.get(), applyWifiConfig);
	TL::FseqEndpoint::init(webServerManager.get(), F("/api/"));
	TL::FseqEndpoint::begin(&SD, configuration.get());
	TL::LogEndpoint::init(webServerManager.get(), F("/api/"));
	TL::LogEndpoint::begin(&SD);
	TL::UpdateEndpoint::init(webServerManager.get(), F("/api/"));
	TL::UpdateEndpoint::begin(&SD);
	TL::ResetEndpoint::init(webServerManager.get(), F("/api/"));
	TL::ResetEndpoint::begin(&SD);
	TL::MotionSensorEndpoint::init(webServerManager.get(), F("/api/"));
	TL::MotionSensorEndpoint::begin(configuration.get(), motionSensor.get());
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("REST API initialized."));

	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Starting web server."));
	webServerManager->begin();
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Web server started."));
}

/**
 * @brief Initialize the timers.
 */
void TesLight::initializeTimers()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Initialize timers."));
	ledTimer = micros();
	lightSensorTimer = micros();
	motionSensorTimer = micros();
	webServerTimer = micros();
	statusTimer = micros();
	temperatureTimer = micros();
	ledFrameCounter = 0;
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Timers initialized."));
}

/**
 * @brief Check if a timer expired.
 * @param timer timer value
 * @param cycleTime cycle time of the timer
 * @param skipFrame is set to true when frames should be skipped because the controller cant keep up
 * @return true when the timer expired
 * @return false when the timer is not expired yet
 */
bool TesLight::checkTimer(unsigned long &timer, unsigned long cycleTime, bool &skipFrame)
{
	skipFrame = false;
	unsigned long mic = micros();
	if (mic - timer > cycleTime)
	{
		timer += cycleTime;
		if (mic - timer > cycleTime * 2)
		{
			skipFrame = true;
		}
		return true;
	}
	return false;
}

/**
 * @brief Check if the update file is available.
 * @return true when update file is available
 * @return false when there is no update file
 */
bool TesLight::updateAvilable()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Check if update package file is available."));
	if (TL::FileUtil::fileExists(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Update package file was found."));
		return true;
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("No update package found."));
		return false;
	}
}

/**
 * @brief Install the system update from a TUP file. This function will not return.
 */
void TesLight::handleUpdate()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Install system update from update package."));

	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Set fan speed to maximum during the update."));
	pinMode(FAN_PWM_PIN, OUTPUT);
	digitalWrite(FAN_PWM_PIN, HIGH);

	if (TL::Updater::install(&SD, (String)UPDATE_DIRECTORY + F("/") + UPDATE_FILE_NAME))
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("Update installed successfully. Rebooting. Good luck ;) !"));
		TL::Updater::reboot(F("Update Success"), 0);
		while (true)
		{
		}
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to install system update! Trying to continue."));
	}
}

/**
 * @brief Create a WiFi access point and connect to the wifi network.
 * @return true when successful
 * @return false when there was an error
 */
bool TesLight::createtWiFiNetwork()
{
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Creating WiFi network."));
	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("Starting WiFi access point."));
	if (wifiManager->startAccessPoint(configuration->getWiFiConfig().accessPointSsid.c_str(), configuration->getWiFiConfig().accessPointPassword.c_str(), configuration->getWiFiConfig().accessPointChannel, false, configuration->getWiFiConfig().accessPointMaxConnections))
	{
		TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi AP started and running."));
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to start WiFi access point. Please check your configuration."));
		return false;
	}

	TL::Logger::log(TL::Logger::LogLevel::DEBUG, SOURCE_LOCATION, F("WiFi network created."));
	return true;
}

/**
 * @brief Callback function is called via an REST endpoint when the system configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool TesLight::applySystemConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration has changed. Updating system configuration."));
	const TL::Configuration::SystemConfig systemConfig = configuration->getSystemConfig();
	TL::Logger::setMinLogLevel((TL::Logger::LogLevel)systemConfig.logLevel);
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("System configuration updated."));
	initializeTimers();
	return true;
}

/**
 * @brief Callback function is called via an REST endpoint when the LED configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool TesLight::applyLedConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LED configuration has changed. Reload LEDs and animators using the LED Manager."));
	if (ledManager->reloadAnimations())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("LEDs and animators reloaded."));
		initializeTimers();
		return true;
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to reload LEDs and animators. Continue without rendering LEDs."));
		initializeTimers();
		return false;
	}
}

/**
 * @brief Callback function is called via an REST endpoint when the WiFi configuration was updated.
 * @return true when the configuration was applied successfully
 * @return false when there was an error applying the configuration
 */
bool TesLight::applyWifiConfig()
{
	TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration has changed. Updating WiFi configuration."));
	if (createtWiFiNetwork())
	{
		TL::Logger::log(TL::Logger::LogLevel::INFO, SOURCE_LOCATION, F("WiFi configuration updated."));
		initializeTimers();
		return true;
	}
	else
	{
		TL::Logger::log(TL::Logger::LogLevel::ERROR, SOURCE_LOCATION, F("Failed to create WiFi network. Continuing without WiFi network. The REST API might be inaccessible."));
		initializeTimers();
		return false;
	}
}
