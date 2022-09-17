/**
 * @file SettingsController.js
 * @author TheRealKasumi
 * @brief Controller for the settings tab of the TesLight UI.
 *
 * @copyright Copyright (c) 2022
 *
 */
class SettingsController {
	/**
	 * Create a new instance of {@link SettingsController} and run the controller.
	 */
	constructor(mainController) {
		// Set the reference to the main controller
		this.mainController = mainController;

		// Find UI elements
		this.uiElements = this.getUiComponents();

		// Attach the events to the UI elements
		this.attachUiEvents();

		// Display the current system configuration in the UI
		this.displaySystemConfiguration();

		// Display the current LED configuration in the UI
		this.displayLedConfiguration();

		// Display the current WiFi configuration in the UI
		this.displayWiFiConfiguration();

		// Set the model as unchanged
		this.mainController.getModel().getSystemConfig().changed = false;
	}

	/**
	 * Find all UI elements requied by the controller.
	 * @returns object containing the ui elements
	 */
	getUiComponents = () => {
		return {
			loggingLevelSelect: document.getElementById("loging-level-select"),
			lightSensorModeSelect: document.getElementById("light-sensor-mode-select"),
			lightSensorThresholdInput: document.getElementById("light-sensor-threshold"),
			lightSensorMinValueInput: document.getElementById("light-sensor-min"),
			lightSensorMaxValueInput: document.getElementById("light-sensor-max"),
			zoneSelect: document.getElementById("zone-select-settings"),
			hardwarePinSelect: document.getElementById("hardware-pin-select"),
			ledCountInput: document.getElementById("led-count-input"),
			apSsidInput: document.getElementById("ap-ssid-input"),
			apPasswordInput: document.getElementById("ap-password-input"),
			applyButton: document.getElementById("apply-settings-button"),
		};
	};

	/**
	 * Attach events to the UI elements.
	 */
	attachUiEvents = () => {
		// Todo
		this.uiElements.loggingLevelSelect.addEventListener("input", this.onLoggingLevelChanged);
		this.uiElements.lightSensorModeSelect.addEventListener("input", this.onLightSensorModeChanged);
		this.uiElements.lightSensorThresholdInput.addEventListener("input", this.onLightSensorThresholdChanged);
		this.uiElements.lightSensorMinValueInput.addEventListener("input", this.onLightSensorMinValueChanged);
		this.uiElements.lightSensorMaxValueInput.addEventListener("input", this.onLightSensorMaxValueChanged);
		this.uiElements.zoneSelect.addEventListener("input", this.onZoneChanged);
		this.uiElements.hardwarePinSelect.addEventListener("input", this.onHardwarePinChanged);
		this.uiElements.ledCountInput.addEventListener("input", this.onLedCountChanged);
		this.uiElements.apSsidInput.addEventListener("input", this.onSsidChanged);
		this.uiElements.apPasswordInput.addEventListener("input", this.onPasswordChanged);
		this.uiElements.applyButton.addEventListener("click", this.onApplySettings);
	};

	/**
	 * Display the current system configuration in the UI.
	 */
	displaySystemConfiguration = () => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		this.uiElements.loggingLevelSelect.selectedIndex = systemConfig.logLevel;
		this.uiElements.lightSensorModeSelect.selectedIndex = systemConfig.lightSensorMode;
		this.uiElements.lightSensorThresholdInput.value = systemConfig.lightSensorThreshold;
		this.uiElements.lightSensorMinValueInput.value = systemConfig.lightSensorMinValue;
		this.uiElements.lightSensorMaxValueInput.value = systemConfig.lightSensorMaxValue;
	};

	/**
	 * Display the current LED configuration in the UI.
	 */
	displayLedConfiguration = () => {
		const ledConfig = this.mainController.getModel().getLedConfig(this.uiElements.zoneSelect.selectedIndex);
		for (let i = 0; i < this.uiElements.hardwarePinSelect.length; i++) {
			if (this.uiElements.hardwarePinSelect[i].value == ledConfig.outputPin) {
				this.uiElements.hardwarePinSelect.selectedIndex = i;
				break;
			}
		}
		this.uiElements.ledCountInput.value = ledConfig.ledCount;
	};

	/**
	 * Display the current WiFi configuration in the UI.
	 */
	displayWiFiConfiguration = () => {
		const wifiConfig = this.mainController.getModel().getWiFiConfig();
		this.uiElements.apSsidInput.value = wifiConfig.accessPointSsid;
		this.uiElements.apPasswordInput.value = wifiConfig.accessPointPassword;
	};

	/**
	 * Is called when the logging level was changed.
	 * @param event event context
	 */
	onLoggingLevelChanged = (event) => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		systemConfig.logLevel = this.uiElements.loggingLevelSelect.selectedIndex;
		systemConfig.changed = true;
	};

	/**
	 * Is called when the light sensor mode was changed.
	 * @param event event context
	 */
	onLightSensorModeChanged = (event) => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		systemConfig.lightSensorMode = this.uiElements.lightSensorModeSelect.selectedIndex;
		systemConfig.changed = true;
	};

	/**
	 * Is called when the light sensor threshold was changed.
	 * @param event event context
	 */
	onLightSensorThresholdChanged = (event) => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		systemConfig.lightSensorThreshold = parseInt(this.uiElements.lightSensorThresholdInput.value);
		systemConfig.changed = true;
	};

	/**
	 * Is called when the light sensor min value was changed.
	 * @param event event context
	 */
	onLightSensorMinValueChanged = (event) => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		systemConfig.lightSensorMinValue = parseInt(this.uiElements.lightSensorMinValueInput.value);
		systemConfig.changed = true;
	};

	/**
	 * Is called when the light sensor max value was changed.
	 * @param event event context
	 */
	onLightSensorMaxValueChanged = (event) => {
		const systemConfig = this.mainController.getModel().getSystemConfig();
		systemConfig.lightSensorMaxValue = parseInt(this.uiElements.lightSensorMaxValueInput.value);
		systemConfig.changed = true;
	};

	/**
	 * Is called when the LED zone was changed.
	 * @param event event context
	 */
	onZoneChanged = (event) => {
		this.displayLedConfiguration();
	};

	/**
	 * Is called when the hardware pin for a zone was changed.
	 * @param event event context
	 */
	onHardwarePinChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].outputPin = parseInt(this.uiElements.hardwarePinSelect.value);
		ledConfig.changed = true;
	};

	/**
	 * Is called when the LED count for a zone was changed.
	 * @param event event context
	 */
	onLedCountChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].ledCount = parseInt(this.uiElements.ledCountInput.value);
		ledConfig.changed = true;
	};

	/**
	 * Is called when the access point sssid was changed.
	 * @param event event context
	 */
	onSsidChanged = (event) => {
		const wifiConfig = this.mainController.getModel().getWiFiConfig();
		wifiConfig.accessPointSsid = event.target.value;
		wifiConfig.changed = true;
	};

	/**
	 * Is called when the access point password was changed.
	 * @param event event context
	 */
	onPasswordChanged = (event) => {
		const wifiConfig = this.mainController.getModel().getWiFiConfig();
		wifiConfig.accessPointSsid = event.target.value;
		wifiConfig.changed = true;
	};

	/**
	 * Is called when the apply button is clicked.
	 * @param event event context
	 */
	onApplySettings = async (event) => {
		try {
			const systemConfig = this.mainController.getModel().getSystemConfig();
			const ledConfig = this.mainController.getModel().getLedConfig();
			const wifiConfig = this.mainController.getModel().getWiFiConfig();

			if (systemConfig.changed === true) {
				await this.mainController.getSystemService().postSystemConfiguration(systemConfig);
			}
			systemConfig.changed = false;

			if (ledConfig.changed === true) {
				await this.mainController.getLedService().postLedConfiguration(ledConfig);
			}
			ledConfig.changed = false;

			if (wifiConfig.changed === true) {
				await this.mainController.getWiFiService().postWiFiConfiguration(wifiConfig);
			}
			wifiConfig.changed = false;

			this.mainController.showMessageBox(
				"Einstellungen aktualisiert. Es ist eventuell notwendig, die Verbindung zum Controller erneut herzustellen.",
				"success",
				5000
			);
		} catch (ex) {
			this.mainController.showMessageBox("Verbindung zum TesLight Controller fehlgeschlagen.", "error", 5000);
			console.log(ex.toString());
			return;
		}
	};
}
