import React from "react";
import Button from "../component/Button";
import FileTable from "../component/FileTable";
import TextInput from "../component/TextInput";

/**
 * Component containing a setting page for the fseq files.
 */
class FseqPage extends React.Component {
	/**
	 * Create a new instance of the {FseqPage} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			fseqService: props.fseqService,
			ledService: props.ledService,
			ledConfiguration: props.ledConfiguration,
			fseqList: [null],
			fileName: "",
			uploadFinished: true,
			componentKey: 0,
		};
		this.loadFseqList();
	}

	/**
	 * Load the fseq list from the TesLight controller.
	 */
	loadFseqList = () => {
		const state = this.state;
		const result = state.fseqService.getFileList();
		result.then((data) => {
			state.fseqList = data;
			state.componentKey += 10;
			this.setState(state);
		});
		result.catch((error) => {
			state.fseqList = null;
			state.componentKey += 10;
			this.setState(state);
		});
	};

	/**
	 * Set the file name for the upload when typing.
	 * @param {string} fileName
	 */
	setFileName = (fileName) => {
		const state = this.state;
		state.fileName = fileName;
		this.setState(state);
	};

	/**
	 * Callback function is called when a play button for an fseq file is pressed.
	 * @param {number} index index of the file to play
	 * @param {string} fileName name of the file to play
	 * @param {number} id identifier of the file to play
	 */
	play = (index, fileName, id) => {
		const state = this.state;
		const ledConfiguration = this.state.ledConfiguration;
		const specificLedConfiguration = ledConfiguration[0];

		// Type 255 means custom animation
		const prevType = specificLedConfiguration.getType();
		specificLedConfiguration.setType(255);
		const customFields = specificLedConfiguration.getCustomFields();

		// Write the file id into the custom fields
		for (let i = 0; i < 4; i++) {
			const byte = id & 0xff;
			customFields[i + 10] = byte;
			id = (id - byte) / 256;
		}

		// Set the last custom field to the previous calculated animation
		if (prevType != 255) {
			customFields[14] = prevType;
		}

		const result = state.ledService.postLedConfiguration(ledConfiguration);
		result.then(() => {
			this.setState(state);
		});
		result.catch((error) => {
			alert(
				"Failed to play animation file. Probably it is incompatible with your LED configuration. Make sure you use the correct number of LEDs."
			);
		});
	};

	/**
	 * Callback function is called, when a delete button for an fseq file is pressed.
	 * @param {number} index index of the file to play
	 * @param {string} fileName name of the file to play
	 * @param {number} id identifier of the file to play
	 */
	delete = (index, fileName, id) => {
		const state = this.state;
		const result = state.fseqService.deleteFseqFile(fileName);
		result.then(() => {
			this.loadFseqList();
		});
		result.catch((error) => {
			alert("The file cound not be delete. Probably you are currently playing it.");
		});
	};

	/**
	 * Upload a fseq file to the controller.
	 * @param {*} event event that triggered the action
	 * @param {*} callback callback function for the button state
	 */
	uploadFile = async (event, callback) => {
		const inputFile = document.getElementById("fseq");
		const state = this.state;

		if (inputFile.files[0] != null) {
			if (state.uploadFinished === true) {
				state.uploadFinished = false;

				try {
					await state.fseqService.postFseqFile(inputFile.files[0], state.fileName);
					this.loadFseqList();
					callback(true);
				} catch (error) {
					callback(false);
				}

				state.uploadFinished = true;
				this.setState(state);
			} else {
				alert("Stawwwwwp! You are already uploading the file. Please wait!");
			}
		} else {
			alert("Oh boy! You haven't even selected a file!");
		}
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<div className="wrapper active">
				<h2>Custom Animations</h2>
				<div className="spacer"></div>

				{this.state.fseqList === null ? (
					<>
						<div className="spacer"></div>
						<p>Failed to load data :(</p>
					</>
				) : this.state.fseqList.length === 0 ? (
					<>
						<h2>No data</h2>
						<div className="spacer"></div>

						<input id="fseq" type="file" name="FSEQ Version 1.0" accept=".fseq" />
						<div className="spacer"></div>

						<TextInput title="File name" value={this.state.fileName} onChange={this.setFileName} />
						<div className="spacer"></div>

						<Button
							key={this.state.componentKey + 1}
							className="button"
							title="Upload"
							successTitle="Animation uploaded"
							errorTitle="Failed to upload animation"
							successClassName="button success"
							errorClassName="button error"
							onClick={this.uploadFile}
						/>
					</>
				) : this.state.fseqList[0] === null ? (
					<>
						<div className="spacer"></div>
						<h2>Loading data...</h2>
					</>
				) : (
					<>
						<FileTable
							key={this.state.componentKey}
							title="Files"
							values={this.state.fseqList}
							onPlay={this.play}
							onDelete={this.delete}
						/>
						<div className="spacer"></div>

						<input id="fseq" type="file" name="FSEQ Version 1.0" accept=".fseq" />
						<div className="spacer"></div>

						<TextInput title="File name" value={this.state.fileName} onChange={this.setFileName} />
						<div className="spacer"></div>

						<Button
							key="upload_fseq_button"
							className="button"
							title="Upload"
							successTitle="Animation uploaded"
							errorTitle="Failed to upload animation"
							successClassName="button success"
							errorClassName="button error"
							onClick={this.uploadFile}
						/>
					</>
				)}
			</div>
		);
	}
}

export default FseqPage;
