# TesLight User Interface

The frontend (web-app) is a browser based UI that is used to configure and control TesLight.
It is served by the TesLight controller from the microSD card.
For this the TesLight controller act's as a WiFi access point and web server.
By connecting a device (computer, laptop, smartphone, ...) to the TesLight controller and entering the IP in a web browser, the TesLight UI is loaded.

## Usage

Generally the frontend files only need to be copied to the MicroSD card, which is plugged into the TesLight controller.
There is no build step necessary.
However, due to hardware limitations the frontend should be minified beforhand to avoid stability issues.
The topic is described in more detail in the [ReadMe](minifier/ReadMe.md) of the minifier.

After the frontend was minified, the minified frontend files can be copied to the `web-app` folder on the MicroSD card.

Next a connection the the TesLight controller can be established.
The TesLight controller is acting as a WiFi access point so that all WiFi capable devices (computer, laptop, smartphone, ...) can connect to it.
By default the access point has the following SSID and password:

```
SSID: TesLight
Password: TesLight
```

Keep in mind that these might be different if you are using a different configuration.

When the connection was established successfully, the frontend should now be able load.
Open a browser on your connected device and enter the (default) IP `192.168.4.1`.
