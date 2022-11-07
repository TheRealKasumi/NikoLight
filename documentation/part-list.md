# Part List

This document contains the full list of all parts to build TesLight.
The list has been split into required and optional parts.
Since TesLight can be modified in a lot of ways, the actual part list depends a little on your individual plans.
Make sure to read the [planning](planning.md) document first.
Also, this list assumes that you have all necessary tools and materials for building electronics projects, like a soldering iron.

## Required Parts

| Part                       | Count   | Source                | Comment                                                              |
| -------------------------- | ------- | --------------------- | -------------------------------------------------------------------- |
| TesLight PCB               | 1       | [JLCPCB][jlcpcb]      | Can be ordered here using gerber files from the project              |
| TesLight case              | 1       | [JLCPCB][jlcpcb]      | Can be ordered here using 3D models from the project                 |
| AzDelivery ESP32 Dev Board | 1       | [Amazon][esp32]       | Or compatible with same size and layout                              |
| MPU6050 Motion Sensor      | 1       | [Reichelt][mpu6050]   | Works without but is highly recommended                              |
| Micro SD Socket            | 1       | [Amazon][sdsocket]    |                                                                      |
| 74HCT541D                  | 1       | [Reichelt][74hct541d] |                                                                      |
| Resistor 220Ω 0.6W         | 8       | [Reichelt][res220]    | 1% tolerance is fine                                                 |
| Resistor 1kΩ 0.6W          | 1       | [Reichelt][res1000]   | 0.1% tolerance recommended                                           |
| Resistor 4.7kΩ 0.6W        | 1       | [Reichelt][res4700]   | 0.1% tolerance recommended                                           |
| Resistor 2.2kΩ 0.6W        | 2       | [Reichelt][res2200]   | 1% tolerance is fine                                                 |
| SMD Capacitor 10µf 6,3V    | 1       | [Reichelt][cap10]     | 6.3V recommended, can be higher                                      |
| SMD Capacitor 100µf 6,3V   | 2       | [Reichelt][cap100]    | 6.3V recommended, can be higher                                      |
| Zener Diode 3.0V 1/2W      | 1       | [Reichelt][zener3.0]  | Warning, has no legs. If you find one with legs, go for it.          |
| Capacitor 220µf            | 2       | [Reichelt][cap220]    | 6.3-10V recommended                                                  |
| Fuse Holder                | 2       | [Reichelt][fholder]   | Depends on the number of LED's, size: 20mmx5mm, holes(PCB): 20mm <-> |
| Fuse 1-8A                  | 1       | [Reichelt][fuse]      | Depends on the number of LED's, 3A recommended. Size: 20mmx5mm       |
| JST-XH 3 Pin Male          | 9       | [Reichelt][jstxh3m]   |                                                                      |
| JST-XH 3 Pin Female        | 9       | [Reichelt][jstxh3f]   |                                                                      |
| JST-XH 4 Pin Male          | 2       | [Reichelt][jstxh4m]   |                                                                      |
| JST-XH 4 Pin Female        | 2       | [Reichelt][jstxh4f]   |                                                                      |
| JST-XH Connectors          | 35      | [Reichelt][jstxhconn] | Maybe get a few more to be on the safe side                          |
| Cables                     | depends | [Amazon][cables]      | Depends on the number of LEDs and your cable management              |

## Parts For On-Board Regulator

| Part                   | Count | Source             | Comment                               |
| ---------------------- | ----- | ------------------ | ------------------------------------- |
| LM2596T-5.0 Regulator. | 1/2   | [Reichelt][lm2596] | Make sure to get the fixed 5V version |
| Inductor min. 33µH     | 1/2   | [Reichelt][ind33]  |                                       |
| IN5824 Schottky Diode  | 1/2   | [Reichelt][in5824] | Or diode with similar specs           |
| Capacitor 680µf        | 1/2   | [Reichelt][cap680] |                                       |

## Possible LEDs

| Part              | Source            | Comment                                              |
| ----------------- | ----------------- | ---------------------------------------------------- |
| WS2812 LED Strip  | [Amazon][ws2812b] |                                                      |
| Fiber Tube        | [Amazon][fiber]   |                                                      |
| LED Injector Case | [JLCPCB][jlcpcb]  | Can be ordered here using 3D models from the project |
| LED Light Bars    | [IYC][lightbars]  | 30cm/75cm/100cm                                      |

## Optional Parts

| Part                    | Count   | Source            | Comment                                         |
| ----------------------- | ------- | ----------------- | ----------------------------------------------- |
| Heatsink for regulators | 2       | [Reichelt][hsink] | Helps to keep the temperature a little lower    |
| Shrinking tube          | depends | [Amazon][tubes]   | Will help you to have a cleaner and safer build |
| Fabric tape             | depends | [Amazon][tape]    | Will help you to have a cleaner and safer build |

## Order the PCB and Printed Parts

See [build guide](build.md#order-the-pcb-and-3d-printed-parts).

[jlcpcb]: https://cart.jlcpcb.com/quote?orderType=1&stencilLayer=2&stencilWidth=100&stencilLength=100
[esp32]: https://www.amazon.de/AZDelivery-NodeMCU-Development-Nachfolgermodell-ESP8266/dp/B071P98VTG/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=3U1PNFQJDSD0A&keywords=az%2Besp32&qid=1661668180&sprefix=az%2Besp32%2Caps%2C82&sr=8-3&th=1
[mpu6050]: https://www.reichelt.de/entwicklerboards-beschleunigung-gyroskop-3-achsen-mpu-6050-debo-sens-3axis-p253987.html?&trstct=pos_0&nbc=1
[sdsocket]: https://www.amazon.de/sourcingmap-St%C3%BCck-Oberfl%C3%A4chenmontage-Speicherkarte-Sockel/dp/B00E6PVMU2/ref=sr_1_2?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=VKTMLVUP5HRL&keywords=micro+sd+socket+pcb&qid=1661668495&sprefix=micro+sd+socket+pcb%2Caps%2C71&sr=8-2
[74hct541d]: https://www.reichelt.de/latch-octal-4-5--5-5-v-so-20-74hct-541d-nxp-p216688.html?&trstct=pos_0&nbc=1
[res220]: https://www.reichelt.de/duennschichtwiderstand-axial-0-6-w-220-ohm-1--vi-mbb02070c2200-p233705.html?CCOUNTRY=445&LANGUAGE=de&trstct=pos_14&nbc=1&&r=1
[res1000]: https://www.reichelt.de/widerstand-metallschicht-1-0-kohm-0207-0-6-w-0-1--mpr-1-00k-p12751.html?&trstct=pos_6&nbc=1
[res4700]: https://www.reichelt.de/widerstand-metallschicht-4-7-kohm-0207-0-6-w-0-1--mpr-4-70k-p12958.html?&trstct=pos_1&nbc=1
[res2200]: https://www.reichelt.de/duennschichtwiderstand-axial-0-6-w-2-2-kohm-1--vi-mbb02070c2201-p233706.html?&trstct=pos_0&nbc=1
[cap10]: https://www.reichelt.de/smd-tantal-kondensator-10-f-10v-tps-3216-10-10-p167046.html?&trstct=pos_1&nbc=1
[cap100]: https://www.reichelt.de/smd-tantal-100-f-10-6-3v-case-b-125-c-ve-2000-t491b-100u-6-p206458.html?&trstct=pos_5&nbc=1
[zener3.0]: https://www.reichelt.de/zener-diode-3-0-v-0-5-w-5-sod-80-minimelf--tzmc3v0-p280025.html?&trstct=pos_0&nbc=1
[cap220]: https://www.reichelt.de/elko-radial-220-uf-10-v-105-c-low-esr-fm-a-220u-10-p200036.html?&trstct=pos_1&nbc=1
[fuse]: https://www.reichelt.de/feinsicherung-5x20mm-traege-3-15a-tr-3-15a-p21797.html?&trstct=pos_6&nbc=1
[fholder]: https://www.reichelt.de/sicherungshalter-5x20mm-max-6-3a-500v-pl-120000-p14679.html?&trstct=pos_4&nbc=1
[jstxh3m]: https://www.reichelt.de/jst-buchsengehaeuse-1x3-polig-xh-jst-xh3p-bu-p185086.html?&trstct=pos_6&nbc=1
[jstxh3f]: https://www.reichelt.de/jst-stiftleiste-gerade-1x3-polig-xh-jst-xh3p-st-p185074.html?&trstct=pos_1&nbc=1
[jstxh4m]: https://www.reichelt.de/jst-buchsengehaeuse-1x4-polig-xh-jst-xh4p-bu-p185087.html?&trstct=pos_5&nbc=1
[jstxh4f]: https://www.reichelt.de/jst-stiftleiste-gerade-1x4-polig-xh-jst-xh4p-st-p185075.html?&trstct=pos_13&nbc=1
[jstxhconn]: https://www.reichelt.de/jst-crimpkontakt-buchse-xh-jst-xh-ckb-p185091.html?&trstct=pos_0&nbc=1
[cables]: https://www.amazon.de/LEADTOPS-3-poliger-Leiterverl%C3%A4ngerungs-Verzinntes-Traumfarbe/dp/B08JPLRXRK/ref=sr_1_13?keywords=kabel+3+adrig&qid=1662188870&sprefix=kabel+3+%2Caps%2C94&sr=8-13
[lm2596]: https://www.reichelt.de/abwaerts-schaltregler-adj-4-5--40-v-5-0-v-3-a-to-220-5-lm-2596-t5-0-p109365.html?&trstct=pos_1&nbc=1
[ind33]: https://www.reichelt.de/smd-power-induktivitaet-eisenpulverkern-100-h-pan-etqp5m101ygk-p245793.html?&trstct=pos_2&nbc=1
[in5824]: https://www.reichelt.de/schottkydiode-40-v-5-a-do-214ab-smc-sk-54c-p146598.html?&trstct=pos_3&nbc=1
[cap680]: https://www.reichelt.de/elko-radial-680-uf-25-v-105-c-low-esr-fm-a-680u-25-p200061.html?&trstct=pos_6&nbc=1
[ws2812b]: https://www.amazon.de/BTF-LIGHTING-adressierbar-Vollfarbiger-wasserdichte-DIY-Projekte/dp/B088BRY2SH/ref=sr_1_8?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1OSVQ80JPAJDG&keywords=ws2812b+strip+60%2Fm&qid=1662188470&sprefix=ws2812b+strip+60%2Fm%2Caps%2C82&sr=8-8
[fiber]: https://www.amazon.de/TABEN-Autoseitenlicht-LED-Beleuchtung-Innenatmosph%C3%A4re-Neonstreifenleuchten/dp/B08DLJ2JJ1/ref=sr_1_7?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=taben+fiber&qid=1662190164&sr=8-7
[lightbars]: https://individualiseyourcar.com/en/ambitrim-digital-rgb-led-ambient-light-components.html
[hsink]: https://www.reichelt.de/kuehlkoerper-15-2-mm-alu-26-k-w-to-220-fk-242-sa-220-o-p228044.html?PROVID=2788&gclid=CjwKCAiA9qKbBhAzEiwAS4yeDbgDFQnpxuPlW2l_6iWmSpzoQlH7U5ZTtUvpgh0bGrCp32GTk7xljBoCXLsQAvD_BwE
[tubes]: https://www.amazon.de/ChiliTec-12000058-Chilitec-Schrumpfschlauch-Sortiment-100-teilig/dp/B003H9CJ1Y/ref=sr_1_20_mod_primary_new?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1Q5IY5D32HNLR&keywords=schrumpfschlauch&qid=1662189028&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=schrumpfschlauch%2Caps%2C115&sr=8-20
[tape]: https://www.amazon.de/tesa-Gewebeband-Witterungsbest%C3%A4ndiges-Panzertape-Befestigen/dp/B000KT7E0U/ref=sr_1_5?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1U7SK4BSKYGK0&keywords=Gewebeband&qid=1662189088&sprefix=gewebeban%2Caps%2C94&sr=8-5
