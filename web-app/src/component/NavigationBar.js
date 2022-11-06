import React from "react";
import "../App.css";

/**
 * Component contains the navigation bar at the bottom of the app.
 */
class NavigationBar extends React.Component {
	/**
	 * Create a new instance of the {NavigationBar}.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			selectedIndex: props.selectedIndex,
			setPage: props.setPage,
		};
	}

	/**
	 * Set the selected index of the {NavigationBar} and open the correct page.
	 * @param {number} index
	 */
	setSelectedIndex = (index) => {
		const state = this.state;
		state.selectedIndex = index;
		this.setState(state);

		switch (index) {
			case 0:
				this.state.setPage(0, {});
				break;
			case 1:
				this.state.setPage(2, {});
				break;
			case 2:
				this.state.setPage(3, {});
				break;
			case 3:
				this.state.setPage(4, {});
				break;
			case 4:
				this.state.setPage(5, {});
				break;
		}
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<nav>
				<ul>
					<li
						className={this.state.selectedIndex === 0 ? "active" : ""}
						onClick={() => {
							this.setSelectedIndex(0);
						}}
					>
						<img src={process.env.PUBLIC_URL + "/img/button/led.svg"} />
					</li>

					<li
						style={{ display: "none" }}
						className={this.state.selectedIndex === 4 ? "active" : ""}
						onClick={() => {
							this.setSelectedIndex(4);
						}}
					>
						<img src={process.env.PUBLIC_URL + "/img/button/film.svg"} />
					</li>

					<li
						className={this.state.selectedIndex === 1 ? "active" : ""}
						onClick={() => {
							this.setSelectedIndex(1);
						}}
					>
						<img src={process.env.PUBLIC_URL + "/img/button/gear.svg"} />
					</li>

					<li
						className={this.state.selectedIndex === 3 ? "active" : ""}
						onClick={() => {
							this.setSelectedIndex(3);
						}}
					>
						<img src={process.env.PUBLIC_URL + "/img/button/update.svg"} />
					</li>

					<li
						className={this.state.selectedIndex === 2 ? "active" : ""}
						onClick={() => {
							this.setSelectedIndex(2);
						}}
					>
						<img src={process.env.PUBLIC_URL + "/img/button/log.svg"} />
					</li>
				</ul>
			</nav>
		);
	}
}

export default NavigationBar;
