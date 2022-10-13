import React from "react";
import "../App.css";

/**
 * Component to display a textual log.
 */
class Log extends React.Component {
	/**
	 * Create a new instance of the {@Log} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			text: props.text,
		};
	}

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<>
				<div className="log">
					<pre>{this.state.text}</pre>
				</div>
			</>
		);
	}
}

export default Log;
