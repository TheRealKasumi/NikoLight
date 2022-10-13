import React from "react";
import ZoneTile from "../component/ZoneTile";

/**
 * Component containing a selection page for the zone.
 * After a zone was selected, it will open the {AnimatorPage} to configure the animation for the selected zone.
 */
class ZonePage extends React.Component {
	/**
	 * Create a new instance of the {ZonePage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			setPage: props.setPage,
		};
	}

	/**
	 * Calls the parents callback to change the page to the {AnimatorPage}
	 * to edit the LED animation of the selected zone.
	 * @param {number} zoneIndex
	 */
	openAnimatorPage = (zoneIndex) => {
		this.state.setPage(1, { zoneIndex: zoneIndex });
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>Select Zone</h2>
				<ul className="zone">
					<ZoneTile
						title="Dashboard"
						icon={process.env.PUBLIC_URL + "/img/zone/dashboard-animated.svg"}
						flipped={false}
						onClick={() => {
							this.openAnimatorPage(0);
						}}
					/>
					<ZoneTile
						title="Center Console"
						icon={process.env.PUBLIC_URL + "/img/zone/center-console-animated.svg"}
						flipped={false}
						onClick={() => {
							this.openAnimatorPage(1);
						}}
					/>
					<ZoneTile
						title="FL Door"
						icon={process.env.PUBLIC_URL + "/img/zone/door-animated.svg"}
						flipped={true}
						onClick={() => {
							this.openAnimatorPage(2);
						}}
					/>
					<ZoneTile
						title="FR Door"
						icon={process.env.PUBLIC_URL + "/img/zone/door-animated.svg"}
						flipped={false}
						onClick={() => {
							this.openAnimatorPage(3);
						}}
					/>
					<ZoneTile
						title="RL Door"
						icon={process.env.PUBLIC_URL + "/img/zone/door-animated.svg"}
						flipped={true}
						onClick={() => {
							this.openAnimatorPage(4);
						}}
					/>
					<ZoneTile
						title="RR Door"
						icon={process.env.PUBLIC_URL + "/img/zone/door-animated.svg"}
						flipped={false}
						onClick={() => {
							this.openAnimatorPage(5);
						}}
					/>
				</ul>
			</div>
		);
	}
}

export default ZonePage;
