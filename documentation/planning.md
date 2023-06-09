# Planning

It is recommended to read through this short document before you start the project.
There are some important considerations and decisions you need to make first.
This will save you some trouble and make it easier to buy the right parts.
It will also help when building the project later.

## LED Configuration

The LED configuration is the most important planning step.
Here you need to consider which LEDs you want to use in which place, how they are attached, the number you want to install in your car and how to wire them.
This is personal preference and will influence the part list and building steps.
Also, it will affect how much work you have to put into the project.
So let's start with some important information that hopefully will answer some of your questions and help you with the planning.

### NikoLight, Channels, Zones and LEDs

Alright, let's start with first thing first.
NikoLight currently has a total of **8 LED channels**.
These channels are for the individual zones in your car.
This means that there are **8 different zones** that can be controlled independently.
How you spread them in your car is completely up to you.
You also don't have to use all of them, but you can if you want to.
An example could look like this:

| Channel/Zone | Used for            |
| ------------ | ------------------- |
| 1            | Dashboard           |
| 2            | Center console      |
| 3            | Left, front door    |
| 4            | Right, front door   |
| 5            | Rear, left door     |
| 6            | Rear, right door    |
| 7            | Footwell lighting 1 |
| 8            | Footwell lighting 2 |

NikoLight is using digital LEDs, so that it control each LED individually.
The LED type used is **WS2812** and all LEDs accepting the same protocol at a 5V input.
Other LED types might not work as expected.

Each zone can have a **variable number of LEDs**.
This number can reach from **0 up to a few hundreds**.
The limiting factors for this are: available memory, time required for data transmission and processing power of the microcontroller. In practice this means a zone should not have more than 200 LEDs.

Every single LED in a zone is **addressable**.
This means that NikoLight can "talk" to the individual LEDs and set a 24 bit color value for each.
You can compare this to the pixels of your monitor.
This is achieved by **chaining the LEDs**, like it is done on LED strips.
So each LED per zone will get a number from 0 to n, where n is the last LED in the line.

![LED Chain](media/planning/led-chain.png)

### Luminant Types

Currently there are three different types of luminants available that you can choose from. They are all fully supported by NikoLight, but not each luminant is suitable for each installation location.

#### Basic WS2812 LED strip

This is the most widely availble type of luminant - a series of WS2812 LEDs mounted in series on a thin, flexible PCB strip.
These strips are available in various lenghts, widths and density (count of LEDs per meter).
Possible installation locations for the LED strip are the dashboard and the foot wells.
A density of around 60 LEDs/meter is a good compromise between cost/power consumption and good looking light (effects).

##### Advantages

* easy to obtain in various configurations
* supports all light effects since many LEDs are distributed across the strip

##### Disadvantages

* can only be installed in locations where it is not directly visible

#### Fiber tube with WS2812 LEDs as light injectors

This type of luminant is a small flexible tube with a tab on it's side. WS2812 LEDs can be placed on one or (better) both ends of the tube to inject light into the tube and make it glow (fairly) evenly along its entire length.
The fiber tube is suitable for all installation locations in the Tesla Model 3/Y with the exception of the footwell lighting. As of now it is also the only lumimant that can be installed in the 2021 or later center console of Model 3/Y and around the seats.

##### Advantages of fiber tube

* low cost
* low power consumption
* easy, flexible installation

##### Disadvantages of fiber tube

* you can have at most 2 colors per tube segment, one injected from each side, which greatly limits the effects that you can display with it
* because of it's white(-ish) color the fiber tube visually stands out a bit againts the dark interior in daylight which can look less professional depending on your taste

#### Light bar with WS2812 LEDs

The light bar is like a hybrid of the light strip and fiber tube. It consists of a very thin WS2812 LED strip mounted onto an also very thin and flexible light distribution bar that is additionally covered in a black, translucent tape.
The density of the currently available light bars is very high with around 130 LEDs/meter. Possible installation locations are the dashboard, the door panels and the pre-2021 center console.

##### Advantages of light bar

* can be installed almost anywhere in the care, where a fiber tube can be installed as well
* supports all light effects since many LEDs are distributed across the light bar
* not very visible in daylight when turned off because it is wrapped in black tape

##### Disadvantages of light bar

* more expensive
* not as easy to obtain because there are not many shops that carry it (especially in Europe)
* only available in lengths up to 1 meter (but you can extend it by soldering 2 bars together)
* consumes a lot of power because of the high LED count
* can also not extend it very far beyond 1 meter before hitting the recommended 200 LED/channel limit

#### Recommendation

The recommendation is to use a normal LED strip under the dashboard for ease of installation, cost and power consumption.
As an alternative it is also possible to use a light bar for the dashboard as well, but it requires extra effort to solder two light bars together to achieved the required length.
For the 2021 and later models use of a light bar in the dashboard and upper door panels results in a nice continuous line of light from door to door.

The fiber tubes are working well for the center console or around the seats.

For the doors, the light bars turned out to be very nice looking and be less noticeable when turned off.
The choice here is fully up to you.
All parts can be found on the [part list](part-list.md).

### Zones

As explained above, NikoLight supports up to **8 different zones**.
How you select these zones is up to you, but keep in mind that you must be able to install and wire the LEDs in the zones.

Since NikoLight was designed with the Tesla Model 3 and Model Y in mind, there is a recommended configuration of the zones.
This is the same configuration as shown in the following video (click to open).

[![NikoLight Demo](https://img.youtube.com/vi/_N5h1IViB-E/0.jpg)](https://www.youtube.com/watch?v=_N5h1IViB-E)

The following table assumes that the NikoLight controller is placed below the center console for easy access and wiring. (It would be best if the antenna of the ESP32 points to the front and the USB port to the back of the vehicle. This way we ensure consistent acceleration directions from the sensor for all users.)

| Zone              | LEDs | Type                      | Cable Length | Physical Channel |
| ----------------- | ---- | ------------------------- | ------------ | ---------------- |
| Dash              | 72   | LED strip                 | 100 cm        | 1                |
| Center Console    | 4    | Fiber cable and injectors | 150 cm        | 2                |
| Front Doors       | 130  | Light bar (2 in parallel) | 215 cm + 50 cm | 3, 4             |
| Rear Doors        | 92   | Light bar                 | 405 cm + 45 cm | 5, 6             |
| Footwell lighting | 4    | LED strip or single LEDs  | 100 cm        | 7, 8             |

So for this build we have a total of around 784 LEDs and need ~20 m cable with 3 wires.
Also, ~2.5 m of fibre cable is recommended for use around the centre console.
Make sure to measure the required lengths carefully for individual build.

### Recommended length of light bars

If you are using the light bars, these are the recommended length of the light bars:
| Location | length of light bars |
| ------------- | ------------- |
| Front Doors top (MY 2019) | 100 cm, and shorten about 10 cm  |
|  Rear Doors top | 75 cm, and shorten a bit  |
| Front and Rear Doors around handle | 100 cm  |
| Dashboard | 130 cm, connect 2* 75 cm in series and shorten a bit  |
| Center console MY2019 (with cover above mobile phone holder) | 2* 85 cm (100 cm, and shorten about 15 cm) |
| Center console MY2021 | Fibre Wire, tbd light bar |

Make sure to measure the required lengths carefully for individual build.

## Power Consumption

Since this project can make use of many LEDs, the power consumption plays an important role.
NikoLight can (and should) be built with an onboard regulator.
This ensures that you stay below the limits the hardware can handle without modifications.
The regulator has 2 channels and is capable of providing a total of 6 A at 5 V, which is around 30 W of power.
These 6A are split to 2x 3A on the channels {1, 3, 5, 7} and {2, 4, 6, 8}.
The load of the two single channels should not exceed 3 A or 15 W at maximum.
By default, NikoLight has a configured power limit of 20 W, or 10 W per regulator.
If this limit is reached, the brightness is reduced to stay below this limit.
Also, a temperature limit of 80 °C is enforced by default when the optional temperature sensors are installed.

If you need to save some money, you can also only build one channel of the regulator.
Then the power output is halved and the jumper `J1` must be closed.
The full dual channel build is recommended because otherwise you are very limited on brightness.

If more power is really needed, then an external regulator is required and the board's input must be bypassed.
In that case, all components for the onboard regulator should be removed from the board.
An exception are the capacitors `C3` and `C4`, which should remain on the board.
The jumper `J1` should be closed
Instead of powering the board via the power input, it must be powered via an LED output.
Make sure to provide stable 5V to an LED output.

However, be careful when dealing with the high currents an external regulator could provide.
It could significantly heat up your connectors, wires, LEDs or even the NikoLight board.
You might need to change the connectors, use thicker wires and thicker copper on the PCB.

Assuming "average" WS2812 LED chips, each channel can draw around 14 mA at 5 V and maximum brightness.
Since each LED has 3 channels (red, green, blue), a current draw of around 42 mA has to be assumed.
To calculate the total power draw, the following formula can be used:

`power = led_count x 0.042 x 5`

From the example configuration above, we assume a total number of 784 LEDs.
You probably already feel that this could easily exceed the limits of the voltage regulator NikoLight comes with.

In theory, we could reach a current of 33 A (784 x 0.042) or 165 W (784 x 0.042 x 5) of power.
This is not safe to deal with for everyone.
Also, it would allow for brightness levels, that are far from practical, especially for the light bars.

In a normal use case, the power provided by the NikoLight controller is enough.
It still allows for a decent brightness without having to deal with dangerously high currents.
Like mentioned above, by default the software ensures that a maximum power draw of around 20 W is not exceeded.
Also, the regulator temperature is limited to 80 °C when the optional temperature sensors are installed.
You can adjust these limits in the UI.
For the power limit calculation to work properly, the LED voltage and current per channel must be configured correctly.

> What happens when I exceed the power limit of the regulator temporary or permanently?

Generally it is NOT recommended to do so in any way.
The 20 W power limit, or 10 W per channel, was tested to be a good compromise between brightness and regulator temperature.
Installing the optional temperature sensors to the board will also limit the temperature to 80 °C by default.
This temperature can be exceeded without the sensors.
When using a proper LM2596 (not faked ones), it will shut down as soon as a critical current or temperature (115 °C) is reached.
No damage _should_ be taken from this.
However, don't cry at me when you burn something down.
You have been warned.
