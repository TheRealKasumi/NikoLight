# Part List

This document contains the full list of all parts to build TesLight.
The list has been split into required and optional parts.
Since TesLight can be modified in a lot of ways, the actual part list depends a little on your individual plans.
Make sure to read the [planning](planning.md) document first.
Also, this list assumes that you have all necessary tools and materials for building electronics projects, like a soldering iron.

## Assembled PCB by JLCPCB

Most parts will come fully assembled with the board.
The parts are already contained in the fiels for the board.
You only have to insert the ESP32 board and the MPU6050 manually.

| Part                       | Count | Source                | Comment                                                       |
| -------------------------- | ----- | --------------------- | ------------------------------------------------------------- |
| TesLight PCB (SMT Service) | 1     | [JLCPCB][jlcpcb]      | Can be ordered from JLCPCB using board files from the project |
| TesLight Case              | 1     | [JLCPCB][jlcpcb]      | Can be ordered from JLCPCB using 3D models from the project   |
| AzDelivery ESP32 Dev Board | 1     | [Amazon][esp32]       | Or compatible with same size and layout                       |
| MPU6050 Motion Sensor      | 1     | [Reichelt][mpu6050]   | Works without but is highly recommended                       |
| JST-XH 2 Pin Male          | 2     | [Reichelt][jstxh2m]   |                                                               |
| JST-XH 3 Pin Male          | 8     | [Reichelt][jstxh3m]   |                                                               |
| JST-XH 4 Pin Male          | 2     | [Reichelt][jstxh4m]   |                                                               |
| JST-XH Connectors          | 36    | [Reichelt][jstxhconn] | Maybe get a few more to be on the safe side                   |

## Self Assembly

| Part                       | Count | Source                | Comment                                                       |
| -------------------------- | ----- | --------------------- | ------------------------------------------------------------- |
| TesLight PCB               | 1     | [JLCPCB][jlcpcb]      | Can be ordered from JLCPCB using board files from the project |
| TesLight Case              | 1     | [JLCPCB][jlcpcb]      | Can be ordered from JLCPCB using 3D models from the project   |
| AzDelivery ESP32 Dev Board | 1     | [Amazon][esp32]       | Or compatible with same size and layout                       |
| MPU6050 Motion Sensor      | 1     | [Reichelt][mpu6050]   | Works without but is highly recommended                       |
| Micro SD Socket            | 1     | [Amazon][sdsocket]    |                                                               |
| 74HCT541D                  | 1     | [Reichelt][74hct541d] |                                                               |
| LM2596T-5.0 Regulator.     | 1/2   | [Reichelt][lm2596]    | Make sure to get the fixed 5V version                         |
| Inductor min. 33µH         | 1/2   | [Reichelt][ind33]     |                                                               |
| IN5824 Schottky Diode      | 1/2   | [Reichelt][in5824]    |                                                               |
| Capacitor 680µf            | 1/2   | [Reichelt][cap680]    |                                                               |
| SMD Capacitor 10µf 10V     | 1     | [Reichelt][cap10]     |                                                               |
| SMD Capacitor 100µf 10V    | 3     | [Reichelt][cap100]    |                                                               |
| SMD Capacitor 220µf 10V    | 2     | [Reichelt][cap220]    |                                                               |
| SMD Resistor 220Ω 125mW    | 9     | [Reichelt][res220]    |                                                               |
| SMD Resistor 1kΩ 125mW     | 1     | ???                   |                                                               |
| SMD Resistor 4.7kΩ 125mW   | 1     | [Reichelt][res4700]   |                                                               |
| SMD Resistor 2.2kΩ 125mW   | 2     | [Reichelt][res2200]   |                                                               |
| SMD Resistor 6.65Ω 500mW   | 1     | [JCSC][res6]          |                                                               |
| Zener Diode 3.0V           | 1     | [Reichelt][zener3.0]  |                                                               |
| Fuse and Holder 5A         | 2     | [Reichelt][fuse]      |                                                               |
| Poly Fuse 3A               | 1     | [Reichelt][pfuse]     |                                                               |
| JST-XH 2 Pin Male          | 2     | [Reichelt][jstxh2m]   |                                                               |
| JST-XH 2 Pin Female        | 2     | [Reichelt][jstxh2f]   |                                                               |
| JST-XH 3 Pin Male          | 8     | [Reichelt][jstxh3m]   |                                                               |
| JST-XH 3 Pin Female        | 8     | [Reichelt][jstxh3f]   |                                                               |
| JST-XH 4 Pin Male          | 2     | [Reichelt][jstxh4m]   |                                                               |
| JST-XH 4 Pin Female        | 2     | [Reichelt][jstxh4f]   |                                                               |
| JST-XH Connectors          | 36    | [Reichelt][jstxhconn] | Maybe get a few more to be on the safe side                   |

## Possible LEDs

| Part              | Source            | Comment                                              |
| ----------------- | ----------------- | ---------------------------------------------------- |
| WS2812 LED Strip  | [Amazon][ws2812b] |                                                      |
| Fiber Tube        | [Amazon][fiber]   |                                                      |
| LED Injector Case | [JLCPCB][jlcpcb]  | Can be ordered here using 3D models from the project |
| LED Light Bars    | [IYC][lightbars]  | 30 cm/75 cm/100 cm                                   |
| LED Light Bars Alternative   | [Herwins][lightbars_symphony]  | 25 cm/75 cm/100 cm (Symphony version are WS2812, check polarity before use, see [build](build.md). These are very small and are masked with black foil and are thus dimmed, which is great as we can use the full color spectrum of the LEDs and at the same time do not get blind while driving.)           |

## Optional Parts

| Part                    | Count   | Source             | Comment                                         |
| ----------------------- | ------- | ------------------ | ----------------------------------------------- |
| Heatsink for Regulators | 2       | [Reichelt][hsink]  | Helps to keep the temperature a little lower    |
| Temperature Sensors     | 2       | [Reichelt][tsense] | Not required but a safety feature               |
| Cooling Fan 30x30       | 1       | [Reichelt][fan]    | Not required but nice to have                   |
| Shrinking tube          | depends | [Amazon][tubes]    | Will help you to have a cleaner and safer build |
| Fabric tape             | depends | [Amazon][tape]     | Will help you to have a cleaner and safer build |

[jlcpcb]: https://cart.jlcpcb.com/quote?orderType=1&stencilLayer=2&stencilWidth=100&stencilLength=100
[esp32]: https://www.amazon.de/AZDelivery-NodeMCU-Development-Nachfolgermodell-ESP8266/dp/B071P98VTG/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=3U1PNFQJDSD0A&keywords=az%2Besp32&qid=1661668180&sprefix=az%2Besp32%2Caps%2C82&sr=8-3&th=1
[mpu6050]: https://www.reichelt.de/entwicklerboards-beschleunigung-gyroskop-3-achsen-mpu-6050-debo-sens-3axis-p253987.html?&trstct=pos_0&nbc=1
[jstxh2m]: https://www.reichelt.de/jst-buchsengehaeuse-1x2-polig-xh-jst-xh2p-bu-p185085.html?&trstct=pos_0&nbc=1
[jstxh2f]: https://www.reichelt.de/jst-stiftleiste-gerade-1x2-polig-xh-jst-xh2p-st-p185073.html?&trstct=pos_0&nbc=1
[jstxh3m]: https://www.reichelt.de/jst-buchsengehaeuse-1x3-polig-xh-jst-xh3p-bu-p185086.html?&trstct=pos_6&nbc=1
[jstxh3f]: https://www.reichelt.de/jst-stiftleiste-gerade-1x3-polig-xh-jst-xh3p-st-p185074.html?&trstct=pos_1&nbc=1
[jstxh4m]: https://www.reichelt.de/jst-buchsengehaeuse-1x4-polig-xh-jst-xh4p-bu-p185087.html?&trstct=pos_5&nbc=1
[jstxh4f]: https://www.reichelt.de/jst-stiftleiste-gerade-1x4-polig-xh-jst-xh4p-st-p185075.html?&trstct=pos_13&nbc=1
[jstxhconn]: https://www.reichelt.de/jst-crimpkontakt-buchse-xh-jst-xh-ckb-p185091.html?&trstct=pos_0&nbc=1
[sdsocket]: https://www.amazon.de/sourcingmap-St%C3%BCck-Oberfl%C3%A4chenmontage-Speicherkarte-Sockel/dp/B00E6PVMU2/ref=sr_1_2?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=VKTMLVUP5HRL&keywords=micro+sd+socket+pcb&qid=1661668495&sprefix=micro+sd+socket+pcb%2Caps%2C71&sr=8-2
[74hct541d]: https://www.reichelt.de/latch-octal-4-5--5-5-v-so-20-74hct-541d-nxp-p216688.html?&trstct=pos_0&nbc=1
[lm2596]: https://www.reichelt.de/abwaerts-schaltregler-adj-4-5--40-v-5-0-v-3-a-to-220-5-lm-2596-t5-0-p109365.html?&trstct=pos_1&nbc=1
[ind33]: https://www.reichelt.de/smd-power-induktivitaet-eisenpulverkern-100-h-pan-etqp5m101ygk-p245793.html?&trstct=pos_2&nbc=1
[in5824]: https://www.reichelt.de/schottkydiode-40-v-5-a-do-214ab-smc-sk-54c-p146598.html?&trstct=pos_3&nbc=1
[cap680]: https://www.reichelt.de/elko-radial-680-uf-25-v-105-c-low-esr-fm-a-680u-25-p200061.html?&trstct=pos_6&nbc=1
[cap10]: https://www.reichelt.de/smd-tantal-kondensator-10-f-10v-tps-3216-10-10-p167046.html?&trstct=pos_1&nbc=1
[cap100]: https://www.reichelt.de/smd-tantal-100-f-10v-125-c-t491c-100u-10-p206478.html?&trstct=pos_0&nbc=1
[cap220]: https://www.reichelt.de/smd-tantal-220-uf-10-v-2000-h-posc-220-10d-40-p145946.html?&trstct=pos_0&nbc=1
[res220]: https://www.reichelt.de/smd-widerstand-0805-220-ohm-125-mw-1--smd-0805-220-p32878.html?&trstct=pos_1&nbc=1
[res4700]: https://www.reichelt.de/smd-widerstand-0805-4-7-kohm-125-mw-1--smd-0805-4-70k-p32894.html?&trstct=pos_6&nbc=1
[res2200]: https://www.reichelt.de/smd-widerstand-0805-2-2-kohm-125-mw-1--smd-0805-2-20k-p32890.html?&trstct=pos_13&nbc=1
[res6]: https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_YAGEO-RC1210FR-076R65L_C309592.html
[zener3.0]: https://www.reichelt.de/zenerdiode-3-3-v-300mw-sod-323-mm3z3v3t1g-ons-p219513.html?&trstct=pos_0&nbc=1
[fuse]: https://www.reichelt.de/smd-sicherungshalter-mit-sicherung-5-0-a-125-v-superflink-litt-0154005-dr-p229204.html?&trstct=pos_14&nbc=1
[pfuse]: https://www.reichelt.de/rueckstellende-sicherungen-8-a-litt-ruef400-p242414.html?&trstct=pos_0&nbc=1
[ws2812b]: https://www.amazon.de/BTF-LIGHTING-adressierbar-Vollfarbiger-wasserdichte-DIY-Projekte/dp/B088BRY2SH/ref=sr_1_8?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1OSVQ80JPAJDG&keywords=ws2812b+strip+60%2Fm&qid=1662188470&sprefix=ws2812b+strip+60%2Fm%2Caps%2C82&sr=8-8
[fiber]: https://www.amazon.de/TABEN-Autoseitenlicht-LED-Beleuchtung-Innenatmosph%C3%A4re-Neonstreifenleuchten/dp/B08DLJ2JJ1/ref=sr_1_7?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=taben+fiber&qid=1662190164&sr=8-7
[lightbars]: https://individualiseyourcar.com/en/ambitrim-digital-rgb-led-ambient-light-components.html
[lightbars_symphony]: https://de.aliexpress.com/item/1005004297563628.html
[hsink]: https://www.reichelt.de/kuehlkoerper-15-2-mm-alu-26-k-w-to-220-fk-242-sa-220-o-p228044.html?PROVID=2788&gclid=CjwKCAiA9qKbBhAzEiwAS4yeDbgDFQnpxuPlW2l_6iWmSpzoQlH7U5ZTtUvpgh0bGrCp32GTk7xljBoCXLsQAvD_BwE
[tsense]: https://www.reichelt.de/digital-thermometer-1-wire--0-5-c-to-92-ds-18b20-p58169.html?&trstct=pos_0&nbc=1
[fan]: https://www.amazon.de/gp/product/B071JN6HKM/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&psc=1
[tubes]: https://www.amazon.de/ChiliTec-12000058-Chilitec-Schrumpfschlauch-Sortiment-100-teilig/dp/B003H9CJ1Y/ref=sr_1_20_mod_primary_new?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1Q5IY5D32HNLR&keywords=schrumpfschlauch&qid=1662189028&sbo=RZvfv%2F%2FHxDF%2BO5021pAnSA%3D%3D&sprefix=schrumpfschlauch%2Caps%2C115&sr=8-20
[tape]: https://www.amazon.de/tesa-Gewebeband-Witterungsbest%C3%A4ndiges-Panzertape-Befestigen/dp/B000KT7E0U/ref=sr_1_5?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&crid=1U7SK4BSKYGK0&keywords=Gewebeband&qid=1662189088&sprefix=gewebeban%2Caps%2C94&sr=8-5
