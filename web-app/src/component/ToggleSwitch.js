import React from "react";

/**
 * Component contains a toggle switch to select a boolean value.
 */
class ToggleSwitch extends React.Component {
	/**
	 * Create a new instance of the {ToggleSwitch} component
	 * @param {*} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			leftText: props.leftText,
			rightText: props.rightText,
			active: props.active,
			onChange: props.onChange,
		};
	}

	/**
	 * Update the value.
	 * @param {object} event event from the UI element
	 */
	onChange = (event) => {
		const state = this.state;
		state.active = event.target.checked;
		state.onChange(state.active);
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
				<div className="switch">
					<label className="toggle">
						<div style={{ width: "35%", textAlign: "right" }}>
							<span>{this.state.leftText}</span>
						</div>
						<div style={{ flex: "1" }}>
							<input className="toggle-checkbox" type="checkbox" onChange={this.onChange} />
							<div className="toggle-switch"></div>
						</div>
						<div style={{ width: "35%", textAlign: "left" }}>
							<span>{this.state.rightText}</span>
						</div>
					</label>
				</div>
			</>
		);
	}
}

export default ToggleSwitch;
