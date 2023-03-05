# TesLight

[![latest release](https://img.shields.io/github/v/release/TheRealKasumi/TesLight)](https://github.com/TheRealKasumi/TesLight/releases/latest)
![PlatformIO Build](https://github.com/TheRealKasumi/TesLight/actions/workflows/PlatformIO-Build.yml/badge.svg?branch=main)
![UI Build](https://github.com/TheRealKasumi/TesLight/actions/workflows/ui-build.yml/badge.svg?branch=main)
![TUP Build](https://github.com/TheRealKasumi/TesLight/actions/workflows/teslight-update-packaging-tool-and-tup-file-build.yml/badge.svg?branch=main)

## Introduction

TesLight is an open source ambient light that you can build yourself.
The project is primarily aimed at people who are not looking for a ready-made solution and prefer to tinker, modify, extend and work on it as a community.
As the name suggests, the project was started primarily for Tesla vehicles.
The focus is on the Model 3 and Model Y.
However, it can also be used for other cars with some small modifications.

With TesLight you can control up tp 8 zones with a variable number of WS2812-type LEDs.
For example, the 4 doors, the center console, the footwell lights and a light bar under the dashboard.
In total, several hundred LEDs can be controlled individually.

Since there should be an easy way to control all these LEDs, there are many different effects for each zone.
These can display a simple color, color gradients or even animations in the individual zones.
Since the controller has a motion sensor, some effects are even interactive.
The effects are expandable in the future and can be adapted to your wishes over time.
Even fully customized animation are possible.
You can create these on your computer and play them back in the car.

TesLight can be easily integrated into your Tesla (and other cars).
There is a sensor that can be connected to the lighting around the footwell.
This way, TesLight switches on and off automatically when you open or lock the car.
It is also possible to regulate the brightness together with the factory-installed lights or via the display.
The controller is also expandable and could support additional hardware in the future.
For example, there could be a CAN adapter to get more information from the vehicle.
How relevant these extensions will be, depends a little bit on the community and the support I will receive.

Once built and installed, TesLight can be configured via a browser-based UI.
When starting the controller, a Wi-Fi hotspot is created to which you can connect to with basically any device.
The user interface can be accessed via a normal browser thanks to the integrated web server.
The project is therefore mostly platform-independent.

**Here is a short summary of the currently most interesting features:**

- Individual control of each LED installed in the car
- Hundreds of LEDs are possible
- Compatible with all WS2812-type LEDs (and LEDs with the same protocol)
- Fiber cables, light bars and normal LED strips are supported
- Up to 8 individual zones
- Currently 18 different effects that can be selected and further customized for each zone
- Browser based user interface
- Switching on/off as well as dynamic brightness via the existing lighting in the footwell
- Rotation and acceleration sensors
- Interactive effects
- Light shows
- Fully customized animations can be created on your PC (playback of fseq 1.0 files from [xLights](https://xlights.org/))
- OTA (wireless) updates
- Hardware is upgradeable via extensions in the future

## Some Videos and Pictures

[![TesLight Demo 2](https://img.youtube.com/vi/_N5h1IViB-E/0.jpg)](https://www.youtube.com/watch?v=_N5h1IViB-E)

(click to watch on YouTube)

[![TesLight Demo 1](https://img.youtube.com/vi/bwzbVkCsNws/0.jpg)](https://www.youtube.com/watch?v=bwzbVkCsNws)

(click to watch on YouTube)

![App](documentation/media/readme/app.jpg)

![PCB](documentation/media/build/pcb.png)

## Planning

It is recommended to read the [planning](documentation/planning.md) document before you start the project.
There are some important considerations and decisions you have to make first.
This will save you some trouble and makes it easier to buy the right parts.

## Part List

A list of required and optional parts can be found [here](documentation/part-list.md).

## Build Guide

The build guide can be found [here](documentation/build.md).

## One Request

A lot of our time, effort and money went into this project.
Nevertheless, we share it publicly with you.
You can use it for free, modify it and even earn a coffee by building the parts for other people as long as you are a private person without commercial intentions.
But please **don't steal** our work, be a little **respectful** and **give credits**.
That said, I hope you have a lot of fun and great experience with TesLight!
Thank you💖!

## Contributors

<table>
  <tbody>
    <tr>
      <td align="center">
        <a href="https://github.com/TheRealKasumi">
          <img src="https://avatars.githubusercontent.com/u/62426919" width="100px;" alt="TheRealKasumi"/>
          <br />
          <b>TheRealKasumi</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
    <tr>
      <td align="center">
        <a href="https://github.com/danielbayerlein">
          <img src="https://avatars.githubusercontent.com/u/457834" width="100px;" alt="danielbayerlein"/>
          <br />
          <b>danielbayerlein</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/JakobLichterfeld">
          <img src="https://avatars.githubusercontent.com/u/16510328" width="100px;" alt="JakobLichterfeld"/>
          <br />
          <b>JakobLichterfeld</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
    </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/heyitsmarlon">
          <img src="https://avatars.githubusercontent.com/u/74954698" width="100px;" alt="heyitsmarlon"/>
          <br />
          <b>heyitsmarlon</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
    </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/MathiasSeifert">
          <img src="https://avatars.githubusercontent.com/u/19532855" width="100px;" alt="MathiasSeifert"/>
          <br />
          <b>MathiasSeifert</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
    </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/PhilippDenn">
          <img src="https://avatars.githubusercontent.com/u/114821380" width="100px;" alt="PhilippDenn"/>
          <br />
          <b>PhilippDenn</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
    </tr>
     <tr>
      <td align="center">
        <a href="https://github.com/rgrae81">
          <img src="https://avatars.githubusercontent.com/u/36889990" width="100px;" alt="rgrae81"/>
          <br />
          <b>rgrae81</b>
        </a>
      </td>
      <td align="center" valign="top">
        <p>Placeholder</p> 
      </td>
    </tr>
  </tbody>
</table>

## Disclaimer

Please be careful when working on the project and especially your car.
I am not a professional electrical engineer and only doing this as a hobby on the side.
Use this project at your own risk.
