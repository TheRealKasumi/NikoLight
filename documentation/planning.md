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

### TesLight, Channels, Zones and LEDs

Alright, let's start with first thing first.
TesLight currently has a total of **8 LED channels**.
These channels are for the individual zones in your car.
This means that there are **8 different zones** that can be controlled independently.
How you spread them in your car is completely up to you.
You also don't have to use all of them but you can if you want to.
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

TesLight is using digital LEDs, so that it controll each LED individually.
The LED type used is **WS2812** and all LEDs accepting the same protocol at a 5V input.
Other LED types might not work as expected.

Each zone can have a **variable number of LEDs**.
This number can reach from **0 up to a few hundreds**.
The limiting factors for this are: available memory, time required for data transmission and processing power of the microcontroller.

Every single LED in a zone is **addressable**.
This means that TesLight can "talk" to the individual LEDs and set a 24 bit color value for each.
You can compare this to the pixels of your monitor.
This is achieved by **chaining the LEDs** like it is done on LED strips.
So each LED per zone will get a number from 0 to n, where n is the last LED in the line.

![LED Chain](media/planning/led-chain.png)

Now you have to make an important choice between three options for each part of the car.
They are all fully supported by TesLight.

-  Use a basic WS2812 LED strip
-  Use a fiber tube and 2 WS2812 LEDs as light injectors
-  Use a light bar which looks cleaner than a fiber tube and allows awesome animations

The recommendation is to use a normal LED strip under the dashboard.
The fiber tubes are working well for the center console or around the seats.
For the doors the light bars turned out to be very nice looking and be less noticable when turned off.
Also they are very bright and are visible even in sunlight.
On the other side they might be a little bright in the dark.
The choice here is fully up to you.
All parts can be found on the [part list](part-list.md).

### Zones

As explained above, TesLight supports up to **8 different zones**.
How you select these zones is up to you, but keep in mind that you must be able to install and wire the LEDs in the zones.

Since TesLight was designed with the Tesla Model 3 and Model Y in mind, there is a recommended configuration of the zones.
This is the same configuration as shown in the following video (click to open).

[![TesLight Demo](https://img.youtube.com/vi/_N5h1IViB-E/0.jpg)](https://www.youtube.com/watch?v=_N5h1IViB-E)

The following table assumes that the TesLight controller is placed below the centre console for easy access and wiring.

| Zone              | LEDs | Type                      | Cable Length | Physical Channel |
| ----------------- | ---- | ------------------------- | ------------ | ---------------- |
| Dash              | 72   | LED strip                 | 150cm        | 1                |
| Centre Console    | 4    | Fiber cable and injectors | 150cm        | 2                |
| Front Doors       | 130  | Light bar (2 in parallel) | 265cm + 50cm | 3, 4             |
| Rear Doors        | 92   | Light bar                 | 405cm + 45cm | 5, 6             |
| Footwell lighting | 4    | LED strip or single LEDs  | 100cm        | 7, 8             |

So for this build we have a total of around 784 LEDs and need ~20m cable with 3 wires.
Also 5m of fibre cable is recommended for use around the centre console.
Make sure to measure the required lengths carefully for individual build.

# Power Consumption

Since this project can make use of many LEDs, the power consumption plays an important role.
TesLight can (and should) be built with an onboard regulator.
This ensures that you stay below the limits the hardware can handle without modifications.
The regulator has 2 channels and is capable of providing a total of 6A at 5V, which is around 30W of power.
These 6A are split to 2x 3A on the channels {1, 3, 5, 7} and {2, 4, 6, 8}.
The load of the two single channels should not exceed 3A or 15W at maximum.
By default TesLight has a configured power limit of 20W, or 10W per regulator.
If this limit is reached, the brightness is reduced to stay below this limit.
Also a temperature limit of 80°C is enforced by default when the optinal temperature sensors are installed.

If you need to save some money, you can also only build one channel of the regulator.
Then the power output is halfed and the jumper `J1` must be closed.
The full dual channel buld is recommended because otherwise you are very limited on brightness.

If more power is really needed, then an external regulator is required and the boards input must be bypassed.
In that case all components for the onboard regulator should be removed from the board.
An exception are the capacitors `C3` and `C4`, which should remain on the board.
The jumper `J1` should be closed
Instead of powering the board via the power input, it must be powered via an LED output.
Make sure to provide stable 5V to an LED output.

However, be careful when dealing with the high currents an external regulator could provide.
It could significantly heat up your connectors, wires, LEDs or even the TesLight board.
You might need to change the connectors, use thicker wires and thicker copper on the PCB.

Assuming "average" WS2812 LED chips, each channel can draw around 14mA at 5V and maximum brightness.
Since each LED has 3 channels (red, green, blue), a current draw of around 42mA has to be assumed.
To calculate the total power draw, the following formula can be used:

`power = led_count x 0.042 x 5`

From the example configuration above we assume a total number of 784 LEDs.
You probably already feel that this could easily exceed the limits of the voltage regulator TesLight comes with.

In theory we could reach a current of 33A (784 x 0.042) or 165W (784 x 0.042 x 5) of power.
This is not save to deal with for everyone.
Also it would allow for brightness levels, that are far from practical, especially for the light bars.

In a normal use case, the power provided by the TesLight controller is enough.
It still allows for a decent brightness without having to deal with dangerously high currents.
Like mentioned above, by default the software ensures that a maximum power draw of around 20W is not exceeded.
Also the regulator temperature is limited to 80°C when the optional temperature sensors are installed.
You can adjust these limits in the UI.
For the power limit calculation to work properly, the LED voltage and current per channel must be configured correctly.

> What happens when I exceed the power limit of the regulator temporary or permanently?

Generally it is NOT recommended to do so in any way.
The 20W power limit, or 10W per channel, was tested to be a good compromise between brightness and regulator temperature.
Installing the optional temperature sensors to the board will also limit the temperature to 80°C by default.
This temperature can be exceeded without the sensors.
When using a proper LM2596 (not faked ones), it will shut down as soon as a critical current or temperature (115°C) is reached.
No damange _should_ be taken from this.
However don't cry at me when you burn something down.
You have been warned.
