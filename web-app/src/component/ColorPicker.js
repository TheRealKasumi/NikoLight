import React from "react";

/**
 * Color picker component to select a color.
 */
class ColorPicker extends React.Component {
	/**
	 * Create a new instance of the {@ColorPicker} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			value: props.value,
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
		state.value = event.target.value;
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
							<input type="color" value={this.state.value} onInput={this.onChange} />
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

export default ColorPicker;
