import React from "react";
import Button from "../component/Button";

/**
 * Component containing a page for installing system updates to the controller.
 */
class UpdatePage extends React.Component {
	/**
	 * Create a new instance of the {UpdatePage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			updateService: props.updateService,
			uploadFinished: true,
			componentKey: 0,
		};
	}

	/**
	 * Upload the selected TUP file to the controller.
	 */
	upload = async () => {
		const inputFile = document.getElementById("tup");
		const state = this.state;

		if (inputFile.files[0] != null) {
			if (state.uploadFinished === true) {
				state.uploadFinished = false;
				state.componentKey++;
				this.setState(state);

				try {
					await state.updateService.uploadTupFile(inputFile.files[0]);
					alert("The upload was successful. Now please wait for the controller to restart.");
				} catch (error) {
					alert("Unfortunately the upload failed.");
				}

				state.uploadFinished = true;
				state.componentKey++;
				this.setState(state);
			} else {
				alert("Stawwwwwp! What are you doing? You are already uploading the file. Please wait!");
			}
		} else {
			alert("Hey! I told you to be careful and you are not even selecting a file for the update...");
		}
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>System Update</h2>
				<div className="spacer2"></div>

				<div style={{ textAlign: "center" }}>
					<p>
						Here a system update can be installed to the TesLight controller. This includes the firmware of the
						mcu as well as the user interface. To install a system update, you first need to download a so called
						"TesLight Update Package". These files have the extension ".tup" and can be found in the release
						section on my GitHub page:
						<br />
						<br />
						https://github.com/TheRealKasumi/TesLight
						<br />
						<br />
						After the download, please use this page to upload the update package to the controller. Once the file
						is uploaded, the controller will restart and install the update. This can take up to 2 minutes but
						usually is faster. During this time, the controller will not be reachable via the WiFi hotspot. Please
						be patient. Afterwards the controller will restart and become reachable as usual.
						<br />
						<br />
						But here is also a word of warning. Uploading broken update packages could make the controller
						unreachable. Then a wired connection to reinstall the firmware is required. So it is recommended to
						only upload stable and tested update packages from the GitHub page.
					</p>
				</div>
				<div className="spacer2"></div>

				<input id="tup" type="file" name="TesLight Update Package" accept=".tup" />
				<div className="spacer"></div>

				<Button
					key={this.state.componentKey}
					title={this.state.uploadFinished ? "Update" : "Working..."}
					onClick={this.upload}
				/>
			</div>
		);
	}
}

export default UpdatePage;
