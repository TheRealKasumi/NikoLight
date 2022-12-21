import React from "react";
import Button from "../component/Button";
import DropDown from "../component/DropDown";
import Slider from "../component/Slider";
import TextInput from "../component/TextInput";
import VoltageSlider from "../component/VoltageSlider";

/**
 * Component containing a setting page for the {LedConfiguration}.
 */
class SettingsPage extends React.Component {
	/**
	 * Create a new instance of the {SettingsPage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			systemService: props.systemService,
			wifiService: props.wifiService,
			systemConfiguration: props.systemConfiguration,
			systemConfigurationCopy: props.systemConfiguration.clone(),
			wifiConfiguration: props.wifiConfiguration,
			wifiConfigurationCopy: props.wifiConfiguration.clone(),
			inputKey: 0,
		};
	}

	/**
	 * Set the light sensor mode.
	 * @param {string} value value of the selection
	 * @param {number} selectedIndex index of the selection
	 */
	setLightSensorMode = (value, selectedIndex) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorMode(parseInt(value));
		this.setState(state);
	};

	/**
	 * Set the light sensor threshold.
	 * @param {string} value value of the selection
	 */
	setLightSensorThreshold = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorThreshold(value);
		this.setState(state);
	};

	/**
	 * Set the minimum ambient brightness for the light sensor.
	 * @param {string} value value of the selection
	 */
	setLightSensorMinAmbientBrightness = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorMinAmbientBrightness(value);
		this.setState(state);
	};

	/**
	 * Set the maximum ambient brightness for the light sensor.
	 * @param {string} value value of the selection
	 */
	setLightSensorMaxAmbientBrightness = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorMaxAmbientBrightness(value);
		this.setState(state);
	};

	/**
	 * Set the minimum LED brightness for automatic adjustment.
	 * @param {string} value value of the selection
	 */
	setLightSensorMinLedBrightness = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorMinLedBrightness(value);
		this.setState(state);
	};

	/**
	 * Set the maximum LED brightness for automatic adjustment.
	 * @param {string} value value of the selection
	 */
	setLightSensorMaxLedBrightness = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorMaxLedBrightness(value);
		this.setState(state);
	};

	/**
	 * Set the light sensor duration.
	 * @param {string} value value of the selection
	 */
	setLightSensorDuration = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setLightSensorDuration(value);
		this.setState(state);
	};

	/**
	 * Set the access point ssid.
	 * @param {string} value value of the selection
	 */
	setAccessPointSsid = (value) => {
		const state = this.state;
		state.wifiConfigurationCopy.setAccessPointSsid(value);
		this.setState(state);
	};

	/**
	 * Set the access point password.
	 * @param {string} value value of the selection
	 */
	setAccessPointPassword = (value) => {
		const state = this.state;
		state.wifiConfigurationCopy.setAccessPointPassword(value);
		this.setState(state);
	};

	/**
	 * Set the system power limit in watts.
	 * @param {string} value value of the selection
	 */
	setSystemPowerLimit = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setSystemPowerLimit(value);
		this.setState(state);
	};

	/**
	 *  Set the temp in °C where brightness is reduced.
	 * @param {string} value value of the selection
	 */
	setRegulatorHighTemperature = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setRegulatorHighTemperature(value);
		this.setState(state);
	};

	/**
	 * Set the temp in °C where LEDs are turned off.
	 * @param {string} value value of the selection
	 */
	setRegulatorCutoffTemperature = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setRegulatorCutoffTemperature(value);
		this.setState(state);
	};

	/**
	 * Set the minimum pwm value output to the fan (stall guard).
	 * @param {string} value value of the selection
	 */
	setFanMinPwmValue = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setFanMinPwmValue(value);
		this.setState(state);
	};

	/**
	 * Set the maximum pwm value output to the fan.
	 * @param {string} value value of the selection
	 */
	setFanMaxPwmValue = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setFanMaxPwmValue(value);
		this.setState(state);
	};

	/**
	 * Set the minimum temp in °C where the fan starts.
	 * @param {string} value value of the selection
	 */
	setFanMinTemperature = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setFanMinTemperature(value);
		this.setState(state);
	};

	/**
	 * Set the maximum temp in °C to run at maximum speed.
	 * @param {string} value value of the selection
	 */
	setFanMaxTemperature = (value) => {
		const state = this.state;
		state.systemConfigurationCopy.setFanMaxTemperature(value);
		this.setState(state);
	};

	/**
	 * Set the log level.
	 * @param {string} value value of the selection
	 * @param {number} selectedIndex index of the selection
	 */
	setLogLevel = (value, selectedIndex) => {
		const state = this.state;
		state.systemConfigurationCopy.setLogLevel(parseInt(value));
		this.setState(state);
	};

	/**
	 * Apply the settings and send them to the TesLight controller.
	 */
	applySettings = (event, callback) => {
		const state = this.state;
		state.systemConfiguration.copyFrom(state.systemConfigurationCopy);
		state.wifiConfiguration.copyFrom(state.wifiConfigurationCopy);
		this.setState(state);

		if (state.systemConfiguration.hasChanged()) {
			const result = state.systemService.postSystemConfiguration(state.systemConfiguration);
			result.then(() => {
				state.systemConfiguration.hasChanged(true);
				state.systemConfigurationCopy.hasChanged(true);
			});
			result.catch((error) => {
				callback(false);
			});
		}

		if (state.wifiConfiguration.hasChanged()) {
			const result = state.wifiService.postWifiConfiguration(state.wifiConfiguration);
			result.then(() => {
				state.wifiConfiguration.hasChanged(true);
				state.wifiConfigurationCopy.hasChanged(true);
				callback(true);
			});
			result.catch((error) => {
				callback(false);
			});
		} else {
			callback(true);
		}
	};

	/**
	 * Cancel and reset the values to the previous ones.
	 */
	cancel = () => {
		const state = this.state;
		state.systemConfigurationCopy.copyFrom(state.systemConfiguration);
		state.wifiConfigurationCopy.copyFrom(state.wifiConfiguration);
		state.inputKey += 100;
		this.setState(state);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>System Settings</h2>
				<div className="spacer"></div>

				<details className="details">
					<summary>Light Sensor</summary>
					<div className="spacer"></div>

					<DropDown
						key={`settings-page-input-key-${this.state.inputKey}`}
						title="Mode"
						value={this.state.systemConfigurationCopy.getLightSensorMode().toString()}
						options={[
							{ value: "0", name: "Always Off" },
							{ value: "1", name: "Always On" },
							{ value: "2", name: "Automatic On/Off ADC" },
							{ value: "3", name: "Automatic Brightness ADC" },
							{ value: "4", name: "Automatic On/Off BH1750" },
							{ value: "5", name: "Automatic Brightness BH1750" },
							{ value: "6", name: "Automatic On/Off MPU6050" },
						]}
						onChange={this.setLightSensorMode}
					/>
					<div className="spacer"></div>

					{[2, 3, 4, 5, 6].includes(this.state.systemConfigurationCopy.getLightSensorMode()) ? (
						<>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 1}`}
								title="Threshold"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorThreshold()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/sensor.svg"}
								onChange={this.setLightSensorThreshold}
							/>
							<div className="spacer"></div>
						</>
					) : null}

					{[3, 5].includes(this.state.systemConfigurationCopy.getLightSensorMode()) ? (
						<>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 2}`}
								title="Minimum Brightness"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorMinAmbientBrightness()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/brightness-min.svg"}
								onChange={this.setLightSensorMinAmbientBrightness}
							/>
							<div className="spacer"></div>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 3}`}
								title="Maximum Brightness"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorMaxAmbientBrightness()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/brightness-max.svg"}
								onChange={this.setLightSensorMaxAmbientBrightness}
							/>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 4}`}
								title="Minimum LED Brightness"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorMinLedBrightness()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/brightness-min.svg"}
								onChange={this.setLightSensorMinLedBrightness}
							/>
							<div className="spacer"></div>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 5}`}
								title="Maximum LED Brightness"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorMaxLedBrightness()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/brightness-max.svg"}
								onChange={this.setLightSensorMaxLedBrightness}
							/>
						</>
					) : null}

					{[6].includes(this.state.systemConfigurationCopy.getLightSensorMode()) ? (
						<>
							<Slider
								key={`settings-page-input-key-${this.state.inputKey + 16}`}
								title="Duration"
								min={1}
								max={255}
								value={this.state.systemConfigurationCopy.getLightSensorDuration()}
								step={1}
								icon={process.env.PUBLIC_URL + "/img/icon/brightness-min.svg"}
								onChange={this.setLightSensorDuration}
							/>
							<div className="spacer"></div>
						</>
					) : null}
				</details>

				<div className="spacer"></div>

				<details className="details">
					<summary>WiFi Hotspot</summary>
					<div className="spacer"></div>

					<TextInput
						key={`settings-page-input-key-${this.state.inputKey + 6}`}
						title="SSID"
						value={this.state.wifiConfigurationCopy.getAccessPointSsid()}
						icon={process.env.PUBLIC_URL + "/img/icon/wifi.svg"}
						onChange={this.setAccessPointSsid}
					/>
					<div className="spacer"></div>

					<TextInput
						key={`settings-page-input-key-${this.state.inputKey + 7}`}
						title="Password"
						value={this.state.wifiConfigurationCopy.getAccessPointPassword()}
						icon={process.env.PUBLIC_URL + "/img/icon/lock.svg"}
						onChange={this.setAccessPointPassword}
					/>
				</details>
				<div className="spacer"></div>

				<details className="details">
					<summary>Regulator</summary>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 8}`}
						title="Power Limit (W)"
						min={1}
						max={30}
						value={this.state.systemConfigurationCopy.getSystemPowerLimit()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setSystemPowerLimit}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 9}`}
						title="Throttle Temperature (°C)"
						min={60}
						max={90}
						value={this.state.systemConfigurationCopy.getRegulatorHighTemperature()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setRegulatorHighTemperature}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 10}`}
						title="Shut Down Temperature (°C)"
						min={60}
						max={100}
						value={this.state.systemConfigurationCopy.getRegulatorCutoffTemperature()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setRegulatorCutoffTemperature}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 11}`}
						title="Fan Start Temp (°C)"
						min={40}
						max={70}
						value={this.state.systemConfigurationCopy.getFanMinTemperature()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setFanMinTemperature}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 12}`}
						title="Fan Full Speed Temp (°C)"
						min={50}
						max={90}
						value={this.state.systemConfigurationCopy.getFanMaxTemperature()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setFanMaxTemperature}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 13}`}
						title="Fan Min PWM (Stall Guard)"
						min={0}
						max={255}
						value={this.state.systemConfigurationCopy.getFanMinPwmValue()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setFanMinPwmValue}
					/>
					<div className="spacer"></div>

					<Slider
						key={`settings-page-input-key-${this.state.inputKey + 14}`}
						title="Fan Max PWM"
						min={0}
						max={255}
						value={this.state.systemConfigurationCopy.getFanMaxPwmValue()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/power.svg"}
						onChange={this.setFanMaxPwmValue}
					/>
					<div className="spacer"></div>
				</details>
				<div className="spacer"></div>

				<details className="details">
					<summary>Logging and Debugging</summary>
					<div className="spacer"></div>

					<DropDown
						key={`settings-page-input-key-${this.state.inputKey + 15}`}
						title="Log Level"
						value={this.state.systemConfigurationCopy.getLogLevel()}
						options={[
							{ value: "0", name: "Debug" },
							{ value: "1", name: "Info" },
							{ value: "2", name: "Warning" },
							{ value: "3", name: "Error" },
						]}
						onChange={this.setLogLevel}
					/>
				</details>
				<div className="spacer"></div>

				<Button
					className="button"
					title="Save"
					successTitle="Configuration saved"
					errorTitle="Failed to save"
					successClassName="button success"
					errorClassName="button error"
					onClick={this.applySettings}
				/>
				<div className="spacer"></div>

				<Button className="button" title="Reset" onClick={this.cancel} />
			</div>
		);
	}
}

export default SettingsPage;
