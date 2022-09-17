# Part List

This document contains a list of all required parts to build TesLight.
The list was split into required and optional parts.
Since TesLight can be modified in a lot of ways, the actual part list depends a little on your individual plans.
Make sure to read the [planning](planning.md) document first.
Also the list assumes that you have all required tools and materials for building electronics projects like a soldering iron.

## Required Parts

| Part                         | Count   | Source                | Comment                                                                   |
|------------------------------|---------|-----------------------|---------------------------------------------------------------------------|
| TesLight PCB or proto board  | 1       | [JLCPCB][JLCPCB]      | Can be ordered here using gerber files from the project                   | 
| TesLight case                | 1       | [JLCPCB][JLCPCB]      | Can be ordered here using 3D models from the project                      |  
| TesLight injector cases      | depends | [JLCPCB][JLCPCB]      | Can be ordered here using 3D models from the project                      |   
| AzDelivery ESP32 Dev Board   | 1       | [Amazon][ESP32]       | Or compatible with same size and layout                                   |
| WS2812B LED or LED Strip     | depends | [Amazon][WS2812B]     | Number depends on your configuration                                      | 
| Fiber cable                  | depends | [Amazon][FIBER]       | Length depends on your configuration                                      | 
| MPU6050 Motion Sensor        | 1       | [Reichelt][MPU6050]   |                                                                           |
| Micro SD Socket              | 1       | [Amazon][SDSOCKET]    |                                                                           |
| SMD Resistor 220Ω or 330Ω    | 6       | [Amazon][RESISTORS]   | Amazon set covers all values                                              |
| SMD Resistor 1kΩ             | 1       | [Amazon][RESISTORS]   | Amazon set covers all values                                              |
| SMD Resistor 4.7kΩ           | 1       | [Amazon][RESISTORS]   | Amazon set covers all values                                              |
| SMD Capacitor 10µf           | 1       | [Reichelt][CAP10]     | 6.3-10V recommended                                                       |
| Zener Diode 3.3V 1/2W        | 1       | [Reichelt][ZENER3.3]  |                                                                           |
| Fuse + Socket 1-8A           | 1       | ?                     | Depends on the number of LED's                                            |
| JST-XH 3 Pin Male            | 7       | [Reichelt][JSTXH3M]   |                                                                           |
| JST-XH 3 Pin Female          | 7       | [Reichelt][JSTXH3F]   |                                                                           |
| JST-XH 4 Pin Male            | 1       | [Reichelt][JSTXH4M]   |                                                                           |
| JST-XH 4 Pin Female          | 1       | [Reichelt][JSTXH4F]   |                                                                           |
| JST-XH Connectors            | 25      | [Reichelt][JSTXHCONN] | Maybe get a few more to be on the safe side                               |
| Cables                       | depends | [Amazon][CABLES]      | Depends on the number of LEDs and your cable management                   |

## Optional Parts On Board Regulator (Max 3A)
| Part                         | Count   | Source               | Comment                                                                   |
|------------------------------|---------|----------------------|---------------------------------------------------------------------------|
| LM2596 Regulator - 5V fixed  | 1       | [Reichelt][LM2596]   | Make sure to get the fixed 5V version                                     |
| IN5824 Schottky Diode        | 1       | [Reichelt][IN5824]   | Or diode with similar specs                                               |
| Capacitor 220µf              | 1       | [Reichelt][CAP220]   | 6.3-10V recommended                                                       |
| Inductor min. 33µH           | 1       | [Reichelt][IND]      |                                                                           |

## Optional Parts
| Part                         | Count   | Source               | Comment                                                                   |
|------------------------------|---------|----------------------|---------------------------------------------------------------------------|
| SMD Resistor 10kΩ            | 4       | [Amazon][RESISTORS]  | Required for some micro SD cards as pull-up, worked fine without for me   |
| SMD Capacitor 40-100µf       | 1       | [Reichelt][CAP100]   | Used to delay the start of the ESP32 until stable voltage is established  |
| Shrinking tube               | depends | [Amazon][TUBES]      | Will help you to have a cleaner and safer build                           |
| Fabric tape                  | depends | [Amazon][TAPE]       | Will help you to have a cleaner and safer build                           |

## Order the PCB and Printed Parts

See [build guide](build.md#order-the-pcb-and-3d-printed-parts).

[JLCPCB]: https://cart.jlcpcb.com/quote?orderType=1&stencilLayer=2&stencilWidth=100&stencilLength=100
[ESP32]: https://www.amazon.de/AZDelivery-NodeMCU-Development-Nachfolgermodell-ESP8266/dp/B071P98VTG/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=3U1PNFQJDSD0A&keywords=az%2Besp32&qid=1661668180&sprefix=az%2Besp32%2Caps%2C82&sr=8-3&th=1
[WS2812B]: https://www.amazon.de/BTF-LIGHTING-adressierbar-Vollfarbiger-wasserdichte-DIY-Projekte/dp/B088BRY2SH/ref=sr_1_8?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1OSVQ80JPAJDG&keywords=ws2812b+strip+60%2Fm&qid=1662188470&sprefix=ws2812b+strip+60%2Fm%2Caps%2C82&sr=8-8
[FIBER]: https://www.amazon.de/TABEN-Autoseitenlicht-LED-Beleuchtung-Innenatmosph%C3%A4re-Neonstreifenleuchten/dp/B08DLJ2JJ1/ref=sr_1_7?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=taben+fiber&qid=1662190164&sr=8-7
[MPU6050]: https://www.reichelt.de/entwicklerboards-beschleunigung-gyroskop-3-achsen-mpu-6050-debo-sens-3axis-p253987.html?&trstct=pos_0&nbc=1
[SDSOCKET]: https://www.amazon.de/sourcingmap-St%C3%BCck-Oberfl%C3%A4chenmontage-Speicherkarte-Sockel/dp/B00E6PVMU2/ref=sr_1_2?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=VKTMLVUP5HRL&keywords=micro+sd+socket+pcb&qid=1661668495&sprefix=micro+sd+socket+pcb%2Caps%2C71&sr=8-2
[RESISTORS]: https://www.amazon.de/ARCELI-SMD-Widerst%C3%A4nde-Sortimentsatz-SMD-Widerstand-Insgesamt/dp/B07RTH6JHX/ref=sr_1_7?keywords=smd+widerstand+sortiment&qid=1661668614&sprefix=smd+widerstand+sor%2Caps%2C82&sr=8-7
[CAP10]: https://www.reichelt.de/smd-tantal-kondensator-10-f-10v-tps-3216-10-10-p167046.html?&trstct=pos_1&nbc=1
[ZENER3.3]: https://www.reichelt.de/zener-diode-3-3-v-0-5-w-5-sod-80-minimelf--tzmc3v3-p280026.html?&trstct=pos_0&nbc=1
[JSTXH3M]: https://www.reichelt.de/jst-buchsengehaeuse-1x3-polig-xh-jst-xh3p-bu-p185086.html?&trstct=pos_6&nbc=1
[JSTXH3F]: https://www.reichelt.de/jst-stiftleiste-gerade-1x3-polig-xh-jst-xh3p-st-p185074.html?&trstct=pos_1&nbc=1
[JSTXH4M]: https://www.reichelt.de/jst-buchsengehaeuse-1x4-polig-xh-jst-xh4p-bu-p185087.html?&trstct=pos_5&nbc=1
[JSTXH4F]: https://www.reichelt.de/jst-stiftleiste-gerade-1x4-polig-xh-jst-xh4p-st-p185075.html?&trstct=pos_13&nbc=1
[JSTXHCONN]: https://www.reichelt.de/jst-crimpkontakt-buchse-xh-jst-xh-ckb-p185091.html?&trstct=pos_0&nbc=1
[CABLES]: https://www.amazon.de/LEADTOPS-3-poliger-Leiterverl%C3%A4ngerungs-Verzinntes-Traumfarbe/dp/B08JPLRXRK/ref=sr_1_13?keywords=kabel+3+adrig&qid=1662188870&sprefix=kabel+3+%2Caps%2C94&sr=8-13

[LM2596]: https://www.reichelt.de/abwaerts-schaltregler-adj-4-5--40-v-5-0-v-3-a-to-220-5-lm-2596-t5-0-p109365.html?&trstct=pos_1&nbc=1
[IN5824]: https://www.reichelt.de/schottkydiode-40-v-5-a-do-214ab-smc-sk-54c-p146598.html?&trstct=pos_3&nbc=1
[CAP220]: https://www.reichelt.de/elko-radial-220-uf-10-v-105-c-low-esr-fm-a-220u-10-p200036.html?&trstct=pos_1&nbc=1
[IND]: https://www.reichelt.de/smd-power-induktivitaet-eisenpulverkern-100-h-pan-etqp5m101ygk-p245793.html?&trstct=pos_2&nbc=1

[CAP100]: https://www.reichelt.de/smd-tantal-100-f-10-6-3v-case-b-125-c-ve-2000-t491b-100u-6-p206458.html?&trstct=pos_5&nbc=1
[TUBES]: https://www.amazon.de/ChiliTec-12000058-Chilitec-Schrumpfschlauch-Sortiment-100-teilig/dp/B003H9CJ1Y/ref=sr_1_20_mod_primary_new?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1Q5IY5D32HNLR&keywords=schrumpfschlauch&qid=1662189028&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=schrumpfschlauch%2Caps%2C115&sr=8-20
[TAPE]: https://www.amazon.de/tesa-Gewebeband-Witterungsbest%C3%A4ndiges-Panzertape-Befestigen/dp/B000KT7E0U/ref=sr_1_5?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1U7SK4BSKYGK0&keywords=Gewebeband&qid=1662189088&sprefix=gewebeban%2Caps%2C94&sr=8-5