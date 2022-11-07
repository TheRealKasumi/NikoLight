import React from "react";
import Log from "../component/Log";
import Button from "../component/Button";

/**
 * Component containing a page for displaying, downloading and clearing the log.
 */
class LogPage extends React.Component {
	/**
	 * Create a new instance of the {LogPage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			logService: props.logService,
			logSize: 0,
			logText: "Loading...",
			logKey: 0,
		};
	}

	/**
	 * Is called automatically once the component is mounted.
	 */
	componentDidMount() {
		this.reloadLog();
	}

	/**
	 * Read the log from the TesLight controller.
	 */
	reloadLog = () => {
		const maxLogSize = 10000;
		const state = this.state;
		const logSize = state.logService.getLogSize();
		logSize
			.then((logSize) => {
				state.logSize = logSize;
				let logStart = 0;
				let byteCount = 0;
				if (logSize > maxLogSize) {
					logStart = logSize - maxLogSize;
					byteCount = maxLogSize;
				} else {
					logStart = 0;
					byteCount = logSize;
				}

				const logText = state.logService.getLog(logStart, byteCount);
				logText
					.then((logText) => {
						state.logText = logText;
						state.logKey++;
						this.setState(state);
					})
					.catch((error) => {
						state.logSize = 0;
						state.logText = "Failed to load log from the controller.";
						state.logKey++;
						this.setState(state);
					});
			})
			.catch((error) => {
				state.logSize = 0;
				state.logText = "Failed to load log size from the controller.";
				state.logKey++;
				this.setState(state);
			});
	};

	/**
	 * Clear the log on the TesLight controller.
	 */
	clearLog = (event, callback) => {
		const state = this.state;
		const result = state.logService.clearLog();
		result
			.then(() => {
				this.reloadLog();
				callback(true);
			})
			.catch(() => {
				callback(false);
			});
	};

	/**
	 * Download the log file.
	 */
	downloadLog = (event, callback) => {
		const state = this.state;
		const logSize = state.logService.getLogSize();
		logSize
			.then((logSize) => {
				const logText = state.logService.getLog(0, logSize);
				logText
					.then((logText) => {
						this.download(logText);
						callback(true);
					})
					.catch((error) => {
						callback(false);
					});
			})
			.catch((error) => {
				callback(false);
			});
	};

	/**
	 * Start a text file download.
	 */
	download = (text) => {
		const element = document.createElement("a");
		element.setAttribute("href", "data:text/plain;charset=utf-8," + encodeURIComponent(text));
		element.setAttribute("download", "teslight-log.txt");
		element.style.display = "none";
		document.body.appendChild(element);
		element.click();
		document.body.removeChild(element);
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>TesLight Log</h2>
				<div className="spacer"></div>

				<Log key={`log-text-field-${this.state.logKey}`} text={this.state.logText} />
				<div className="spacer"></div>

				<p key={`log-size-info-${this.state.logKey}`}>
					{`Size: ${(this.state.logSize / 1024.0 / 1024.0).toFixed(3)} MB`}
				</p>
				<div className="spacer"></div>

				<Button
					className="button"
					title="Clear log"
					successTitle="Log cleared"
					errorTitle="Failed to clear log"
					successClassName="button success"
					errorClassName="button error"
					onClick={this.clearLog}
				/>
				<div className="spacer"></div>

				<Button
					className="button"
					title="Download"
					successTitle="Download started"
					errorTitle="Failed to start download"
					successClassName="button success"
					errorClassName="button error"
					onClick={this.downloadLog}
				/>
			</div>
		);
	}
}

export default LogPage;
