import React from "react";
import ZoneTile from "../component/ZoneTile";
import Button from "../component/Button";

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
			ledService: props.ledService,
			ledConfiguration: props.ledConfiguration,
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
	 * Return to standard animations.
	 * @param {*} even triggering event
	 * @param {*} callback callback function to set the status of the button
	 */
	setStandardAnimation = (event, callback) => {
		const state = this.state;
		const ledConfiguration = this.state.ledConfiguration;
		const specificLedConfiguration = ledConfiguration[0];

		// Reset the type
		const customFields = specificLedConfiguration.getCustomFields();
		specificLedConfiguration.setType(customFields[14]);

		const result = state.ledService.postLedConfiguration(ledConfiguration);
		result.then(() => {
			callback(true);
			this.setState(state);
		});
		result.catch((error) => {
			callback(false);
		});
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>Select Zone</h2>

				{this.state.ledConfiguration[0].getType() === 255 ? (
					<>
						<div className="spacer2"></div>
						<p style={{ textAlign: "center" }}>
							You are currently using a custom animation. Because of that, the zones can not be configured
							manually. Please return to standard animations to customize your zones.
						</p>

						<Button
							className="button"
							title="Default animations"
							successTitle="Done!"
							errorTitle="Failed, please retry!"
							successClassName="button success"
							errorClassName="button error"
							onClick={this.setStandardAnimation}
						/>
					</>
				) : (
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
				)}
			</div>
		);
	}
}

export default ZonePage;
