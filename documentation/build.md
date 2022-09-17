# Build Guide

Before starting with this guide, make sure to read the [planning](planning.md) document and [part list](part-list.md) first.
They contain important information and consideration before building the project.
If you start to build the project blindly, you might run into issues later.

## General Knowledge about the Hardware

Let's start with some general knowledge about the hardware.

At first the TesLight controller is based on a ESP32 microcontroller board, which is placed on a custom PCB.
The ESP32 contains two Tensilica-LX6 cores, running at 80MHz or 240MHz.
They come with 512kB of SRAM and 4MB of flash memory.
Also, they have onboard WiFi (802.11bgn) as well as Bluetooth (classic an LE) and hardware support for SPI, I2C, CAN, UART, etc. could be used in the future.
Generally the board operates at the 3.3V level.
It has an onboard voltage regulator which will provide these 3.3V from the 5V, provided by the TesLight PCB.
If you are planing to do customizations please keep in mind that all pins can only handle 3.3V and a very limited current of a few milliamps.

The TesLight PCB contains a few active and passive components.
In the first place it _can_ have a voltage regulator, providing stable 5V at max 3A to the ESP32 and your LEDs.
It can handle an input voltage of up to 45V according to the [specs](https://www.ti.com/lit/ds/symlink/lm2596.pdf?ts=1663311548260&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FLM2596) of the regulator.
But keep in mind that this is an absolute maximum rating, which should never be reached.

Also the PCB contains a MPU6050 motion sensor, right under the ESP32 board.
This will later be used to build animations depending on acceleration and rotation data.
It is suitable for up to 16g's but TesLight is using a limit of 2g's currently.
If someone can prove to me that 2g's are not enough in a Tesla, then I will congratulate this person and we can talk about setting it to 4g in the software😋.
Hidden challenge?
I... I would never challenge you to do stupid things...

![Girl Shy](media/fun/girl-shy.gif)

Umm... we better continue... So... where did we stop?

There also is a voltage divider, used to lower the voltage of the sensor pin to a level the ESP32 can handle.
This pin can be connected to your cars existing ambient light to give TesLight a signal when to turn on or off.
Last there are some resistors to properly drive the WS2812 type LEDs and protect the ESP32 board.

Like mentioned the used LED type is WS2812 or compatible.
These are LED chips with an on board controller capable of receiving a digital, 24 bits color value.
When they are put in series, they operate as a shif register.
TesLight is able to drive a variable number of LEDs in series (up to a few hundred).
The LEDs operate at 5V and **only 5V**.
Never connect them directly to the 12V of your car.
If the onboard voltage regulator can not deliver enough current, you need to use an external 5V regulator.

The following picture shows the pinout of the finished board.

![Teslight Pinout](media/build/testlight-pinout.png)

As you can see there are 6 connectors for the LEDs.
These outputs a regulated 5V and data signal suitable for the WS2812 type LEDs.
Canncel 1 is the left, bottom connector.
Channel 2, 3, 4 are in the upward direction.
Channel 5 and 6 are on the right from top to bottom.

| Channel   | Mapped to         |
| --------- | ----------------- |
| Channel 1 | Dash              |
| Channel 2 | Center console    |
| Channel 3 | Front, left door  |
| Channel 4 | Front, right door |
| Channel 5 | Rear, left door   |
| Channel 6 | Rear, right door  |

At the bottom of the PCB there are 2 connectors, a micro USB port and a micros SD card slot.

The 4 pin XH connector is for powering the board.
When you use the onboard regulator, this can be up to 45V (absolute maximum rating).
When an external regulator is used, the jumper J1 has to be bridged and the input voltage must be stable 5V.
Never bridge J1 when the onboard regulator is used.
This will destroy your hardware.

The 3 pin XH connector can be used as voltage sensor.
It can be used to measure the voltage of your cars ambient light and give TesLight a signal to turn on or off.
The pin can be used for analog voltages and also PWM signal with a relatively high frequency.
Low frequency PWM signals can lead to measurement errors.
It is recommended to not measure voltages above 20V.
However, due to the over voltage protection it's safe to exceed this limit for a short amount of time if really necessary.
Keep in mind that according to the diagram above, the right pin is connected to ground.
Never connect it to the positive wire of your ambient light.
If you are sure that there is a good ground connection, which is usually the case, there is no need to use it.
One last thing to keep in mind is that only positive voltages can be measured.

Also, there is a micro USB port on the ESP32 board.
This one will later be used to upload the software.
The micro SD card slot is used for a mandatory micro SD card.
It's used to save data and UI files, required by TesLight.

## Clone or Download the Project Files

Before we start, you should clone or download the project files.
Go to the [main page](https://github.com/TheRealKasumi/TesLight) and click the `Code` button.
You can then decide if you want to clone the repository using [Git](https://git-scm.com/) or [download](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip) it as zip file.

![Clone](media/build/clone.jpeg)

### Using Git

-  Open a terminal in your destination folder
-  Run `git clone https://github.com/TheRealKasumi/TesLight.git`
-  A folder `TesLight` should be created, containing all project files

### Download as Zip

-  Click the [download link](https://github.com/TheRealKasumi/TesLight/archive/refs/heads/main.zip)
-  Extract the TesLight folder

## Order the PCB and 3D Printed Parts

Like already mentioned in the [part list](part-list.md), the PCB and 3D printed parts can be ordered at [JLCPCB](https://jlcpcb.com/).
But of course you are free to order the parts somewhere else or create them by yourself.

Open their website and click on [Order Now](https://cart.jlcpcb.com/quote) on the right, top of the page.
This should bring you to the configuration page where you can upload the files and configure the PCB.

### Upload Gerber Files

At first the so called `gerber files` should be uploaded.
These contain all the data required to produce the PCB.
They can be found in the [/pcb/gerber](/pcb/gerber/) folder of the project.
JLCPCB requires you to upload all of them as a zip file.
Create a new zip file and put all files from the `gerber` folder into the root of the zip file.
Click on `Add gerber file` on the configuration page and then upload the newly created zip file.
Once the upload and processing is done, you will see a preview of the PCB. It should look like this:

![PCB](media/build/pcb.png)

### Configuration

Generally, you should be fine with their default configuration.
But you are free to modify it if you wish.
However, keep in mind that this can have a big impact on the price.
Changing the number of layers and dimensions is not recommended and can lead to problems later.
Also the PCB shouldn't be too thin.
Here the default 1.6mm are just perfect.
When you are done, add the PCB to the shopping cart.

![PCB Options](media/build/pcb-options.jpeg)

### Upload 3D Files

This works very similar to the steps described above.
Go back to the [order page](https://cart.jlcpcb.com/quote) and select `3D-Printing` at the top of the page.
The 3D files can be found in the [/models](/models/) folder of the project.
You will need one print of the [mcu_case_all.stl](/models/mcu_case_all.stl) which is the case for the TesLight controller.
Also, depending on the number of light injectors for the fibre tubes, you will need the [led-case.stl](/models/led-case.stl).
The material choice is fully up to you.
But it shouldn't be metal so that it is **NOT** electrically conductive.
When you are done, add the 3D prints to the shopping cart.

### Order

Go to your shopping cart and make sure it contains all the items.
Then continue their instructions to finish the order.

## Assemble the TesLight Controller

First, make sure you got the correct and all required parts.
Go back to the [part list](part-list.md) if you want to check it again.
Prepare your tools, pre-heat the soldering station and the fun part can start😋.

![pcb-parts](media/build/pcb-parts.jpeg)

Don't worry, it's not as hard as it looks first.

### Voltage Regulator

Building the voltage regulator is optional and only suitable for a current of max. 3A.
For higher currents you need to use an external 5V regulator with a higher power limit.
See [planning](planning.md#power-consumption) for more details or if you are unsure.

If you want to use an external regulator, please bridge the jumper `J1` with a bit of solder.
You can then skip to the last point.

When using the integrated regulator, this should be the first step.
This allows you to test it properly afterwards without destroying other components when you make a mistake.

**Never bridge the jumper `J1` when using the internal regulator!**

1. As a first step its's recommended to put the inductor `I1` in place and solder it to the PCB.
   For this put a thin layer of solder on the pads and the legs of the inductor.
   Position the inductor and heat up the solder pads.
   Feed it with some more solder and press down the inductor until it soldered together.
   Repeat the same for the pad on the other side.

2. Place the diode on the spot marked with `D2`.
   Proceed in the same way as above.
   Since it's a diode, the polarity matters.
   Make sure to put it in the right direction.
   You should be able to see a white line on the diode.
   It must be aligned with the white line on the PCB.

3. Continue by inserting the capacitor into `C1`.
   Here it's important to check the polarity as well.
   Otherwise the capacitor might explode.
   You definitely don't want this to happen to protect your eyes and ears.
   If it's inserted correctly, cut its legs as short as possible on the backside.
   Use some solder to connect it properly.

4. Next is the `LM2596`, right next the inductor.
   Obviously polarity matters again.
   Make sure to have the metal part pointing to the right/ESP32/SD/MPU6050.
   It's also marked on the PCB.
   Push all 5 legs through the holes and cut them as short as possible on the backside.
   Solder them all in place.
   Make sure to have good connections and not bridge the contacts.
   They are close to each other, so pay attention.

5. As a last step it's time to solder the fuse to the PCB.
   Ideally using a fuse holder.
   It's for your own safety and to avoid a fire in case of issues and faults.
   So I don't want you do to anything stupid like this: <br> ![Fuse Replacement](media/build/fuse-replacement.jpg)<br>
   You have been warned.
   For the internal regulator please use a fuse with a maximum current of 2A.
   When using external regulators make sure to pick a suitable one.
   If your regulator doesn't come with a fuse on the 12V side, you should definitely add one.
   The PCB should be good for around 8-10A.
   However, make sure that your connectors are high quality and can deal with the current.
   If even higher currents are required, please bypass the PCB and connect the LED's directly to your regulator.
   However, don't forget to provide 5V to the TesLight controller and have a shared ground connection.

#### Test the Voltage Regulator

This step is optional but highly recommended.
When you finished building the regulator, please take some time to test it.
If there is a malfunction, it could damage all your parts.
So it's definitely worth the time.

To test the regulator power the PCB with a voltage above 6-7V.
The polarity and right connections were already shown at the beginning of the guide.
You should then be able to measure a voltage of 5V at the marked connections:

![PCB 5V](media/build/pcb-5v.jpg)

Increase the Voltage up to 20-30V.
The output voltage should stay stable at 5V.
If you have the option to add a load to the 5V side, please test it out.
1A of load is a good test.
The voltage should stay at stable 5V.
If that is the case, the build was successful.

### MPU6050 (Motion Sensor)

This part is the easiest and can be done in two ways.
Place the MPU6050 (Motion Sensor) in the market place on the PCB.
Make sure to align it precisely so that the holes and contacts line up.
Now it has to be soldered to the PCB.
This can be done directly using the holes or by using small wires.
The choice is yours.
Just make sure the sensor can not move and is mounted as low and flat as possible.
The space is required later.

### Micro SD Card Holder

At first make sure that you bought a fitting card holder by placing it on the PCB and trying to align the contacts.
Some card holder has a tiny plastic pin at the bottom.
These can simply be cut so it lays flat on the PBC.
If everything seems to fit well, move the card holder aside.
Use your soldering station to put a **very thin** layer of solder onto the contact plates.
This will make the soldering process easier.
Don't forget the 4 ground contacts around the card holder.

Bring the card holder back in place and make sure it's aligned precisely.
Then start with the 4 ground contacts so that it can't move anymore.
Now continue with the remaining contacs.
It's recommended to heat them up and then do a stroking movement away from the card holder.
This way you usually get nice and clean connections.
Make sure there aren't any bridged or unconnected pins left.
This step is very important because otherwise the controller might not work later and you have to decide the ESP32 board again.
Save yourself some pain and make sure everything is fine.

### SMD Resistors

Soldering the SMD resistors is the trickiest part.
It requires a bit of patience and focus.
A total of 8 resistors have to be soldered.
Also, there are 4 optional ones which are not required unless you run into trouble.
Start with the resistors R1 to R6.
These should have a value between 220Ω and 330Ω.
Put a very thin layer of solder onto the pads.
Then align one resistor with the pads using a tweezer.
Push it down, so that it will not slip away.
Heat up one end and press it down to the pad.
Once it got hot enough, it should stick to the pad now.
Continue the same way on the other side.
Repeat until all resistors are placed.

Then move on with the two missing resistors R7 and R8.
These should have a value of 4.7kΩ for R7 and 1kΩ for R8.
Proceed as previously.

### Zener Diode

Insert the Zener diode in place and cut the legs as short as possible on the backside.
Pay attention to the polarity.
Like for the other diodes, there should be one side marked with a line.
This line has to be aligned with the PCB.
Solder it in place and you are done.

### Connectors

Same procedure once more.
Put all connectors in place and cut the legs on the backside.
Make sure to put them in the right way, it's marked on the PCB.
Solder them in place.

### ESP32

There is nothing special about installing the ESP32 on the board.
Make sure to push all pins through the holes, move it as far down as possible and cut the legs on the backside.
Solder them to the PCB and you are done.

> Maybe you want to wait with this step and test if everything works first.
> Just push all pins in the holes and let the ESP start up when you uploaded the software.
> Desoldering the ESP32 board is no fun.
> So it might be worth a test first.
> It might be that you have bad contacts this way.
> So it's recommended to push the board a little bit against the PCB to ensure good contact.

When you are done, the TesLight controller should look similar to this (some components might look a little different):
![Controller](media/build/pcb-finished.jpg)

## Assemble the LEDs

First of all you should make sure to have the correct number of cases and LEDs.
Also it's recommended that you have an exact plan where you want to place the LEDs, how many of them and what the wire lengths are.
It will save a lot of trouble to do a good planing beforehand.

Building the "light injectors" is pretty straightforward.
All you need to do is to connect the LEDs with the wires, pay attention to the wire lengths and then push them into their case.
The LED's are connected like shown in the following picture.
Pay attention to the polarity and everything will work out fine.

![LED Connection](media/planning/led-chain.png)

If you are done building the chains and inserting the LEDs into their case, it's recommended to apply some glue so they can't slip out again.
Fabric tape can also be used to fix them in place.
Also, you can use it to wrap the wires and avoid unwanted noises when driving.
Last step is to solder or crimp the XH 3 pin male connector to the first LED.
The following picture shows the pinout.

![LED XH Pinout](media/build/led-pinout.jpg)

When one "injector chain" is done, it could look like this.

![LED Injector Chain](media/build/injector-chain.jpg)

As you can see it's also possible to solder a female connector to the first LED and then use an extension cable.
This allows to easily replace the LEDs in case one dies at some point in the future.
But this is fully optional.

![Extension Cable](media/build/extension-cable.jpg)

## Upload the Software

### TesLight Controller

Uploading the software is the final step before you can finally test your work.
Please install the following software and extension for the upload procedure:

-  [VS Code](https://code.visualstudio.com/download)
-  [PlatformIO](https://platformio.org/install/ide?install=vscode)

Start VS Code and then open the [mcu](/mcu/) folder of the project.
You can do so by clicking `File` -> `Open Folder...`.
On the left all files inside the opened folder should be listed.
Open the file `platformio.ini`.
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

You should see the TesLight controller starting up, but then stopping with `Failed to initialize SD card`.

```
████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗
╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║
   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║
   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║
   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
Firmware version 0.0.1 (beta)

00:00:00:049 [INFO] (main.cpp:setup): Initialize SD card...
[E][sd_diskio.cpp:194] sdCommand(): Card Failed! cmd: 0x00
[E][sd_diskio.cpp:775] sdcard_mount(): f_mount failed: (3) The physical drive cannot work
[E][sd_diskio.cpp:194] sdCommand(): Card Failed! cmd: 0x00
00:00:00:659 [ERROR] (main.cpp:setup): Failed to initialize SD card.
```

### Prepare and insert the Micro SD Card

TesLight **requires** a `FAT` or `FAT32` formatted micro SD card.
At least 512MB of storage is recommended for upcoming features.
The SD card is used to store your settings, animations, logs and the browser based UI.
In the future, it might be possible to create fully customized animations.
These will also be stored on the SD card.

Since the controller will create the settings and log data automatically, only the UI files must be copied to the SD card.
The UI comes as a single html file with all required data embedded into it.
It can be found in the [web-app](/web-app/) folder and is called [index.html.min.html](/web-app/index.html.min.html).
Create a new folder called `web-app` in the root of your SD card.
Copy the [index.html.min.html](/web-app/index.html.min.html) into the newly created folder and rename it to `index.html`.
Eject the SD card from your computer and insert it into the micros SD card slot of the TesLight controller.

## Let's test it!

Now that everything is ready, it's time for a short test.
Connect the board back to your computer and fire up the serial monitor.
If everything works, you should see the following (or similar) output:

```
████████╗███████╗███████╗██╗     ██╗ ██████╗ ██╗  ██╗████████╗
╚══██╔══╝██╔════╝██╔════╝██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
   ██║   █████╗  ███████╗██║     ██║██║  ███╗███████║   ██║
   ██║   ██╔══╝  ╚════██║██║     ██║██║   ██║██╔══██║   ██║
   ██║   ███████╗███████║███████╗██║╚██████╔╝██║  ██║   ██║
   ╚═╝   ╚══════╝╚══════╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝
Firmware version 0.0.1 (beta)

00:00:00:049 [INFO] (main.cpp:setup): Initialize SD card...
00:00:00:060 [INFO] (main.cpp:setup): SD card initialized.
00:00:00:061 [INFO] (main.cpp:setup): Switching to SD card logger...
␀00:00:00:570 [INFO] (main.cpp:setup): Switched to SD card logger.
00:00:00:592 [INFO] (main.cpp:setup): Initialize, load and save configuration...
00:00:00:616 [INFO] (Configuration.cpp:load): Loading configuration from file "/config.tli"...
00:00:00:645 [INFO] (Configuration.cpp:save): Saving configuration to file "/config.tli"...
00:00:00:742 [INFO] (Configuration.cpp:save): Configuration saved.
00:00:00:765 [INFO] (main.cpp:setup): Runtime configuration loaded.
00:00:00:785 [INFO] (main.cpp:setup): Initialize LED drivers and animator...
00:00:00:807 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 70 LEDs using pin 13 and rmt channel 0.
00:00:00:828 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:00:850 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 4 LEDs using pin 14 and rmt channel 1.
00:00:00:882 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:00:909 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 2 LEDs using pin 15 and rmt channel 2.
00:00:00:934 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:00:961 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 4 LEDs using pin 16 and rmt channel 3.
00:00:00:987 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:01:014 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 4 LEDs using pin 17 and rmt channel 4.
00:00:01:064 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:01:088 [INFO] (LedDriver.cpp:begin): Initialize LED driver for 4 LEDs using pin 21 and rmt channel 5.
00:00:01:108 [INFO] (LedDriver.cpp:begin): LED driver initialized successfully.
00:00:01:129 [INFO] (main.cpp:setup): LED drivers and animators initialized.
00:00:01:149 [INFO] (main.cpp:setup): Initialize light sensor...
00:00:01:169 [INFO] (main.cpp:setup): Light sensor initialized.
00:00:01:195 [INFO] (main.cpp:setup): Initialize motion sensor...
00:00:01:215 [INFO] (MotionSensor.cpp:MotionSensor): Initializing Motion Sensor properties.
00:00:01:235 [INFO] (MotionSensor.cpp:begin): Initializing Initializing Motion Sensor.
00:00:01:258 [INFO] (MotionSensor.cpp:begin): Initially filling buffer.
00:00:01:288 [INFO] (MotionSensor.cpp:begin): Calculating sensor offsets.
00:00:01:308 [INFO] (MotionSensor.cpp:begin): Motion sensor initialized successfully.
00:00:01:334 [INFO] (main.cpp:setup): Motion sensor initialized.
00:00:01:353 [INFO] (main.cpp:setup): Initialize WiFiManager...
00:00:01:373 [INFO] (WebServer.cpp:WebServer): Initializing WiFi manager.
00:00:01:516 [INFO] (WebServer.cpp:WebServer): WiFi manager initialized.
00:00:01:539 [INFO] (main.cpp:setup): WiFi manager initialized.
00:00:01:559 [INFO] (main.cpp:setup): Creating to WiFi network...
00:00:01:580 [INFO] (WebServer.cpp:startAccessPoint): Starting WiFi access point with ssid 'TesLight' and password 'TesLight' on channel 1.
00:00:01:608 [INFO] (WebServer.cpp:startAccessPoint): WiFi access point started. Listening on: 192.168.4.1
00:00:01:629 [INFO] (main.cpp:setup): WiFi Network created.
00:00:01:649 [INFO] (main.cpp:setup): Starting Webserver...
00:00:01:669 [INFO] (WebServer.cpp:WebServer): Starting webserver on port 80.
00:00:01:691 [INFO] (WebServer.cpp:WebServer): Webserver running.
00:00:01:712 [INFO] (main.cpp:setup): Webserver started on port 80.
00:00:01:732 [INFO] (main.cpp:setup): Initializing REST api...
00:00:01:761 [INFO] (main.cpp:setup): REST api initialized.
00:00:01:781 [INFO] (main.cpp:setup): TesLight initialized successfully, going into work mode.
00:00:10:000 [INFO] (main.cpp:loop): LEDs running at an average of 60 FPS
```

When this is the case, you successfully built the TesLight controller.
Now search for a WiFi network called `TesLight`.
Connect to it using the default password `TesLight` and open up `192.168.4.1` in your browser.
Now the TesLight UI should show up.
Bookmark this page so that you will not forget about it.
From here you can control your LED's.
But don't forget to connect to the `TesLight` WiFi first.
The password can be changed in the settings.
