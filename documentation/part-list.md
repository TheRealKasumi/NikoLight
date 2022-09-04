# Part List

This document contains a list of all required parts to build TesLight.
The list was split into required and optional parts.
Since TesLight can be modified in a lot of ways, the actual part list depends a little on your individual plans.
Make sure to read the [planning](/documentation/planning.md) document first.
Also the list assumes that you have all required tools and materials for building electronics projects like a soldering iron.

## Required Parts

| Part                         | Count   | Source               | Comment                                                                   |
|------------------------------|---------|----------------------|---------------------------------------------------------------------------|
| TesLight PCB or proto board  | 1       | [JLCPCB][JLCPCB]     | Can be ordered here using gerber files from the project                   | 
| TesLight case                | 1       | [JLCPCB][JLCPCB]     | Can be ordered here using 3D models from the project                      |  
| TesLight injector cases      | depends | [JLCPCB][JLCPCB]     | Can be ordered here using 3D models from the project                      |   
| AzDelivery ESP32 Dev Board   | 1       | [Amazon][ESP32]      | Or compatible with same size and layout                                   |
| WS2812B LED or LED Strip     | depends | [Amazon][WS2812B]    | Number depends on your configuration                                      | 
| Fiber cable                  | depends | [Amazon][FIBER]      | Length depends on your configuration                                      | 
| MPU6050 Motion Sensor        | 1       | [Amazon][MPU6050]    |                                                                           |
| Micro SD Socket              | 1       | [Amazon][SDSOCKET]   |                                                                           |
| SMD Resistor 220Ω or 330Ω    | 6       | [Amazon][RESISTORS]  | Amazon set covers all values                                              |
| SMD Resistor 1kΩ             | 1       | [Amazon][RESISTORS]  | Amazon set covers all values                                              |
| SMD Resistor 4.7kΩ           | 1       | [Amazon][RESISTORS]  | Amazon set covers all values                                              |
| SMD Capacitor 10µf           | 1       | [Amazon][CAP10]      | 6.3-10V recommended                                                       |
| Zener Diode 3.3V 1/2W        | 1       | ?                    |                                                                           |
| Fuse + Socket 1-8A           | 1       | ?                    | Depends on the number of LED's                                            |
| JST-XH 3 Pin M/F             | 7       | [Amazon][CONNECTORS] |                                                                           |
| JST-XH 4 Pin M/F             | 1       | [Amazon][CONNECTORS] |                                                                           |
| Cables                       | depends | [Amazon][CABLES]     | Depends on the number of LEDs and your cable management                   |

## Optional Parts On Board Regulator (Max 3A)
| Part                         | Count   | Source               | Comment                                                                   |
|------------------------------|---------|----------------------|---------------------------------------------------------------------------|
| LM2596 Regulator - 5V fixed  | 1       | ?                    | Make sure to get the fixed 5V version                                     |
| IN5824 Schottky Diode        | 1       | [Amazon][IN5824]     | Or diode with similar specs                                               |
| Capacitor 220µf              | 1       | [Amazon][CAP220]     | 6.3-10V recommended                                                       |
| Inductor 33µH                | 1       | [Amazon][IND33]      |                                                                           |

## Optional Parts
| Part                         | Count   | Source               | Comment                                                                   |
|------------------------------|---------|----------------------|---------------------------------------------------------------------------|
| SMD Resistor 10kΩ            | 4       | [Amazon][RESISTORS]  | Required for some micro SD cards as pull-up, worked fine without for me   |
| SMD Capacitor 40-100µf       | 1       | [Amazon][CAP100]     | Used to delay the start of the ESP32 until stable voltage is established  |
| Shrinking tube               | depends | [Amazon][TUBES]      | Will help you to have a cleaner and safer build                           |
| Fabric tape                  | depends | [Amazon][TAPE]       | Will help you to have a cleaner and safer build                           |

[JLCPCB]: https://cart.jlcpcb.com/quote?orderType=1&stencilLayer=2&stencilWidth=100&stencilLength=100
[ESP32]: https://www.amazon.de/AZDelivery-NodeMCU-Development-Nachfolgermodell-ESP8266/dp/B071P98VTG/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=3U1PNFQJDSD0A&keywords=az%2Besp32&qid=1661668180&sprefix=az%2Besp32%2Caps%2C82&sr=8-3&th=1
[WS2812B]: https://www.amazon.de/BTF-LIGHTING-adressierbar-Vollfarbiger-wasserdichte-DIY-Projekte/dp/B088BRY2SH/ref=sr_1_8?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1OSVQ80JPAJDG&keywords=ws2812b+strip+60%2Fm&qid=1662188470&sprefix=ws2812b+strip+60%2Fm%2Caps%2C82&sr=8-8
[FIBER]: https://www.amazon.de/TABEN-Autoseitenlicht-LED-Beleuchtung-Innenatmosph%C3%A4re-Neonstreifenleuchten/dp/B08DLJ2JJ1/ref=sr_1_7?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=taben+fiber&qid=1662190164&sr=8-7
[MPU6050]: https://www.amazon.de/ARCELI-Beschleunigungsmesser-Gyroskop-Beschleunigungssensor-Datenausgang/dp/B07BVXN2GP/ref=sr_1_6?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=7REQ92RBGA49&keywords=mpu6050&qid=1661668403&sprefix=mpu6050%2Caps%2C103&sr=8-6
[SDSOCKET]: https://www.amazon.de/sourcingmap-St%C3%BCck-Oberfl%C3%A4chenmontage-Speicherkarte-Sockel/dp/B00E6PVMU2/ref=sr_1_2?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=VKTMLVUP5HRL&keywords=micro+sd+socket+pcb&qid=1661668495&sprefix=micro+sd+socket+pcb%2Caps%2C71&sr=8-2
[RESISTORS]: https://www.amazon.de/ARCELI-SMD-Widerst%C3%A4nde-Sortimentsatz-SMD-Widerstand-Insgesamt/dp/B07RTH6JHX/ref=sr_1_7?keywords=smd+widerstand+sortiment&qid=1661668614&sprefix=smd+widerstand+sor%2Caps%2C82&sr=8-7
[CAP10]: https://www.amazon.de/Temhyu-Shuxiang-Kondensatoren-Tantalkondensator-Tantal-Kondensator-Kondensatoren/dp/B08XQ5MQQL/ref=sr_1_13?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=35IHIF1HLF5U3&keywords=smd+capacitor+10uf&qid=1661669568&sprefix=smd+capacitor+10uf%2Caps%2C70&sr=8-13
[CONNECTORS]: https://www.amazon.de/YIXISI-Stecker-Weiblich-Adapter-M%C3%A4nnlich/dp/B082ZLYRRN/ref=sr_1_6?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=17MXVOEFJHZCD&keywords=xh+connector+set&qid=1661670146&sprefix=xh+connector+set%2Caps%2C79&sr=8-6
[CABLES]: https://www.amazon.de/LEADTOPS-3-poliger-Leiterverl%C3%A4ngerungs-Verzinntes-Traumfarbe/dp/B08JPLRXRK/ref=sr_1_13?keywords=kabel+3+adrig&qid=1662188870&sprefix=kabel+3+%2Caps%2C94&sr=8-13

[IN5824]: https://www.amazon.de/POPESQ%C2%AE-Stk-Diode-Schottky-A1542/dp/B084WW2TCL/ref=sr_1_5?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=IVSKUBPI2J35&keywords=schottky+5A+smd&qid=1662187694&sprefix=schottky+5a+smd%2Caps%2C68&sr=8-5
[CAP220]: https://www.amazon.de/B2Q-Elektrolytkondensator-Kondensator-220uF-105%C2%B0C/dp/B00Q00NL28/ref=sr_1_14?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=CSHGES7B165V&keywords=capacitor+10v+220uf&qid=1662187834&sprefix=capacitor+10v+220uf%2Caps%2C66&sr=8-14
[IND33]: https://www.amazon.de/Aexit-Einbaufl%C3%A4che-eckig-Inductanz-Leistung/dp/B07D7XCB9T/ref=sr_1_8?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=2OO062SX0V1CQ&keywords=inductor+33uh+5a&qid=1662187940&sprefix=inductor+33uh+5a%2Caps%2C64&sr=8-8

[CAP100]: https://www.amazon.de/ARCELI-SMD-Widerst%C3%A4nde-Sortimentsatz-SMD-Widerstand-Insgesamt/dp/B07RTH6JHX/ref=sr_1_7?keywords=smd+widerstand+sortiment&qid=1661668614&sprefix=smd+widerstand+sor%2Caps%2C82&sr=8-7
[TUBES]: https://www.amazon.de/ChiliTec-12000058-Chilitec-Schrumpfschlauch-Sortiment-100-teilig/dp/B003H9CJ1Y/ref=sr_1_20_mod_primary_new?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1Q5IY5D32HNLR&keywords=schrumpfschlauch&qid=1662189028&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=schrumpfschlauch%2Caps%2C115&sr=8-20
[TAPE]: https://www.amazon.de/tesa-Gewebeband-Witterungsbest%C3%A4ndiges-Panzertape-Befestigen/dp/B000KT7E0U/ref=sr_1_5?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1U7SK4BSKYGK0&keywords=Gewebeband&qid=1662189088&sprefix=gewebeban%2Caps%2C94&sr=8-5