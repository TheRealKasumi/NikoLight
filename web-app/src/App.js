import React from "react";
import ZonePage from "./page/ZonePage";
import AnimatorPage from "./page/AnimatorPage";
import SettingsPage from "./page/SettingsPage";
import LogPage from "./page/LogPage";
import UpdatePage from "./page/UpdatePage";
import NavigationBar from "./component/NavigationBar";
import SystemService from "./service/SystemService";
import LedService from "./service/LedService";
import WifiService from "./service/WifiService";
import LogService from "./service/LogService";
import UpdateService from "./service/UpdateService";
import "./App.css";

/**
 * Base component of the App.
 */
class App extends React.Component {
	/**
	 * Create a new instance of the {App} base component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			loading: true,
			loadingAnimationEnded: false,
			roundCorners: false,
			page: { id: -1, vars: {} },
			systemService: new SystemService("/api/"),
			ledService: new LedService("/api/"),
			wifiService: new WifiService("/api/"),
			logService: new LogService("/api/"),
			updateService: new UpdateService("/api/"),
			systemConfiguration: null,
			ledConfiguration: null,
			wifiConfiguration: null,
		};
	}

	/**
	 * Is called automatically once the component is mounted.
	 */
	componentDidMount() {
		this.loadSystemConfiguration();
		this.loadLedConfiguration();
		this.loadWifiConfiguration();

		setTimeout(() => {
			const state = this.state;
			state.loadingAnimationEnded = true;
			this.setState(state);
		}, 2900);
	}

	/**
	 * Load the {SystemConfiguration} from the TesLight controller.
	 */
	loadSystemConfiguration = () => {
		const result = this.state.systemService.getSystemConfiguration();
		result.then((systemConfiguration) => {
			const state = this.state;
			state.systemConfiguration = systemConfiguration;
			this.setState(state);
			this.configurationLoadedCallback();
		});
		result.catch((error) => {
			this.configurationLoadFailedCallback(error);
		});
	};

	/**
	 * Load the {LedConfiguration} from the TesLight controller.
	 */
	loadLedConfiguration = () => {
		const result = this.state.ledService.getLedConfiguration();
		result.then((ledConfiguration) => {
			const state = this.state;
			state.ledConfiguration = ledConfiguration;
			this.setState(state);
			this.configurationLoadedCallback();
		});
		result.catch((error) => {
			this.configurationLoadFailedCallback(error);
		});
	};

	/**
	 * Load the {WifiConfiguration} from the TesLight controller.
	 */
	loadWifiConfiguration = () => {
		const result = this.state.wifiService.getWifiConfiguration();
		result.then((wifiConfiguration) => {
			const state = this.state;
			state.wifiConfiguration = wifiConfiguration;
			this.setState(state);
			this.configurationLoadedCallback();
		});
		result.catch((error) => {
			this.configurationLoadFailedCallback(error);
		});
	};

	/**
	 * Callback is called when a configuration was loaded successfully.
	 * Once all configurations are loaded, a timer is started to wait
	 * for the end of the loading animation. Then the UI is shown.
	 */
	configurationLoadedCallback = () => {
		if (
			this.state.systemConfiguration !== null &&
			this.state.ledConfiguration !== null &&
			this.state.wifiConfiguration !== null
		) {
			const interval = setInterval(() => {
				if (this.state.loadingAnimationEnded === true) {
					clearInterval(interval);
					const state = this.state;
					state.loading = false;
					state.page.id = 0;
					this.setState(state);
				}
			}, 100);
		}
	};

	/**
	 * Callback is called when the laoding of a configuration failed.
	 * This could happens due to connection errors.
	 * @param {Exception} error the exception that caused the error
	 */
	configurationLoadFailedCallback = (error) => {
		alert("Failed to load the configuration from the TesLight controller.");
		// Todo handle error
	};

	/**
	 * Set the currently visible page.
	 * @param {number} pageId
	 * @param {object} vars
	 */
	setPage = (pageId, vars) => {
		const state = this.state;
		state.page.id = pageId;
		state.page.vars = vars;
		this.setState(state);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div
				id="phone"
				style={{ borderRadius: this.state.roundCorners ? "50px" : "0" }}
				className={this.state.loading ? "phone startup" : "phone"}
			>
				<div className="logo">
					<img src={process.env.PUBLIC_URL + "/img/logo.svg"} />
				</div>
				<div className="content">
					{this.state.page.id === 0 ? <ZonePage setPage={this.setPage} /> : null}
					{this.state.page.id === 1 ? (
						<AnimatorPage
							ledService={this.state.ledService}
							ledConfiguration={this.state.ledConfiguration}
							selectedZone={this.state.page.vars.zoneIndex}
							setPage={this.setPage}
						/>
					) : null}
					{this.state.page.id === 2 ? (
						<SettingsPage
							systemService={this.state.systemService}
							wifiService={this.state.wifiService}
							systemConfiguration={this.state.systemConfiguration}
							wifiConfiguration={this.state.wifiConfiguration}
						/>
					) : null}
					{this.state.page.id === 3 ? <LogPage logService={this.state.logService} /> : null}
					{this.state.page.id === 4 ? <UpdatePage updateService={this.state.updateService} /> : null}
				</div>
				<NavigationBar selectedIndex={0} setPage={this.setPage} />
			</div>
		);
	}
}

export default App;
