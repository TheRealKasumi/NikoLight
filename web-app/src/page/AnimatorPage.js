import React from "react";
import Button from "../component/Button";
import ColorPicker from "../component/ColorPicker";
import DropDown from "../component/DropDown";
import Slider from "../component/Slider";
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
	 * Apply the settings and send them to the TesLight controller.
	 */
	applySettings = () => {
		const state = this.state;
		state.ledConfiguration[state.selectedZone].copyFrom(state.ledConfigurationCopy);
		this.setState(state);

		if (state.ledConfiguration[state.selectedZone].hasChanged()) {
			const result = state.ledService.postLedConfiguration(state.ledConfiguration);
			result.then(() => {
				state.ledConfiguration[state.selectedZone].hasChanged(true);
				state.ledConfigurationCopy.hasChanged(true);
			});
			result.catch((error) => {
				// Todo properly handle this with info box
				alert("Failed to update LED animation");
			});
		}
	};

	/**
	 * Cancel and go back to the zone selection.
	 */
	cancel = () => {
		const state = this.state;
		state.ledConfigurationCopy.copyFrom(state.ledConfiguration[state.selectedZone]);
		this.setState(state);
		this.state.setPage(0, {});
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

				<DropDown
					title="Animation"
					value={this.state.ledConfigurationCopy.getType()}
					options={[
						{ value: "0", name: "Rainbow" },
						{ value: "1", name: "Rainbow Linear" },
						{ value: "2", name: "Rainbow Centered" },
						{ value: "3", name: "Gradient" },
						{ value: "4", name: "Static" },
					]}
					onChange={this.setAnimatorType}
				/>
				<div className="spacer"></div>

				<Slider
					title="Brightness"
					min={0}
					max={255}
					value={this.state.ledConfigurationCopy.getBrightness()}
					step={5}
					icon="../../img/icon/brightness.svg"
					onChange={this.setAnimatorBrightness}
				/>
				<div className="spacer"></div>

				{this.state.ledConfigurationCopy.getType() <= 2 ? (
					<Slider
						title="Speed"
						min={0}
						max={255}
						value={this.state.ledConfigurationCopy.getSpeed()}
						step={5}
						icon="../../img/icon/speed.svg"
						onChange={this.setAnimationSpeed}
					/>
				) : null}
				<div className="spacer"></div>

				{this.state.ledConfigurationCopy.getType() <= 2 ? (
					<>
						<Slider
							title="Offset"
							min={0}
							max={255}
							value={this.state.ledConfigurationCopy.getOffset()}
							step={5}
							icon="../../img/icon/offset.svg"
							onChange={this.setAnimationOffset}
						/>
						<div className="spacer"></div>
					</>
				) : null}

				{this.state.ledConfigurationCopy.getType() === 3 || this.state.ledConfigurationCopy.getType() === 4 ? (
					<>
						<ColorPicker title="Color 1" value={this.getAnimationColor1()} onChange={this.setAnimationColor1} />
						<div className="spacer"></div>
					</>
				) : null}

				{this.state.ledConfigurationCopy.getType() === 3 ? (
					<>
						<ColorPicker title="Color 2" value={this.getAnimationColor2()} onChange={this.setAnimationColor2} />
						<div className="spacer"></div>
					</>
				) : null}

				<Slider
					title="Fading"
					min={0}
					max={255}
					value={this.state.ledConfigurationCopy.getFadeSpeed()}
					step={5}
					icon="../../img/icon/fading.svg"
					onChange={this.setAnimationFadingSpeed}
				/>
				<div className="spacer"></div>

				{this.state.ledConfigurationCopy.getType() <= 3 ? (
					<>
						<ToggleSwitch
							title="Reverse"
							leftText="Normal"
							rightText="Reverse"
							value={this.state.ledConfigurationCopy.getReverse()}
							onChange={this.setAnimationReverse}
						/>
						<div className="spacer"></div>
					</>
				) : null}

				<Button title="Apply" onClick={this.applySettings} />
				<div className="spacer"></div>

				<Button title="Back to Zones" onClick={this.cancel} />
				<div className="spacer"></div>
			</div>
		);
	}
}

export default AnimatorPage;
