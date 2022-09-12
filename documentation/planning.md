# Planning

It is recommended to read through this short document before you start with the project.
There are some important considerations and decisions you have to make first.
This will save you some trouble and makes it easier to buy the right parts.
It will also help to build the project later.

## LED Configuration

The LED configuration is the most important step.
Here you need to think about where, how and how many LEDs you want to install in your car.
This highly depends on your wishes and will influence the part list and building steps.
Also, it will affect how much work you have to put into the project.
So let's start with some important information.

### TesLight and LEDs

TesLight currently has a total of **6 LED channels**.
These channels are for the individual zones in your car.
This means that **6 different zones** can be controlled individually.
The used LED type is **WS2812** and compatible ones.

Each zone can have a **variable number of LED's**.
This number can reach from **0 up to a few hundreds**.
It is mainly limited by the amount of memory and secondary by the processing power of the used Microcontroller.

Every single LED in a zone is **addressable**.
This means that TesLight can "talk" to the individual LED's and set a 24 bit color and brightness value for each.
This is achieved by **chaining the LED's** like it is done on LED strips.
As a consequence you can obviously use **WS2812 compatible LED strips**. 

![LED Chain](media/planning/led-chain.png)

### Zones

As explained above, TesLight supports up to **6 different zones**.
How you select these zones is up to you.
But keep in mind that you have to be able to install and wire the LED's in these places.
So they should be accessible in the car or you should know how the get access to them.

Since TesLight was mainly designed for the Tesla Model 3 and Model Y there is a recommended configuration of the zones.
The guide will focus on these.
This is the same configuration shown in the teaser.

![Teaser 1](media/teaser/teaser_1.jpeg)

The following table assumes that the TesLight controller is placed below of the center console.
This allows easy access and positions the controller centered in the car.
As a consequence the wiring is relatively "easy".
More about that at a later point in the build guide.

| Zone            | LED's | Cable Length              | Physical Channel |
|-----------------|-------|---------------------------|------------------|
| Dash            | 34-72 | 150cm                     | 1                |
| Center Console  | 2     | 100cm                     | 2                |
| Front Doors     | 4     | 265cm + 8cm + 75cm + 90cm | 3, 4             |
| Rear Doors      | 4     | 405cm + 8cm + 75cm + 75cm | 5, 6             |

So for this build we would need 44 to 82 LEDs and ~20m cable with 3 wires.
Also 15m of the fiber "cable" was used around the doors and center console.
Make sure to measure the required length carefully for the build.

# Power Consumption

Since the WS2812 type LED's and the TesLight controller require a stable 5V power supply, the power consumption becomes relevant.
TesLight can be built with an on-board 2-3A buck converter.
This results in a power output of around 10-15W max for the LED's.
If more power is required a external regulator is required.

Assuming "standard" 5x5mm LED chips, each channel can draw around 15mA at 5V and maximum brightness.
Since each LED has 3 channels (red, green, blue), a current draw of around 45mA has to be assumed.
To calculate the total power draw, to following formula can be used:

`power = led_count * 0.045 * 5`

From the example configuration above we assume a total of 44 to 82 LED's.

In the first case this would result in a current draw of around 1.98A or a total power of 9.9W.
The on board regulator is perfectly suitable for this kind of load.

Assuming the second case with 82 LED's, we have to expect a current draw of up to 3.69A or a total power of 18.45W.
This could theoretically reach the limit of the onboard regulator.
In practice, these loads are only reached when the LED's are set to full brightness at a constant white color.
If you are planing to do so, you should go with an external regulator with a higher power limit.
Otherwise when using the rainbow effects or lower brighness, these limits will not be reached.

> What happens when I exceed the power limit of the regulator permanently?

When using a proper LM2596 (not faked ones), it will shut down as soon as a critical current or temperature is reached.
It's not recommended but it shouldn't destroy or burn anything.
