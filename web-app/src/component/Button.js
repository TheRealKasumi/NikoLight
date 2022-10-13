import React from "react";
import "../App.css";

/**
 * Component containing a simple button.
 */
class Button extends React.Component {
	/**
	 * Create a new instance of the {Button} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			onClick: props.onClick,
		};
	}

	/**
	 * Callback for the click event.
	 * @param {object} event click event
	 */
	onClick = (event) => {
		if (this.state.onClick) {
			this.state.onClick(event);
		}
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return <input type="button" value={this.state.title} onClick={this.onClick} className="button" />;
	}
}

export default Button;
