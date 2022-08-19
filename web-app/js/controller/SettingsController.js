/**
 * @file SettingsController.js
 * @author TheRealKasumi
 * @brief Controller for the settings tab of the TesLight UI.
 * @version 0.0.1
 * @date 2022-08-18
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

		// Display the current LED configuration in the UI
		this.displayLedConfiguration();

		// Display the current WiFi configuration in the UI
		this.displayWiFiConfiguration();
	}

	/**
	 * Find all UI elements requied by the controller.
	 * @returns object containing the ui elements
	 */
	getUiComponents = () => {
		return {
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
		this.uiElements.zoneSelect.addEventListener("input", this.onZoneChanged);
		this.uiElements.hardwarePinSelect.addEventListener("input", this.onHardwarePinChanged);
		this.uiElements.ledCountInput.addEventListener("input", this.onLedCountChanged);
		this.uiElements.apSsidInput.addEventListener("input", this.onSsidChanged);
		this.uiElements.apPasswordInput.addEventListener("input", this.onPasswordChanged);
		this.uiElements.applyButton.addEventListener("click", this.onApplySettings);
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
		const ledConfig = this.mainController.getModel().getLedConfig(this.uiElements.zoneSelect.selectedIndex);
		ledConfig.outputPin = parseInt(this.uiElements.hardwarePinSelect.value);
	};

	/**
	 * Is called when the LED count for a zone was changed.
	 * @param event event context
	 */
	onLedCountChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig(this.uiElements.zoneSelect.selectedIndex);
		ledConfig.ledCount = parseInt(this.uiElements.ledCountInput.value);
	};

	/**
	 * Is called when the access point sssid was changed.
	 * @param event event context
	 */
	onSsidChanged = (event) => {
		const wifiConfig = this.mainController.getModel().getWiFiConfig();
		wifiConfig.accessPointSsid = event.target.value;
	};

	/**
	 * Is called when the access point password was changed.
	 * @param event event context
	 */
	onPasswordChanged = (event) => {
		const wifiConfig = this.mainController.getModel().getWiFiConfig();
		wifiConfig.accessPointSsid = event.target.value;
	};

	/**
	 * Is called when the apply button is clicked.
	 * @param event event context
	 */
	onApplySettings = async (event) => {
		try {
			await this.mainController.getLedService().postLedConfiguration(this.mainController.getModel().ledConfig);
			await this.mainController.getWiFiService().postWiFiConfiguration(this.mainController.getModel().wifiConfig);
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
