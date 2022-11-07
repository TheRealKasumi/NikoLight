import React from "react";
import Button from "./Button";
import "../App.css";

/**
 * File table component to list fseq files, their size, ID and offer a play/delete action.
 */
class FileTable extends React.Component {
	/**
	 * Create a new instance of the {@FileTable} component.
	 * @param {object} props properties of the component
	 */
	constructor(props) {
		super(props);
		this.state = {
			title: props.title,
			values: props.values,
			onPlay: props.onPlay,
			onDelete: props.onDelete,
		};
	}

	/**
	 * Is calling the callback function to play a file.
	 * @param {number} index index of the file to play
	 * @param {string} fileName name of the file to play
	 * @param {number} id identifier of the file to play
	 */
	onPlay = (index, fileName, id) => {
		const state = this.state;
		if (state.onPlay) {
			state.onPlay(index, fileName, id);
		}
	};

	/**
	 * Is calling the callback function to delete a file.
	 * @param {number} index index of the file to delete
	 * @param {string} fileName name of the file to delete
	 * @param {number} id identifier of the file to delete
	 */
	onDelete = (index, fileName, id) => {
		const state = this.state;
		if (state.onDelete) {
			state.onDelete(index, fileName, id);
		}
	};

	/**
	 * Render the component.
	 * @returns generated JSX
	 */
	render() {
		return (
			<>
				<label className="title">{this.state.title}</label>
				<table className="table">
					<thead>
						<tr>
							<th key="file_table_header_1">File</th>
							<th key="file_table_header_2">Size</th>
							<th key="file_table_header_3">ID</th>
							<th key="file_table_header_4">Actions</th>
						</tr>
					</thead>
					<tbody>
						{this.state.values &&
							this.state.values.length > 0 &&
							this.state.values.map((file, index) => (
								<tr key={`file_table_row_${index}`}>
									<td key={`file_table_cell_1_${index}`}>{file.fileName}</td>
									<td key={`file_table_cell_2_${index}`}>
										{(file.fileSize / 1024 / 1024).toFixed(2) + " MB"}
									</td>
									<td key={`file_table_cell_3_${index}`}>{file.fileId}</td>
									<td key={`file_table_cell_4_${index}`}>
										<Button
											key={`file_table_play_button_1_${index}`}
											className="button"
											title="Play"
											onClick={() => {
												this.onPlay(index, file.fileName, file.fileId);
											}}
										/>
										<Button
											key={`file_table_play_button_2_${index}`}
											className="button"
											title="Delete"
											onClick={() => {
												this.onDelete(index, file.fileName, file.fileId);
											}}
										/>
									</td>
								</tr>
							))}
					</tbody>
				</table>
			</>
		);
	}
}

export default FileTable;
