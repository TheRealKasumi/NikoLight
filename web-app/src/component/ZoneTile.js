import React from "react";

/**
 * Component containing a single tile for the zone selection.
 */
class ZoneTile extends React.Component {
	/**
	 * Create a new instance of the {ZoneTile} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);

		this.state = {
			active: false,
			title: props.title,
			icon: props.icon,
			flipped: props.flipped,
			onClick: props.onClick,
		};
	}

	/**
	 * Set the zone button as active.
	 */
	setActive = () => {
		const state = this.state;
		state.active = true;
		this.setState(state);
	};

	/**
	 * Set the zone button as inactive.
	 */
	setInactive = () => {
		const state = this.state;
		state.active = false;
		this.setState(state);
	};

	/**
	 * Call the click event with a small delay.
	 */
	itemClick = () => {
		setTimeout(() => {
			this.state.onClick();
		}, 250);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<li>
				<label
					className={this.state.active === true ? "active" : ""}
					onMouseDown={this.setActive}
					onMouseUp={this.setInactive}
					onTouchStart={this.setActive}
					onTouchEnd={this.setInactive}
					onClick={this.itemClick}
				>
					<img
						src={this.state.icon}
						alt=""
						className="bi bi-car-front"
						style={{ transform: this.state.flipped ? "scaleX(-1)" : "scaleX(1)" }}
					/>
					<p>{this.state.title}</p>
				</label>
			</li>
		);
	}
}

export default ZoneTile;
