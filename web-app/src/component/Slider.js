import React from "react";
import "../App.css";

/**
 * Slider component to select a value between a minimum and a maximum.
 */
class Slider extends React.Component {
	/**
	 * Create a new instance of the {@Slider} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			min: props.min,
			max: props.max,
			value: props.value,
			step: props.step,
			icon: props.icon,
			onChange: props.onChange,
		};
	}

	/**
	 * Update the value.
	 * @param {object} event event from the UI element
	 */
	onChange = (event) => {
		const state = this.state;
		state.value = parseInt(event.target.value);
		state.onChange(state.value);
		this.setState(state);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<>
				<label className="title">{this.state.title}</label>
				<div className="slider">
					<div>
						<div style={{ width: "15%", textAlign: "center" }}>
							<img src={this.state.icon} height="20px" />
						</div>
						<div style={{ flex: "1" }}>
							<input
								type="range"
								min={this.state.min}
								max={this.state.max}
								value={this.state.value}
								onChange={this.onChange}
								step={this.state.step}
							/>
						</div>
						<div style={{ width: "15%", textAlign: "center" }}>
							<p>{this.state.value}</p>
						</div>
					</div>
				</div>
			</>
		);
	}
}

export default Slider;
