import React from "react";
import Button from "../component/Button";
import ColorPicker from "../component/ColorPicker";
import DropDown from "../component/DropDown";
import Slider from "../component/Slider";
import VoltageSlider from "../component/VoltageSlider";
import ToggleSwitch from "../component/ToggleSwitch";
import ColorUtil from "../util/ColorUtil";

/**
 * Component containing a setting page for the {LedConfiguration}.
 */
class AnimatorPage extends React.Component {
	/**
	 * Create a new instance of the {AnimatorPage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			ledService: props.ledService,
			selectedZone: props.selectedZone,
			ledConfiguration: props.ledConfiguration,
			ledConfigurationCopy: props.ledConfiguration[props.selectedZone].clone(),
			setPage: props.setPage,
		};
	}

	/**
	 * Set the animator type for the currently selected zone.
	 * @param {string} value value of the selection
	 * @param {number} selectedIndex index of the selection
	 */
	setAnimatorType = (value, selectedIndex) => {
		const state = this.state;
		state.ledConfigurationCopy.setType(parseInt(value));
		this.setState(state);
	};

	/**
	 * Set the animation brightness for the currently selected zone.
	 * @param {number} value brightness value
	 */
	setAnimatorBrightness = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setBrightness(value);
		this.setState(state);
	};

	/**
	 * Set the animation speed for the currently selected zone.
	 * @param {number} value speed value
	 */
	setAnimationSpeed = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setSpeed(value);
		this.setState(state);
	};

	/**
	 * Set the animation offset for the currently selected zone.
	 * @param {number} value offset value
	 */
	setAnimationOffset = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setOffset(value);
		this.setState(state);
	};

	/**
	 * Set the fading speed for the currently selected zone.
	 * @param {number} value fading speed
	 */
	setAnimationFadingSpeed = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setFadeSpeed(value);
		this.setState(state);
	};

	/**
	 * Set if the animation of the currently selected zone is reversed.
	 * @param {boolean} value true for reverse, false for normal
	 */
	setAnimationReverse = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setReverse(value);
		this.setState(state);
	};

	/**
	 * Set the first color for the currently selected zone.
	 * @param {string} value
	 */
	setAnimationColor1 = (value) => {
		const rgb = ColorUtil.hexToRgb(value);
		const state = this.state;
		state.ledConfigurationCopy.getCustomFields()[0] = rgb.r;
		state.ledConfigurationCopy.getCustomFields()[1] = rgb.g;
		state.ledConfigurationCopy.getCustomFields()[2] = rgb.b;
		state.ledConfigurationCopy.setCustomFields(state.ledConfigurationCopy.getCustomFields());
		this.setState(state);
	};

	/**
	 * Get the first color value for the currently selected zone.
	 * @returns hex representation of the color
	 */
	getAnimationColor1 = () => {
		const rgb = {
			r: this.state.ledConfigurationCopy.getCustomFields()[0],
			g: this.state.ledConfigurationCopy.getCustomFields()[1],
			b: this.state.ledConfigurationCopy.getCustomFields()[2],
		};
		return ColorUtil.rgbToHex(rgb);
	};

	/**
	 * Set the second color for the currently selected zone.
	 * @param {string} value
	 */
	setAnimationColor2 = (value) => {
		const rgb = ColorUtil.hexToRgb(value);
		const state = this.state;
		state.ledConfigurationCopy.getCustomFields()[3] = rgb.r;
		state.ledConfigurationCopy.getCustomFields()[4] = rgb.g;
		state.ledConfigurationCopy.getCustomFields()[5] = rgb.b;
		state.ledConfigurationCopy.setCustomFields(state.ledConfigurationCopy.getCustomFields());
		this.setState(state);
	};

	/**
	 * Get the second color value for the currently selected zone.
	 * @returns hex representation of the color
	 */
	getAnimationColor2 = () => {
		const rgb = {
			r: this.state.ledConfigurationCopy.getCustomFields()[3],
			g: this.state.ledConfigurationCopy.getCustomFields()[4],
			b: this.state.ledConfigurationCopy.getCustomFields()[5],
		};
		return ColorUtil.rgbToHex(rgb);
	};

	/**
	 * Set the LED count for the currently selected zone.
	 * @param {number} value
	 */
	setLedCount = (value) => {
		const state = this.state;
		state.ledConfigurationCopy.setLedCount(value);
		this.setState(state);
	};

	/**
	 * Set the LED voltage for the currently selected zone.
	 * @param {number} value
	 */
	setLedVoltage = (value) => {
		const state = this.state;
		console.log(value);
		state.ledConfigurationCopy.setLedVoltage(value);
		this.setState(state);
	};

	/**
	 * Set the channel current for LED of the currently selected zone.
	 * @param {number} value
	 */
	setLedChannelCurrent = (value) => {
		const state = this.state;
		console.log(value);
		state.ledConfigurationCopy.setLedChannelCurrent([value, value, value]);
		this.setState(state);
	};

	/**
	 * Apply the settings and send them to the TesLight controller.
	 */
	applySettings = (event, callback) => {
		const state = this.state;
		state.ledConfiguration[state.selectedZone].copyFrom(state.ledConfigurationCopy);

		if (state.ledConfiguration[state.selectedZone].hasChanged()) {
			const result = state.ledService.postLedConfiguration(state.ledConfiguration);
			result.then(() => {
				state.ledConfiguration[state.selectedZone].hasChanged(true);
				state.ledConfigurationCopy.hasChanged(true);
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
	 * Cancel and go back to the zone selection.
	 */
	cancel = (event, callback) => {
		const state = this.state;
		state.ledConfigurationCopy.copyFrom(state.ledConfiguration[state.selectedZone]);
		state.setPage(0, {});
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>Select Animation</h2>
				<div className="spacer"></div>

				<details className="details" open>
					<summary className="summary">Basic</summary>
					<div className="spacer"></div>

					<DropDown
						title="Animation"
						value={this.state.ledConfigurationCopy.getType().toString()}
						options={[
							{ value: "0", name: "Rainbow" },
							{ value: "1", name: "Rainbow Linear" },
							{ value: "2", name: "Rainbow Centered" },
							{ value: "3", name: "Gradient Linear" },
							{ value: "4", name: "Gradient Center" },
							{ value: "5", name: "Static" },
							{ value: "6", name: "Color Bars Linear Hard" },
							{ value: "7", name: "Color Bars Linear Soft" },
							{ value: "8", name: "Color Bars Center Hard" },
							{ value: "9", name: "Color Bars Center Soft" },
							{ value: "10", name: "Rainbow Linear Acc X" },
							{ value: "11", name: "Rainbow Linear Acc Y" },
							{ value: "12", name: "Rainbow Centered Acc X" },
							{ value: "13", name: "Rainbow Centered Acc Y" },
							{ value: "14", name: "Gradient Linear Acc X" },
							{ value: "15", name: "Gradient Linear Acc Y" },
							{ value: "16", name: "Gradient Centered Acc X" },
							{ value: "17", name: "Gradient Centered Acc Y" },
						]}
						onChange={this.setAnimatorType}
					/>
					<div className="spacer"></div>

					<Slider
						title="Brightness"
						min={0}
						max={255}
						value={this.state.ledConfigurationCopy.getBrightness()}
						step={2}
						icon={process.env.PUBLIC_URL + "/img/icon/brightness.svg"}
						onChange={this.setAnimatorBrightness}
					/>
					<div className="spacer"></div>

					{[0, 1, 2, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17].includes(
						this.state.ledConfigurationCopy.getType()
					) ? (
						<Slider
							title="Speed"
							min={0}
							max={255}
							value={this.state.ledConfigurationCopy.getSpeed()}
							step={5}
							icon={process.env.PUBLIC_URL + "/img/icon/speed.svg"}
							onChange={this.setAnimationSpeed}
						/>
					) : null}
					<div className="spacer"></div>

					{[0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17].includes(
						this.state.ledConfigurationCopy.getType()
					) ? (
						<>
							<Slider
								title="Offset"
								min={0}
								max={255}
								value={this.state.ledConfigurationCopy.getOffset()}
								step={5}
								icon={process.env.PUBLIC_URL + "/img/icon/offset.svg"}
								onChange={this.setAnimationOffset}
							/>
							<div className="spacer"></div>
						</>
					) : null}

					{[3, 4, 5, 6, 7, 8, 9, 14, 15, 16, 17].includes(this.state.ledConfigurationCopy.getType()) ? (
						<>
							<ColorPicker
								title="Color 1"
								value={this.getAnimationColor1()}
								icon={process.env.PUBLIC_URL + "/img/icon/color.svg"}
								onChange={this.setAnimationColor1}
							/>
							<div className="spacer"></div>
						</>
					) : null}

					{[3, 4, 6, 7, 8, 9, 14, 15, 16, 17].includes(this.state.ledConfigurationCopy.getType()) ? (
						<>
							<ColorPicker
								title="Color 2"
								value={this.getAnimationColor2()}
								icon={process.env.PUBLIC_URL + "/img/icon/color.svg"}
								onChange={this.setAnimationColor2}
							/>
							<div className="spacer"></div>
						</>
					) : null}

					<Slider
						title="Fading"
						min={0}
						max={255}
						value={this.state.ledConfigurationCopy.getFadeSpeed()}
						step={5}
						icon={process.env.PUBLIC_URL + "/img/icon/fading.svg"}
						onChange={this.setAnimationFadingSpeed}
					/>
					<div className="spacer"></div>

					{[0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17].includes(
						this.state.ledConfigurationCopy.getType()
					) ? (
						<>
							<ToggleSwitch
								title="Reverse"
								leftText="Normal"
								rightText="Reverse"
								active={this.state.ledConfigurationCopy.getReverse()}
								onChange={this.setAnimationReverse}
							/>
							<div className="spacer"></div>
						</>
					) : null}
				</details>
				<div className="spacer"></div>

				<details className="details">
					<summary>Advanced</summary>
					<div className="spacer"></div>

					<Slider
						title="LED Count"
						min={1}
						max={200}
						value={this.state.ledConfigurationCopy.getLedCount()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/count.svg"}
						onChange={this.setLedCount}
					/>
					<div className="spacer"></div>

					<VoltageSlider
						title="LED Voltage"
						min={40}
						max={55}
						value={this.state.ledConfigurationCopy.getLedVoltage()}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/voltage.svg"}
						onChange={this.setLedVoltage}
					/>
					<div className="spacer"></div>

					<Slider
						title="LED Current Per Channel (mA)"
						min={1}
						max={200}
						value={this.state.ledConfigurationCopy.getLedChannelCurrent()[0]}
						step={1}
						icon={process.env.PUBLIC_URL + "/img/icon/voltage.svg"}
						onChange={this.setLedChannelCurrent}
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

				<Button className="button" title="Back to Zones" onClick={this.cancel} />
				<div className="spacer"></div>
			</div>
		);
	}
}

export default AnimatorPage;
