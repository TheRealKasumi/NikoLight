# Build Guide

Before starting with this guide, make sure to read the [planning](planning.md) document and [part list](part-list.md) first.
They contain important information and some considerations before beginning the project.
If you start to build the project blindly, you might run into issues later.

## General Knowledge about the Hardware

Let's start with some general knowledge about the hardware.
I know this might be boring, but I also have to cover people who want to know more about it.

At first the TesLight controller is based on an ESP32 microcontroller board, which is placed on a custom PCB.
The ESP32 contains two Tensilica-LX6 cores, running at 80MHz or 240MHz.
They come with 512kB of SRAM and 4MB of flash memory.
Also, they have onboard WiFi (802.11bgn) as well as Bluetooth (classic and LE).
Hardware support for SPI, I2C, CAN, UART, etc. could be used in the future.
Generally the board operates at the 3.3V level.
It has a voltage regulator which will provide the 3.3V from the 5V, provided by the TesLight PCB.
If you plan to do your own customisations please keep in mind that all IO pins can only handle 3.3V and a very limited current in the range of a few milliamps.

The TesLight PCB contains a few active and passive components.
First of all, there is voltage regulator, providing stable 5V to the LEDs and the ESP32 board.
To be more precise, there now are two channels that can provide a max of 3A per channel or 6A in total.
There is no force to build both channels.
The board also works with a single regulator but then the output power is more limited of course.
For a single regulator build the Jumper 'J1' should be closed.

By default the channels of the regulators are not connected but of course they share a common ground.
The reason for not connecting them is that this can lead to trouble due to manufacuting tolerances.
If one of the two regulators provides a slightly higher voltage than the other regulator, most of the load will be driven by the regulator with the higher voltage.
This could lead to one regulator overheating and also to unwanted osccilations.
In case you feel confident connecting the outputs together, this is possible via the jumper `J1`.
Otherwise each regulator will drive 4 output channels.
The first regulator will drive the channels 2, 4, 6, 8 and the second regulator will drive the channels 1, 3, 5, 7.

The dual regulator build is always recommended to be more flexible and have more headroom.
When properly configured, the software will now also keep an eye on your power consumption.
The power draw is calculated and limited for each regulator by limiting the LED brightness.
Also two temperature sensors can be added to the regulators. For Temperature Sensor Install Position see following picture:

<a href="media/build/install_temperature_sensor.jpg"><img src="media/build/install_temperature_sensor.jpg" alt="Temperature Sensor Install Position" style="width:150px"></a>

The software will then also check for the temperature, control a cooling fan and reduce the power output if necessary.
For even more protection there is a fuse and a poly fuse in place.

The regulators can handle an input voltage of up to 45V according to the [specs](https://www.ti.com/lit/ds/symlink/lm2596.pdf?ts=1663311548260&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FLM2596), which makes the board also very usable for 24V applications.
But keep in mind that the input capacitors might need to be changed and that these 45V are an absolute maximum rating.

Next the PCB holds a MPU6050 motion sensor.
It is used for interactive animations using acceleration and rotation data.
The sensor is suitable for up to 16g's but TesLight is using a limit of 2g's currently.
If anyone can prove to me that 2g's are not enough in a Tesla, then I will congratulate you and we can talk about setting it to 4g in the software😋.
Hidden challenge?
I... I would never challenge you to do stupid things...

![Girl Shy](media/fun/girl-shy.gif)

Umm... we better continue... So... where did we stop?

Since we learned the hard way that it is not a good idea to drive WS2812-type LEDs with a 3.3V signal, there now is a 74HCT541D on the board.
It can safely read the 3.3V data signals from the ESP32 board and convert it to a 5V level.
This now allows us so safely drive all kind of WS2812-type LEDs, even over longer signal wires.

There is also a voltage divider, with a following low-pass filter used to lower and smooth the voltage of the sensor pin
It can then be used by the ESP32 and converted into a digital value using it's ADC (analog digital converter).
This pin can be connected to your car's existing ambient light to give TesLight a signal when to turn on and off or to adjust the brightness automatically.

Lastly, there are some passive components like resistors and capacitors to protect the data lines, pull the I2C data lines to 3.3V and to smooth some things out...
I guess we are getting too basic here...

Like mentioned the used LED type is WS2812 or compatible.
These LEDs have integrated controllers, capable of receiving a digital 24 bit colour value.
When connected in series they function as a shift register.
TesLight is able to drive a variable number of LEDs in series (up to a few hundred).
The LEDs operate at 5V and **only 5V**.
Never connect them directly to the 12V of your car.
If you are using other LEDs that are 12V compatible and share the same data protocoll, make sure to never feed 12V into the TesLight board via the power output or data pins.

The following picture shows the pinout of the TesLight board.

![Teslight Pinout](media/build/testlight-pinout.png)

As you can see there are 8 connectors for the LED outputs.
These provide power to your LEDs and a data signal suitable for driving the WS2812 type.
The connectors are ordered like this:

| 1 | 2 |<br>
| 3 | 4 |<br>
| 5 | 6 |<br>
| 7 | 8 |

By default these outputs are mapped to the following zones.
This mapping can be changed via the (build time) configuration.
However the mapping was not chosen randomly and should only be changed if you know what you are doing.
Since there now are 2 voltage regulators, the power draw should be split as even as possible between the regulators.
The mapping was chosen based on this criteria.

| Channel   | Mapped to        |
| --------- | ---------------- |
| Channel 1 | Dash             |
| Channel 2 | Center console   |
| Channel 3 | Front left door  |
| Channel 4 | Front right door |
| Channel 5 | Rear left door   |
| Channel 6 | Rear right door  |
| Channel 7 | Footwell left    |
| Channel 8 | Footwell right   |

At the bottom of the PCB there are 3 connectors, a micro USB port and a micro SD card slot.

The 4 pin XH connector is for powering the board.
The input voltage can be up to 45V with suitable capacitors (absolute maximum rating).
Keep in mind that according to the picture diagram above, the right pin is connected to ground.
Never connect the ground pin to the positive wire of your footwell lights.
If you are sure that there is a good ground connection, which is usually the case, there is no need to use it.
One last thing to keep in mind is that only positive voltages can be measured, so avoid doing something crazy with this simple input.

The center 2 pin XH connector can be used as a sensor to measure the voltage of your car's ambient light and supply TesLight an on/off or brightness signal.
The pin can be used with analog voltages and also PWM signals due to the low-pass filter.
It is recommended to not measure voltages above 19V with this controller.
There is an overvoltage protection in place and exceeding this limit for a short period should be possible but not recommended.
Instead the resistor `R9` and `R10` of the voltage devider can be adjusted accordingly.
Use an "electricity thief" to get the signal of the factory build footwell lights without harm. It could look like this if you add the correct plugs to get the signal without any harm. See [part list](part-list.md).

<a href="media/build/get_signal_of_footwell_lights_without_harm.jpg"><img src="media/build/get_signal_of_footwell_lights_without_harm.jpg" alt="get signal of footwell lights without harm" style="width:150px"></a>
<a href="media/build/get_signal_of_footwell_lights_without_harm_2.jpg"><img src="media/build/get_signal_of_footwell_lights_without_harm_2.jpg" alt="get signal of footwell lights without harm 2" style="width:150px"></a>

Another 2 pin XH connector is for powering an optinal, 5V cooling fan.

There is a micro USB port on the ESP32 board which will later be used to upload the software.
This should be a one time proceedure due to the OTA (wireless) updates.
The micro SDCard is required by TesLight.
It's used to save configuration data, UI files and custom animations of the user.

## Clone or Download the Project Files

Alright, now the point has come where we can start.
At first you should clone or download the project files.
Go to the [project's main page](https://github.com/TheRealKasumi/TesLight) and click the `Code` button.
You can then decide if you want to clone the repository using [Git](https://git-scm.com/) or [download](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip) it as zip file.

![Clone](media/build/clone.jpeg)

As an alternative you can also go to the releases and download one of them.

### Using Git

- Open a terminal in your destination folder
- Run `git clone https://github.com/TheRealKasumi/TesLight.git`
- A folder `TesLight` will be created, containing all project files

### Download as Zip

- Click the [download link](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip)
- Extract the TesLight folder

## Order the PCB and 3D Printed Parts

Like already mentioned in the [part list](part-list.md), the PCB and 3D printed parts can be ordered at [JLCPCB](https://jlcpcb.com/).
But of course you are free to order the parts somewhere else or create them by yourself.

Open their website and click on [Order Now](https://cart.jlcpcb.com/quote) on the right, top of the page.
This should bring you to the configuration page where you can upload the files and configure the PCB.

### Upload Gerber Files

First, the so-called `gerber files` should be uploaded.
These contain all the data required to produce the PCB.
They can be found in the [/pcb/gerber](/pcb/gerber/) folder of the project.

Now two different versions are available.
One version of the board comes with pre-installed DIP headers so that no soldering is required for the board.
The other version has no such header to save space.
Therefore soldering the ESP32 and MPU6050 to the board is required.
Choice is yours

JLCPCB requires you to upload one the zip archives with the gerber files first.
Click on `Add gerber file` on the configuration page and then upload the zip archive of your choice.
Once upload and processing is done, you will see a preview of the PCB.
It should look like this, with the two left images:

![PCB](media/build/pcb.png)

### Board Configuration

Generally, you should be fine with their default configuration but you are free to modify it if you wish.
However, keep in mind that this can have a big effect on the price.
Changing the number of layers and dimensions is not recommended and can lead to problems later.
Also the PCB shouldn't be too thin.
Here the default 1.6mm is just perfect.
For a nice look I would recommend black or purple PCBs, even if the build time is a little higher usually.

![PCB Options](media/build/pcb-options.jpeg)

### Assembly Service

When you are done, configuring the PCB, then it's time to configure the assembly service.
At first you need to chose one of their assembly services.
`Economic` and `Standard` are the available options.

Economic:

- Cheaper
- Collection of boards of different customers are created together
- No borders required
- Takes longer

Standard:

- More expensive
- Your boards are checked, adjusted and produced individually
- Borders and markers are required
- Faster production

Once more, choice is yours but `Economic` is recommended.
Assembly side must always be `top`.
Tooling holes should be added by JLCPCB and confirmation of parts placement is recommended.

![PCB Options](media/build/assembly-options.jpeg)

Once you are done, click on `Confirm`.

### Upload BOM and Placement Files

On the next page you need to upload the BOM (material list) file.
This file contains all components that should be placed on the board.
At the same time you need to upload a so called "pick and place" file.
It is used for the correct component placement.
Make sure to upload the correct files, matching your board version.
Click next to move to the following step.

### Select the parts that should be placed

On this page you should check if all parts are available.
Also you can select whcih components should be installed.
If you want to install a few components on you own, you can unselect them here.
Once done click next and add the items to your shopping cart.

### Upload or Print 3D Files

Ordering works very similar to the steps described above.
Go back to the [order page](https://cart.jlcpcb.com/quote) and select `3D-Printing` at the top of the page.
The 3D files can be found in the [/model](/model/) folder of the project.

In a normal build you will need both, the [TesLight_Case.stl](/model/TesLight_Case.stl) and the [TesLight_Case_Cover.stl](/model/TesLight_Case_Cover.stl).
The all-in-one verion [TesLight_Case_AIO.stl](/model/TesLight_Case_AIO.stl) can be used as an alternative.
When you are using a smaller 30x30x6mm fan, the [TesLight_Fan_Adapter.stl](/model/TesLight_Fan_Adapter.stl) is recommended.
If you plan to use fiber cables and LED injectors, the [TesLight_LED_Case.stl](TesLight_LED_Case.stl) should be ordered as well.
The number depends on the number of light injectors you want to use for the fibre tubes.

Generally the material choice shouldn't be a metal.
The cases must **NOT** be electrically conductive but it is otherwise up to you.
A recommendation is to use a more heat resistant material like ABS or PETG.
Ideally a flame-retardant material is used.

When you plan to print the case yourself, the following setup is recommended:

- Nozzle: 0.4 - 0.6mm
- Layerheight 0.1 - 0.2mm

### Order

Open your shopping cart and make sure it contains all required items.
Then complete the order.

## Assemble the TesLight Controller

### Fully Assembled Boards

Your boards will be produced and assembled by the PCB serivce.
Insert the MPU6050 and ESP32 boards into their place and you are done.
The ESP32 USB port should point to the bottom.
Pay attention to not install it the wrong way around.
The MPU6050 is installed so that the PCB points to the left when the ESP32 USB port is pointing towards you.
That's it, you went for the simple way and you are done😋.

### Manual Assembly

First, make sure you have all required parts.
Go back to the [part list](part-list.md) if you want to check it again.
Prepare your tools, pre-heat the soldering station and then the "fun part" can start😋.

#### Soldering of small Components

I would recommend to start with the small components first.
For the small components I created a short video to show how it can be done.
Especially when you are new to SMD components, it is recommended to watch the video first and follow the steps shown.
Some steps might be missing or are different depending on your board version.
But since you are a tinkerer, I am sure you can do it.

[![TesLight Demo](https://img.youtube.com/vi/Nh1XLG8EGs4/0.jpg)](https://www.youtube.com/watch?v=Nh1XLG8EGs4)

#### Soldering the remaining Components

Generally the remaining components are soldered with the "through-hole technique".
For this cut the legs of the components so that they are just long enough to reach the other side of the PCB.
Insert the component and flip the PCB around.
Always pay attention to the polarity of the components.
Capacitors can potentially explode and hurt your ears and eyes when you insert the in the wrong way around.
Please be careful about that.

Heat up the contects and feed some solder to it until the component is soldered to the board nicely.
It's the same process for all remaining components.
Which exact component to use is printed on the backside of the PCB.

Before you install the ESP32 and MPU6050, you can relatively safely test the voltage regulators.

##### Test the Voltage Regulator

This step is optional but highly recommended.
When you finished building the regulator, please take some time to test it.
If there is a malfunction, it could damage all your other parts.
So it's definitely worth the time.

To test the regulator power the PCB with a voltage above 7V.
It would be good practice to use a low current limit like 250mA.
The polarity and right connections were already shown at the beginning of the guide.
You should then be able to measure a voltage of 5V at the LED outputs.

![Teslight Pinout](media/build/testlight-pinout.png)

Increase the Voltage up to 18V if possible.
The output voltage should stay stable at 5V and the input current should stay very low (usually less than 10mA).
If you have the option to add a load to the 5V side, please test it out.
1A of load is a good test.
If the voltage doesn't break down under load, the build of the regulators was successful.
Once you are done, you can solder all remaining parts to the PCB.

#### ESP32

There is nothing special about installing the ESP32 on the board.
Make sure to push all pins through the holes, move it as far down as possible and cut the legs on the backside.
Solder them to the PCB and you are done.

> Maybe you want to wait with this step and test if everything works first.
> Just push all pins in the holes and let the ESP start up when you uploaded the software.
> Desoldering the ESP32 board is no fun.
> So it might be worth a test first.
> You may find you have bad contacts.
> So it's recommended to push the board a little bit against the PCB to ensure good contact.

When you are done, the TesLight controller should look similar to this (some components might look a little different):
![Controller](media/build/pcb-finished.png)

## Install position in car

It would be best if the antenna of the ESP32 points to the front and the USB port to the back of the vehicle. This way we ensure consistent acceleration directions from the sensor for all users.

## Assemble the LED Injectors (optional for fiber cables)

First of all you should make sure to have the correct number of cases and LEDs.
Also it's recommended that you have an good idea of where you want to place the LEDs, how many and what the wire lengths are.
It will save a lot of trouble to do good planning prior.

Building the "light injectors" is pretty straightforward.
All you need to do is to connect the LEDs with the wires, pay attention to the wire lengths and then push them into their case.
The LEDs are connected like shown in the following picture.
Pay attention to the polarity and everything will work out fine.

![LED Connection](media/planning/led-chain.png)

If you are done building the chains and inserting the LEDs into their case, it's recommended to apply some glue so they can't slip out again.
Fabric tape can also be used to fix them in place.
Also, you can use it to wrap the wires and avoid unwanted noises when driving.
Last step is to solder or crimp the XH 3 pin male connector to the first LED.
The following picture shows the pinout.

![LED XH Pinout](media/build/led-pinout.jpg)

When one "injector chain" is done, it should look something like this.

![LED Injector Chain](media/build/injector-chain.jpg)

As you can see it's also possible to solder a female connector to the first LED and then use an extension cable.
This allows to easily replace the LEDs in case one dies at some point in the future.
But this is entirely optional.

![Extension Cable](media/build/extension-cable.jpg)

## Attach Wires to LED Strips (optional for LED Strips)

This is very much the same procedure as for the LED injectors.
Usually to connections for WS2812 LED strips is as follows.

![LED Connection](media/planning/led-chain.png)

Make sure to correctly install the cables and connectors.
Once again, this is how the connections should look at the end.

![LED XH Pinout](media/build/led-pinout.jpg)

## Attach Wires to Light Bars (optional for Light Bars)

Since the light bars also only use an LED strip internally, the procedure is the same as for the LED strips above.
However, depending on the light bars, the polarity might be different.
It is highly recommended, to open the light bar at the cable end to check on the polarity.
We already encountered the same type of light bars with different cable colors for the connections.
So please be careful and don't trust the cable colours.
How to check: The middle cable (green?) should always be data. Then plug one cable to GND and connect the other one with a 220 Ohm resistor to 5 V. If the strip turns on, then everything is good. If not, try the other way around. There should be no current flowing through the resistor high enough to damage the strip.

At the end the 3 pin XH connectors must have the following pinout.

![LED XH Pinout](media/build/led-pinout.jpg)

## Upload the Software

### TesLight Controller

Uploading the software is the final step before you can test your work.
Please install the following software and extension for the upload procedure:

- [Git](https://git-scm.com/downloads)
- [VS Code](https://code.visualstudio.com/download)
- [PlatformIO](https://platformio.org/install/ide?install=vscode)
- [NodeJS](https://nodejs.org)
- [NPM (comes with NodeJS)](https://www.npmjs.com/)

Start VS Code and then open the [mcu](/mcu/) folder of the project.
You can do so by clicking `File` -> `Open Folder...`.
On the left all files inside the opened folder should be listed.
Open the file `platformio.ini`.
In the configuration you can see two attributes, `upload_port` and `monitor_port`.

![plattformio.ini](media/build/plattformio-ini.jpeg)

Connect the TesLight controller to your computer via a micro USB cable.
Now you need to find out which port the controller was assigned to.
The recommendation is to use the device manager when you are using Windows.
Press the Windows key + R and type `devmgmt.msc`.
Then hit enter.
The device manager will start up and you should be able to identify the port of the controller.
Go back to the `platformio.ini` and adjust the port.
For Linux and Unix users I will not even explain this step.
They should know how this works, right😛?
Once changed, you can save the file and close it.

Now there is one more configuration step before you are ready to upload the code to the board.
Please navigate to [include/configuration](/mcu/include/configuration) and open the file [SystemConfiguration.h](/mcu/include/configuration/SystemConfiguration.h).

If you decided to only install one voltage regulator, please do the following changes:

- `#define REGULATOR_COUNT 1`
- `#define REGULATOR_ZONE_MAPPING {{13, 0}, {14, 0}, {15, 0}, {16, 0}, {17,0}, {21, 0}, {22, 0}, {25, 0}}`

Also depending on the board version, the following changes are required.

Board version 1.0 (@luap):

- `#define REGULATOR_COUNT 1`
- `#define REGULATOR_ZONE_MAPPING {{13, 0}, {14, 0}, {15, 0}, {16, 0}, {17,0}, {21, 0}, {22, 0}, {25, 0}}`

Board version 2.0 (@PhilippDen):

- `#define LED_DEFAULT_OUTPUT_PINS {13, 17, 14, 21, 15, 22, 16, 25}`
- `#define REGULATOR_ZONE_MAPPING {{13, 0}, {17, 1}, {14, 0}, {21, 1}, {15, 0}, {22, 1}, {16, 0}, {25, 1}}`

Save the files and then click the upload button in the bottom left.

![Upload Button](media/build/upload-button.jpeg)

The code should now be compiled and uploaded to the board.
In case the upload fails, hold down the `boot` button on the ESP32 until the upload started.

Now open the serial monitor by clicking the "plug" button, near to the upload button.

![Serial Button](media/build/monitor-button.jpeg)

You should see the TesLight controller starting up, but then stops with `Failed to initialize SD card`.

```shell
████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗
╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║
   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║
   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║
   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
Firmware version 1.0.0

00:00:00:059 [INFO] (src/main.cpp) (setup) (331): Initialize SD card.
[   364][E][sd_diskio.cpp:199] sdCommand(): Card Failed! cmd: 0x00
[   364][E][sd_diskio.cpp:802] sdcard_mount(): f_mount failed: (3) The physical drive cannot work
[   666][E][sd_diskio.cpp:199] sdCommand(): Card Failed! cmd: 0x00
00:00:00:666 [ERROR] (src/main.cpp) (setup) (338): Failed to initialize SD card.
```

### Prepare and insert the Micro SD Card

TesLight **requires** a `FAT` or `FAT32` formatted micro SD card.
At least 128MB of storage is recommended for upcoming features.
Well... we live in year 2022.
So just throw a 4GB+ card inside and be done.
The SD card is used to store your settings, animations, logs and the browser based UI.
Also, it can be used to save and play `fseq` files, with fully customized animations.

Since the controller will create the settings and log data automatically, only the UI files must be copied to the SD card.
These can be found in the [web-app](/web-app/) folder.
But not so fast, there is a mandatory build step first.
Open a terminal of your choice and navigate to the mentioned folder.
Once you are there, please run the following commands.

```sh
npm install
npm run build
```

This can take a moment and should create a [build](/web-app/build/) folder.
Create a new folder called `web-app` in the root of your SD card.
Copy the content of [build](/web-app/build/) into the newly created folder.
Eject the SD card from your computer and insert it into the micro SD card slot of the TesLight controller.

## Let's test it

Now that everything is ready, it's time for a short test.
Connect the board back to your computer and fire up the serial monitor.
If required, press the reset button on the ESP32 board.
If everything works, you should see the following (or similar) output:

```shell
████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗
╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║
   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║
   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║
   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
Firmware version 1.0.0

00:00:00:059 [INFO] (src/main.cpp) (setup) (331): Initialize SD card.
00:00:00:071 [INFO] (src/main.cpp) (setup) (334): SD card initialized.
00:00:00:072 [INFO] (src/main.cpp) (setup) (342): Switching to SD card logger.
00:00:00:593 [INFO] (src/main.cpp) (setup) (346): Switched to SD card logger.
00:00:00:645 [INFO] (src/main.cpp) (setup) (354): Check if system update is available.
[   674][E][vfs_api.cpp:104] open(): /sd/update/update.tup does not exist, no permits for creation
00:00:00:675 [INFO] (src/main.cpp) (setup) (362): No available system update found.
00:00:00:832 [INFO] (src/main.cpp) (setup) (365): Initialize, load and save configuration.
[  1045][E][vfs_api.cpp:104] open(): /sd/config.tli does not exist, no permits for creation
00:00:01:046 [ERROR] (src/util/BinaryFile.cpp) (open) (46): Failed to open binary file.
00:00:01:262 [ERROR] (src/configuration/Configuration.cpp) (load) (158): Failed to load configuration file: /config.tli
00:00:01:289 [ERROR] (src/main.cpp) (initializeConfiguration) (140): Failed to initialize runtime configuration from MicroSD card.
00:00:01:556 [WARN] (src/main.cpp) (setup) (372): Failed to load configuration. Loading defaults and continue.
00:00:01:825 [INFO] (src/main.cpp) (setup) (376): Updating log level from configuration.
00:00:02:049 [INFO] (src/main.cpp) (setup) (379): Log level updated from configuration.
00:00:02:071 [INFO] (src/main.cpp) (setup) (381): Initialize LED Manager.
00:00:02:093 [INFO] (src/main.cpp) (setup) (383): LED Manager initialized.
00:00:02:121 [INFO] (src/main.cpp) (setup) (385): Initialize light sensor.
00:00:02:145 [INFO] (src/main.cpp) (setup) (387): Light sensor initialized.
00:00:02:167 [INFO] (src/main.cpp) (setup) (389): Initialize motion sensor.
00:00:02:205 [INFO] (src/main.cpp) (setup) (392): Motion sensor initialized.
00:00:02:228 [INFO] (src/main.cpp) (setup) (399): Initialize WiFiManager.
00:00:02:250 [INFO] (src/wifi/WiFiManager.cpp) (WiFiManager) (16): Initializing WiFi manager.
00:00:02:402 [INFO] (src/wifi/WiFiManager.cpp) (WiFiManager) (18): WiFi manager initialized.
00:00:02:427 [INFO] (src/main.cpp) (setup) (401): WiFi manager initialized.
00:00:02:449 [INFO] (src/main.cpp) (setup) (403): Starting Webserver.
00:00:02:471 [INFO] (src/server/WebServerManager.cpp) (WebServerManager) (20): Starting webserver on port 80.
00:00:02:495 [INFO] (src/server/WebServerManager.cpp) (WebServerManager) (25): Webserver running.
00:00:02:521 [INFO] (src/main.cpp) (setup) (405): Webserver started on port 80.
00:00:02:549 [INFO] (src/main.cpp) (setup) (407): Initializing REST api.
00:00:02:588 [INFO] (src/main.cpp) (setup) (409): REST api initialized.
00:00:02:610 [INFO] (src/main.cpp) (setup) (411): Load LEDs and animators from configuration using the LED Manager.
00:00:02:661 [INFO] (src/main.cpp) (setup) (414): LEDs and animators loaded.
00:00:02:683 [INFO] (src/main.cpp) (setup) (421): Creating to WiFi network.
00:00:02:707 [INFO] (src/wifi/WiFiManager.cpp) (startAccessPoint) (40): Starting WiFi access point with ssid 'TesLight' and password 'TesLightPW' on channel 1.
00:00:02:737 [INFO] (src/wifi/WiFiManager.cpp) (startAccessPoint) (53): Starting WiFi access point.
00:00:02:859 [INFO] (src/wifi/WiFiManager.cpp) (startAccessPoint) (56): Configuring WiFi access point.
00:00:02:882 [INFO] (src/wifi/WiFiManager.cpp) (startAccessPoint) (61): WiFi access point started. Listening on: 192.168.4.1
00:00:02:905 [INFO] (src/main.cpp) (setup) (424): WiFi Network created.
00:00:02:934 [INFO] (src/main.cpp) (setup) (431): Initialize timers.
00:00:02:955 [INFO] (src/main.cpp) (initializeTimers) (243): Initialize timers.
00:00:02:977 [INFO] (src/main.cpp) (setup) (433): Timers initialized.
00:00:02:999 [INFO] (src/main.cpp) (setup) (435): TesLight initialized successfully, going into work mode.
00:00:12:983 [INFO] (src/main.cpp) (loop) (488): LEDs running at an average of 60 FPS
```

When this is the case, you successfully built the TesLight controller.
Success!
Now search for a WiFi network called `TesLight`.
Connect to it using the default password `TesLightPW` and open up `192.168.4.1` in your browser.
Now the TesLight UI should show up.
Bookmark this page so that you will not forget it.
Alternatively, you can also try to "install" the UI on your device.
Since it is a ReactJS based UI, some devices offer support for this.

From here you can control TesLight using the browser based UI.
But don't forget to connect to the `TesLight` WiFi first.
The ssid and password can be changed in the settings.
I am sure you will figure out how the UI works :) .
