/**
 * @file LedController.js
 * @author TheRealKasumi
 * @brief Controller for the LED tab of the TesLight UI.
 *
 * @copyright Copyright (c) 2022
 *
 */
class LedController {
	/**
	 * Create a new instance of {@link LedController} and run the controller.
	 */
	constructor(mainController) {
		// Set the reference to the main controller
		this.mainController = mainController;

		// Find UI elements
		this.uiElements = this.getUiComponents();

		// Attach the events to the UI elements
		this.attachUiEvents();

		// Display the current LED configuration of the selected zone in the UI
		this.displayLedConfiguration();

		// Set the model as unchanged
		this.mainController.getModel().getLedConfig().changed = false;
	}

	/**
	 * Find all UI elements requied by the controller.
	 * @returns object containing the ui elements
	 */
	getUiComponents = () => {
		const elements = {
			zoneSelect: document.getElementById("zone-select"),
			animatorTypeSelect: document.getElementById("animator-type-select"),
			brightnessSlider: document.getElementById("brightness-slider"),
			colorSlider1: document.getElementById("color-slider-1"),
			satturationSlider1: document.getElementById("satturation-slider-1"),
			colorSlider2: document.getElementById("color-slider-2"),
			satturationSlider2: document.getElementById("satturation-slider-2"),
			speedSlider: document.getElementById("speed-slider"),
			distanceSlider: document.getElementById("distance-slider"),
			fadingSlider: document.getElementById("fading-slider"),
			reverseCheck: document.getElementById("reverse-check"),
			applyButton: document.getElementById("apply-led-button"),
		};

		elements.brightnessSliderInput = elements.brightnessSlider.getElementsByTagName("input")[0];
		elements.colorSliderInput1 = elements.colorSlider1.getElementsByTagName("input")[0];
		elements.satturationSliderInput1 = elements.satturationSlider1.getElementsByTagName("input")[0];
		elements.colorSliderInput2 = elements.colorSlider2.getElementsByTagName("input")[0];
		elements.satturationSliderInput2 = elements.satturationSlider2.getElementsByTagName("input")[0];
		elements.speedSliderInput = elements.speedSlider.getElementsByTagName("input")[0];
		elements.distanceSliderInput = elements.distanceSlider.getElementsByTagName("input")[0];
		elements.fadingSliderInput = elements.fadingSlider.getElementsByTagName("input")[0];
		elements.reverseCheckInput = elements.reverseCheck.getElementsByTagName("input")[0];

		return elements;
	};

	/**
	 * Attach events to the UI elements.
	 */
	attachUiEvents = () => {
		this.uiElements.zoneSelect.addEventListener("input", this.onZoneChanged);
		this.uiElements.animatorTypeSelect.addEventListener("input", this.onAnimatorChanged);
		this.uiElements.brightnessSliderInput.addEventListener("input", this.onBrightnessSliderChanged);
		this.uiElements.colorSliderInput1.addEventListener("input", this.onColorSliderChanged1);
		this.uiElements.satturationSliderInput1.addEventListener("input", this.onSatturationSliderChanged1);
		this.uiElements.colorSliderInput2.addEventListener("input", this.onColorSliderChanged2);
		this.uiElements.satturationSliderInput2.addEventListener("input", this.onSatturationSliderChanged2);
		this.uiElements.speedSliderInput.addEventListener("input", this.onSpeedSliderChanged);
		this.uiElements.distanceSliderInput.addEventListener("input", this.onDistanceSliderChanged);
		this.uiElements.fadingSliderInput.addEventListener("input", this.onFadingSliderChanged);
		this.uiElements.reverseCheckInput.addEventListener("input", this.onReverseCheckChanged);
		this.uiElements.applyButton.addEventListener("click", this.onLedButtonClick);
	};

	/**
	 * Display the LED configuration for the selected zone in the UI.
	 */
	displayLedConfiguration = () => {
		const config = this.mainController.getModel().getLedConfig(this.uiElements.zoneSelect.selectedIndex);
		this.uiElements.animatorTypeSelect.selectedIndex = config.animatorType;
		this.uiElements.brightnessSliderInput.value = config.brightness;
		this.uiElements.colorSliderInput1.value = config.hue1;
		this.uiElements.satturationSliderInput1.value = config.satturation1;
		this.uiElements.colorSliderInput2.value = config.hue2;
		this.uiElements.satturationSliderInput2.value = config.satturation2;
		this.uiElements.speedSliderInput.value = config.speed;
		this.uiElements.distanceSliderInput.value = config.offset;
		this.uiElements.reverseCheckInput.checked = config.reverse;
		this.onAnimatorChanged({ target: this.uiElements.animatorTypeSelect });
	};

	/**
	 * Show the properties based on the animator type.
	 * @param {string} animatorType {@code " Rainbow"}, {@code "Gradient"} or {@code "Static"}
	 */
	showAnimatorProperties = (animatorType) => {
		this.mainController.showUiElement(this.uiElements.brightnessSlider, false);
		this.mainController.showUiElement(this.uiElements.colorSlider1, false);
		this.mainController.showUiElement(this.uiElements.satturationSlider1, false);
		this.mainController.showUiElement(this.uiElements.colorSlider2, false);
		this.mainController.showUiElement(this.uiElements.satturationSlider2, false);
		this.mainController.showUiElement(this.uiElements.speedSlider, false);
		this.mainController.showUiElement(this.uiElements.distanceSlider, false);
		this.mainController.showUiElement(this.uiElements.fadingSlider, false);
		this.mainController.showUiElement(this.uiElements.reverseCheck, false);

		switch (animatorType) {
			case "Rainbow":
				this.mainController.showUiElement(this.uiElements.brightnessSlider, true);
				this.mainController.showUiElement(this.uiElements.speedSlider, true);
				this.mainController.showUiElement(this.uiElements.fadingSlider, true);
				this.mainController.showUiElement(this.uiElements.reverseCheck, true);
				break;
			case "Rainbow Linear":
				this.mainController.showUiElement(this.uiElements.brightnessSlider, true);
				this.mainController.showUiElement(this.uiElements.speedSlider, true);
				this.mainController.showUiElement(this.uiElements.distanceSlider, true);
				this.mainController.showUiElement(this.uiElements.fadingSlider, true);
				this.mainController.showUiElement(this.uiElements.reverseCheck, true);
				break;
			case "Rainbow Middle":
				this.mainController.showUiElement(this.uiElements.brightnessSlider, true);
				this.mainController.showUiElement(this.uiElements.speedSlider, true);
				this.mainController.showUiElement(this.uiElements.distanceSlider, true);
				this.mainController.showUiElement(this.uiElements.fadingSlider, true);
				this.mainController.showUiElement(this.uiElements.reverseCheck, true);
				break;
			case "Gradient":
				this.mainController.showUiElement(this.uiElements.brightnessSlider, true);
				this.mainController.showUiElement(this.uiElements.colorSlider1, true);
				this.mainController.showUiElement(this.uiElements.satturationSlider1, true);
				this.mainController.showUiElement(this.uiElements.colorSlider2, true);
				this.mainController.showUiElement(this.uiElements.satturationSlider2, true);
				this.mainController.showUiElement(this.uiElements.fadingSlider, true);
				this.mainController.showUiElement(this.uiElements.reverseCheck, true);
				break;
			case "Static":
				this.mainController.showUiElement(this.uiElements.brightnessSlider, true);
				this.mainController.showUiElement(this.uiElements.colorSlider1, true);
				this.mainController.showUiElement(this.uiElements.satturationSlider1, true);
				this.mainController.showUiElement(this.uiElements.fadingSlider, true);
				break;
		}
	};

	/**
	 * Is called when the LED zone was changed.
	 * @param event event context
	 */
	onZoneChanged = (event) => {
		this.displayLedConfiguration();
	};

	/**
	 * Is called when the animator type of the zone was changed.
	 * @param event event context
	 */
	onAnimatorChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].animatorType = event.target.selectedIndex;
		ledConfig.changed = true;
		this.showAnimatorProperties(event.target.value);
	};

	/**
	 * Is called when the brightness slider was changed.
	 * @param event event context
	 */
	onBrightnessSliderChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].brightness = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the first color slider was changed.
	 * @param event event context
	 */
	onColorSliderChanged1 = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].hue1 = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is callaed when the first satturation slider was changed.
	 * @param event event context
	 */
	onSatturationSliderChanged1 = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].satturation1 = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the second color slider was changed.
	 * @param event event context
	 */
	onColorSliderChanged2 = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].hue2 = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the second satturation slider was changed.
	 * @param event event context
	 */
	onSatturationSliderChanged2 = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].satturation2 = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the speed slider was changed.
	 * @param event event context
	 */
	onSpeedSliderChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].speed = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the distance slider was changed.
	 * @param event event context
	 */
	onDistanceSliderChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].offset = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the fading slider was changed.
	 * @param event event context
	 */
	onFadingSliderChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].fadingSpeed = event.target.value;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the reverse checkbox was changed.
	 * @param event event context
	 */
	onReverseCheckChanged = (event) => {
		const ledConfig = this.mainController.getModel().getLedConfig();
		ledConfig[this.uiElements.zoneSelect.selectedIndex].reverse = event.target.checked;
		ledConfig.changed = true;
	};

	/**
	 * Is called when the apply button below the LED settings is clicked.
	 * @param event event context
	 */
	onLedButtonClick = async (event) => {
		try {
			const ledConfig = this.mainController.getModel().ledConfig;
			if (ledConfig.changed === true) {
				await this.mainController.getLedService().postLedConfiguration(ledConfig);
			}
			ledConfig.changed = false;
			this.mainController.showMessageBox("Farbschema aktualisiert.", "success", 5000);
		} catch (ex) {
			this.mainController.showMessageBox("Verbindung zum TesLight Controller fehlgeschlagen.", "error", 5000);
			console.log(ex.toString());
			return;
		}
	};
}
