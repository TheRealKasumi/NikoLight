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
			className: props.className,
			successTitle: props.successTitle,
			errorTitle: props.errorTitle,
			successClassName: props.successClassName,
			errorClassName: props.errorClassName,
			onClick: props.onClick,
			result: null,
		};
	}

	/**
	 * Callback for the click event.
	 * @param {object} event click event
	 */
	onClick = (event) => {
		if (this.state.onClick) {
			this.state.onClick(event, this.onResult);
		}
	};

	/**
	 * Should be called from the onClick handler to give feedback about the result.
	 * @param {boolean} result true or false
	 */
	onResult = (result) => {
		const state = this.state;
		state.result = result;
		this.setState(state);

		setTimeout(() => {
			const state = this.state;
			state.result = null;
			this.setState(state);
		}, 2000);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<input
				type="button"
				value={
					this.state.result === true
						? this.state.successTitle
						: this.state.result == false
						? this.state.errorTitle
						: this.state.title
				}
				onClick={this.onClick}
				className={
					this.state.result === true
						? this.state.successClassName
						: this.state.result == false
						? this.state.errorClassName
						: this.state.className
				}
			/>
		);
	}
}

export default Button;
