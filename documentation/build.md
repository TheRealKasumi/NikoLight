# Build Guide

Before starting with this guide, make sure to read the [planning](planning.md) document and [part list](part-list.md) first.
They contain important information and considderations before starting the project.
If you start to build the project blindly, you might run into issues later.

## General Knowledge about the Hardware

Todo

## Clone or Download the Project Files

Before we start, you should clone or download the project files.
Go to the [main page](https://github.com/TheRealKasumi/TesLight) and click the `Code` button.
You can then decide if you want to clone the repository using [Git](https://git-scm.com/) or [download](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip) it as zip file.

![Clone](media/build/clone.jpeg)

### Using Git

- Open a terminal in your destination folder
- Run `git clone https://github.com/TheRealKasumi/TesLight.git`
- A folder `TesLight` should be created, containing all project files

### Download as Zip

- Click the [download link](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip)
- Extract the TesLight folder

## Order the PCB and 3D Printed Parts

Like already mentioned in the [part list](part-list.md), the PCB and 3D printed parts can be ordered at [JLCPCB](https://jlcpcb.com/).
But of course you are free to order the parts somewhere else or create them by yourself.

Open their website and click on [Order Now](https://cart.jlcpcb.com/quote) on the right, top of the page.
This should bring you to the configuration page wher you can upload the files and configure the PCB.

### Upload Gerber Files

At first the so called `gerber files` should be uploaded.
These contain all the data required to producte the PCB.
They can be found in the [/pcb/gerber](/pcb/gerber/) folder of the project.
JLCPCB requires you to upload all of them as a zip file.
Create a new zip file and put all files from the gerber folder into the root of the zip file.
Click on `Add gerber file` on the configuration page and then upload the newly created zip file.
Once the upload and processing is done, you will see a preview of the PCB. It should look like this:

![PCB](media/build/pcb.png)

### Configuration

Generally you should be fine with their default configuration.
But you are free to modify it if you wish.
However keep in mind that this can have a big impact on the price.
Changing the number of layers and dimmensions is not recommended and can lead to problems later.
Also the PCB shouldn't be too thin.
Here the default 1.6mm are just perfect.
When you are done, add the PCB to the shopping cart.

![PCB Options](media/build/pcb-options.jpeg)

### Upload 3D Files

This works very similar to the steps described above.
Go back to the [order page](https://cart.jlcpcb.com/quote) and select `3D-Printing` at the top of the page.
The 3D files can be found in the [/models](/models/) folder of the project.
You will need one print of the [mcu_case_all.stl](/models/mcu_case_all.stl) which is the case for the TesLight controller.
Also depending on the number of light injectors for the fibre tubes, you will need the [led-case.stl](/models/led-case.stl).
The material choice is fully up to you.
But it shouldn't be metal so that it is **NOT** electrically conductive.
When you are done, add the 3D prints to the shopping cart.

### Order

Go to your shopping cart and make sure it contains all the items.
Then continue their instructions to finish the order.

## Assemble the TesLight Controller

## Assemble the LEDs

## Upload the Software

### TesLight Controller 

Uploading the software is the final step before you can finally test your work.
Please install the following software and extension for the upload procedure:

- [VS Code](https://code.visualstudio.com/download)
- [PlatformIO](https://platformio.org/install/ide?install=vscode)

Start VS Code and then open the [mcu](/mcu/) folder of the project.
You can do so by clicking `File` -> `Open Folder...`.
On the left all files inside the opened folder should be listed.
Open the the file `platformio.ini`.
In the configuration you can see two attributes, `upload_port` and `monitor_port`.

![plattformio.ini](media/build/plattformio-ini.jpeg)

Connect the ESP32 of the TesLight controller to your computer via a micro USB cable.
Now you need to find out which port the controller was assigned to.
The recommendation is to use the device manager when you are using Windows.
Press the Windows key + R and type `devmgmt.msc`.
Then hit enter.
The device manager will start up and you should be able to identify the port of the controller.
Go back to the `platformio.ini` and adjust the port.
Save the file and then click the upload button at the left, bottom.

![Upload Button](media/build/upload-button.jpeg)

The code should now be compiled and uploaded to the board.
In case the upload fails, hold down the `Boot` button on the ESP32 until the upload started.

Now open the serial monitor by clicking the "plug" button, near to the upload button.

![Serial Button](media/build/monitor-button.jpeg)

You should see the TesLight controller starting up but then stopping with `Failed to initialize SD card`.

![SD Fail Log](media/build/fail-init-sd.jpeg)

### Prepare and insert the Micro SD Card

