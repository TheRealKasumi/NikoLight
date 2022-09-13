/**
 * @file MainController.js
 * @author TheRealKasumi
 * @brief Main controller for the TesLight UI.
 *
 * @copyright Copyright (c) 2022
 *
 */
class MainController {
	/**
	 * Create a new instance of {@link MainController} and run the controller.
	 */
	constructor() {
		// Init
		this.init();
	}

	/**
	 * Initialize the {@link MainController}.
	 */
	init = async () => {
		// Display background effect
		this.loadParticles();

		// Find UI elements
		this.uiElements = this.getUiComponents();

		// Attach the events to the UI elements
		this.attachUiEvents();

		// Create the LED and WiFi service
		this.systemService = new SystemService("/api/");
		this.ledService = new LedService("/api/");
		this.wifiService = new WiFiService("/api/");

		// Get the system configuration from the TesLigth controller
		let systemConfig;
		try {
			systemConfig = await this.systemService.getSystemConfiguration();
		} catch (ex) {
			this.showMessageBox("Verbindung zum TesLight Controller fehlgeschlagen.", "error");
			this.showUiElement(this.uiElements.loader, false);
			console.log(ex.toString());
			return;
		}

		// Get the LED configuration from the TesLigth controller
		let ledConfig;
		try {
			ledConfig = await this.ledService.getLedConfiguration();
		} catch (ex) {
			this.showMessageBox("Verbindung zum TesLight Controller fehlgeschlagen.", "error");
			this.showUiElement(this.uiElements.loader, false);
			console.log(ex.toString());
			return;
		}

		// Get the WiFi configuration from the TesLigth controller
		let wifiConfig;
		try {
			wifiConfig = await this.wifiService.getWiFiConfiguration();
		} catch (ex) {
			this.showMessageBox("Verbindung zum TesLight Controller fehlgeschlagen.", "error");
			this.showUiElement(this.uiElements.loader, false);
			console.log(ex.toString());
			return;
		}

		// Create the frontend model
		this.model = {
			systemConfig: systemConfig,
			ledConfig: ledConfig,
			wifiConfig: wifiConfig,
			getSystemConfig: () => {
				return this.model.systemConfig;
			},
			getLedConfig: (selectedZone) => {
				return typeof selectedZone === "number" ? this.model.ledConfig[selectedZone] : this.model.ledConfig;
			},
			getWiFiConfig: () => {
				return this.model.wifiConfig;
			},
		};

		// Create the {@link LedController} and {@link SettingsController}
		new LedController(this);
		new SettingsController(this);

		// Initially show the led-view
		this.showUiElement(this.uiElements.loader, false);
		this.showUiElement(this.uiElements.tabBar, true);
		this.showView("led-view");
	};

	/**
	 * Load the particle system.
	 */
	loadParticles = () => {
		window.particlesJS("background", particleConfiguration);
	};

	/**
	 * Find all UI elements requied by the controller.
	 * @returns object containing the ui elements
	 */
	getUiComponents = () => {
		return {
			messageBox: document.getElementById("message-box"),
			messageBoxText: document.getElementById("message-box-text"),
			loader: document.getElementById("loader"),
			ledView: document.getElementById("led-view"),
			settingsView: document.getElementById("settings-view"),
			tabBar: document.getElementById("tab-bar"),
			ledTabButton: document.getElementById("led-tab-button"),
			settingsTabButton: document.getElementById("settings-tab-button"),
		};
	};

	/**
	 * Attach events to the UI elements.
	 */
	attachUiEvents = () => {
		this.uiElements.ledTabButton.addEventListener("click", this.onLedTabButtonClick);
		this.uiElements.settingsTabButton.addEventListener("click", this.onSettingsTabButtonClick);
	};

	/**
	 * Show or hide an UI element.
	 */
	showUiElement = (element, visible) => {
		if (visible) {
			element.classList.remove("hidden");
		} else {
			element.classList.add("hidden");
		}
	};

	/**
	 * Show a specific view by it's name.
	 * @param {string} activeView {@code "led-view"} or {@code "settings-view"}
	 */
	showView = (activeView) => {
		switch (activeView) {
			case "led-view":
				this.showUiElement(this.uiElements.ledView, true);
				this.showUiElement(this.uiElements.settingsView, false);
				this.uiElements.ledTabButton.classList.add("active");
				this.uiElements.settingsTabButton.classList.remove("active");
				break;
			case "settings-view":
				this.showUiElement(this.uiElements.ledView, false);
				this.showUiElement(this.uiElements.settingsView, true);
				this.uiElements.ledTabButton.classList.remove("active");
				this.uiElements.settingsTabButton.classList.add("active");
				break;
		}
	};

	/**
	 * Show a message at the top of the app.
	 * @param {string} message message text
	 * @param {string} type type of the box {@code success}, {@code warning} or {@code error}
	 * @param {number} duration duration for which the message is shown
	 */
	showMessageBox = (message, type, duration) => {
		this.uiElements.messageBoxText.innerHTML = message;
		this.uiElements.messageBox.classList.add(type);
		this.uiElements.messageBox.classList.remove("hidden");

		if (duration) {
			setTimeout(() => {
				this.uiElements.messageBox.classList.remove(type);
				this.uiElements.messageBox.classList.add("hidden");
			}, duration);
		}
	};

	/**
	 * Get the {@link SystemService}.
	 * @returns {@link SystemService} instance
	 */
	getSystemService = () => {
		return this.systemService;
	};

	/**
	 * Get the {@link LedService}.
	 * @returns {@link LedService} instance
	 */
	getLedService = () => {
		return this.ledService;
	};

	/**
	 * Get the {@link WiFiService}.
	 * @returns {@link WiFiService} instance
	 */
	getWiFiService = () => {
		return this.wifiService;
	};

	/**
	 * Get the local application model
	 * @returns local application model
	 */
	getModel = () => {
		return this.model;
	};

	/**
	 * Is called when the led tab button is clicked.
	 * @param event event context
	 */
	onLedTabButtonClick = (event) => {
		this.showView("led-view");
	};

	/**
	 * Is called when the settings tab button is clicked.
	 * @param event event context
	 */
	onSettingsTabButtonClick = (event) => {
		this.showView("settings-view");
	};
}

/**
 * Create a new {@link MainController} instance when the page is loaded.
 */
window.onload = function () {
	new MainController();
};
