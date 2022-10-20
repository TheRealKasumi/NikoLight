# Planning

It is recommended to read through this short document before you start the project.
There are some important considerations and decisions you need to make first.
This will save you some trouble and make it easier to buy the right parts.
It will also help when building the project later.

## LED Configuration

The LED configuration is the most important step.
Here you need to consider where to place the LEDs, how they are attached and the number you want to install in your car.
This is personal preference and will influence the part list and building steps.
Also, it will affect how much work you have to put into the project.
So let's start with some important information.

### TesLight and LEDs

TesLight currently has a total of **6 LED channels**.
These channels are for the individual zones in your car.
This means that there are **6 different zones** that can be controlled independently.
The LED type used is **WS2812** and compatible ones.

Each zone can have a **variable number of LEDs**.
This number can reach from **0 up to a few hundreds**.
The two limiting factors for this (in order of importance) are: available memory and processing power of the microcontroller.

Every single LED in a zone is **addressable**.
This means that TesLight can "talk" to the individual LEDs and set a 24 bit color and brightness value for each.
This is achieved by **chaining the LEDs** like it is done on LED strips.
This means you can use **WS2812 compatible LED strips**.

![LED Chain](media/planning/led-chain.png)

### Zones

As explained above, TesLight supports up to **6 different zones**.
How you select these zones is up to you, but keep in mind that you must be able to install and wire the LEDs in the zones.
Ideally they should be easily accessible from within the car or you should know how to gain access them.

Since TesLight was designed with the Tesla Model 3 and Model Y in mind, there is a recommended configuration of the zones.
This is the same configuration as shown in the teaser image and what will be used for this guide.

![Teaser 1](media/teaser/teaser_1.jpeg)

The following table assumes that the TesLight controller is placed below the centre console for easy access.
This makes wiring relatively "easy".
More about that later in the build guide.

| Zone           | LEDs  | Cable Length              | Physical Channel |
| -------------- | ----- | ------------------------- | ---------------- |
| Dash           | 34-72 | 150cm                     | 1                |
| Centre Console | 2     | 100cm                     | 2                |
| Front Doors    | 4     | 265cm + 8cm + 75cm + 90cm | 3, 4             |
| Rear Doors     | 4     | 405cm + 8cm + 75cm + 75cm | 5, 6             |

So for this build we need between 44 and 90 LEDs plus ~20m cable with 3 wires.
Also 15m of fibre cable was used around the doors and centre console.
Make sure to measure the required length carefully for the build.

# Power Consumption

Since the WS2812 type LEDs and the TesLight controller require a stable 5V power supply, power consumption is important.
TesLight can be built with an onboard 2-3A buck converter.
This results in a power output of around 10-15W max for the LEDs.
If more power is needed then an external regulator is required.

Assuming "standard" 5x5mm LED chips, each channel can draw around 12mA at 5V and maximum brightness.
Since each LED has 3 channels (red, green, blue), a current draw of around 36mA has to be assumed.
To calculate the total power draw, the following formula can be used:

`power = led_count * 0.036 * 5`

From the example configuration above we assume a total number of LEDs ranging from 44 and 90.

If the minimum number of LEDs were used this would result in a current draw of around 1.58A or a total power of 7.92W.
The onboard regulator is perfectly suitable for this kind of load.

Assuming the second case with 90 LEDs, we expect a current draw of up to 3.24A or a total power of 16.2W.
This comes close to the limit of the onboard regulator.
In practice, these loads are only reached when the LEDs are set to full brightness with constant white color.
If you are plan to use the LEDs at max current draw, you should consider to use an external regulator with a higher power limit.
When using the rainbow effects or brighness is lowered then current limits are not reached.

You can also configure a power limit in the UI.
TesLight will then automatically limit the brightness to not exceed this limit.
By default this limit is set to 10W to protect the onboard regulator.
For this calculation to work properly, the LED voltage and current per channel must be configured correctly.

The PCB should be good for around 8-10A.
However, make sure that your connectors are high quality and can handle this level of current.
It's recommended to never exceed 3A on a single LED port of the PCB.
If even higher current is required, please bypass the PCB and connect the LEDs directly to your regulator.
However, don't forget to provide 5V to the TesLight controller and have a shared ground connection.

> What happens when I exceed the power limit of the regulator permanently?

When using a proper LM2596 (not faked ones), it will shut down as soon as a critical current or temperature (105°C) is reached.
It's not recommended but nothing should be burnt or damaged.
