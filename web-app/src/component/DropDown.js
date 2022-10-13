import React from "react";
import "../App.css";

/**
 * Component contains a drop down selection.
 */
class DropDown extends React.Component {
	/**
	 * Create a new instance of {DropDown}.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			value: props.value,
			options: props.options,
			onChange: props.onChange,
		};
	}

	/**
	 * Change the selected value and the selection index.
	 * @param {object} event event from the UI element
	 */
	onChange = (event) => {
		const state = this.state;
		state.value = event.target.value;
		state.onChange(state.value, parseInt(event.target.selectedIndex));
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
				<select value={this.state.value} onChange={this.onChange}>
					{this.state.options.map(({ value, name }, index) => (
						<option key={index} value={value}>
							{name}
						</option>
					))}
				</select>
			</>
		);
	}
}

export default DropDown;
