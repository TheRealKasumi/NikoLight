import React from "react";
import "../App.css";

/**
 * TextInput component to enter a textual value.
 */
class TextInput extends React.Component {
	/**
	 * Create a new instance of the {@TextInput} component.
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
				<div className="text">
					<div>
						<div style={{ width: "15%", textAlign: "center" }}>
							<img src={this.state.icon} height="20px" />
						</div>
						<div style={{ flex: "1" }}>
							<input type="text" value={this.state.value} onChange={this.onChange} />
						</div>
					</div>
				</div>
			</>
		);
	}
}

export default TextInput;
