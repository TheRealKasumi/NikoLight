# TesLight Update Package Tool

This tool was developted to create `TUP` files, which stands for `TesLight Update Package`.
It is a simple package that combines a variable number of files and folders into a single file.
This package file is then used to update the TesLight controller via OTA update.

When updating the controller, there are two main components to update.

1. The firmware on the microcontroller.
   This is a binary artifact create from compiling the source code.
   During the update process, it is written to the controller's flash memory.
   Once the controller is restarting, it will load the new firmware from the flash.

2. The files on the MircoSD card.
   The MircoSD card stores multiple files, for example for the web base user interface.
   These files should be updated as well during the update procedure.

Since there can be many files to update, it would be impractical to upload them as single files to the controller.
Also this could lead to errors easily, for example when the connection breaks and only parts of the files are updated.
So the decision was made to update everything on the system with a single package file.

## Build

You can use any C++17 compatible compiler to build this tool.
I used [g++](https://www.mingw-w64.org/) on Windows.

```sh
mkdir build
g++ -g src\*.cpp -o build\tupt.exe
```

## Usage

First of all you need to create a new folder.
Here you need to copy all data for the update, including the firmware and the frotnend files.
All other data, except user settings, log and animation data is removed from the MicroSD card after the update.
This can make the controller unusable without connecting it to your computer again, so be careful.
By convention the firmware file for the controller is called `firmware.bin` and must be in the root of the created update folder.
Once you copied all files to the update folder, we are ready to go.

```sh
tupt <output_file> <source_directory>
```

## TUP File Format

There is nothing complicated about this file format.
It consists of a simple header and a variable number of data blocks.
The header contains some general information while each data block represents a file or directory.

### TUP Header

| index | type    | description                                            |
| ----- | ------- | ------------------------------------------------------ |
| 0     | char[4] | Identifier, always "TLUP"                              |
| 4     | uint8   | File version, should be 1                              |
| 5     | uint32  | A very simple hash to run a quick check on the package |
| 9     | uint32  | Number of following data blocks                        |

The data blocks start directly after the header.

### TUP Data Blocks

| index | type    | description                                                   |
| ----- | ------- | ------------------------------------------------------------- |
| 0     | uint8   | Type of the data block: 0 = firmware, 1 = file, 2 = directory |
| 1     | uint16  | Length of the following path in bytes                         |
| 1     | char[n] | The null terminated path and file name for the installation   |
| n + 1 | uint32  | The size of the data                                          |
| n + 5 | uint8\* | Array of bytes, representing the data of the embedded file    |
