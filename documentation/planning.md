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
| 5            | Center console      |
| 2            | Left, front door    |
| 6            | Right, front door   |
| 3            | Rear, left door     |
| 7            | Rear, right door    |
| 4            | Footwell lighting 1 |
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
| Centre Console    | 4    | Fiber cable and injectors | 150cm        | 5                |
| Front Doors       | 130  | Light bar (2 in parallel) | 265cm + 50cm | 2, 6             |
| Rear Doors        | 92   | Light bar                 | 405cm + 45cm | 3, 7             |
| Footwell lighting | 4    | LED strip or single LEDs  | 100cm        | 4, 8             |

So for this build we have a total of around 784 LEDs and need ~20m cable with 3 wires.
Also 5m of fibre cable is recommended for use around the centre console.
Make sure to measure the required lengths carefully for individual build.

# Power Consumption

Since this project can make use of many LEDs, the power consumption plays an important role.
TesLight can (and should) be built with an onboard regulator.
This ensures that you stay below the limits the hardware can handle without modifications.
The regulator has 2 channels and is capable of providing a total of 6A at 5V, which is around 30W of power.
These 6A are split to 2x 3A on the channels {1, 2, 3, 4} and {5, 6, 7, 8}.
The load of the two single channels should not exceed 3A or 15W at maximum.
By default TesLight has a configured power limit of 20W, or 10W per regulator.
If this limit is reached, the brightness is reduced to stay below this limit.

If you need to save some money, you can also only build one channel of the regulator.
Then the power output is halfed and the jumper `J2` must be closed.
The full dual channel buld is recommended because otherwise you are very limited on brightness.

If more power is really needed, then an external regulator is required.
In that case all components for the onboard regulator must be removed from the board.
An exception are the capacitors `C3` and `C4`, which should remain on the board.
The jumpers `J1` and `J2` must be closed.

However, be careful when dealing with the high currents an external regulator could provide.
It could significantly heat up your connectors, wires, LEDs or even the TesLight board.
It is NOT recommended going any higher than around 8A in total.
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
By default the software ensures that a maximum power draw of around 20W is not exceeded.
You can adjust this value in the UI.
For this calculation to work properly, the LED voltage and current per channel must be configured correctly.

> What happens when I exceed the power limit of the regulator temporary or permanently?

Generally it is NOT recommended to do so in any way.
The 20W power limit, or 10W per channel, was tested to be a good compromise between brightness and regulator temperature.
When using a proper LM2596 (not faked ones), it will shut down as soon as a critical current or temperature (115°C) is reached.
Due to the missing power the controller will shut down and is not available via the web-app anymore.
No damange _should_ be taken from this.
Once the regulators cooled down, they should be reset and the controller reboots.
However this is a emergency feature that should not be triggered in normal use.
You are still risking temperatures of up to 115°C on the board.
Keep that in mind when you think you are special and need to exceed limits😛.
Also don't cry at me when you burn something down.
You have been warned.
