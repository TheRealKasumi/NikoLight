# TesLight's REST API

TesLight offers a simple json based API that can be consumed by clients to read information from the controller, adjust settings and transfer files.
Generally the API is documentend in the [Postman collection](TesLight%20API.postman_collection.json) as well as the [OpenApi document](OpenApi.yaml).
As addition the well known Swagger UI is available under `192.168.4.1/ui/swagger.html` once you are connected to the controller.
It can be used to explore and experiment with the API.
In this document we will give some additional information, where we think it is necessary.

## LED Configuration and the `animationSettings`

You can use the `api/config/led` endpoint to `GET` and `POST` the LED configuration.
It will contain information about the LED setup and what the MCU is supposed to display.
Most parts of this endpoint should be self explaining.
However we know that the `animationSettings` array can be a little confusing.

Since TesLight implements very different animations, they also require very differnt parameters.
We found it hard to introduce fixed names for these parameters since they are different depenting on the animation type.
This is the reason why the `animationSettings`-array was introduced.
It can/must contain addition information about how the animation is rendered depending on the selected animation.
Generally the array contains 25 elements.
Each of these elements represents a byte that can be used by an animation.
This means each field must be in the range 0~255.
To bring some light into the dark which field to use with which animation, you can refer to the following table.
There all combinations are listed.

| Animation             | Rainbow      | Sparkl              | Gradient      | Static      | Color Bar     | Rainbow Motion | Gradient Motion | FSEQ          |
| --------------------- | ------------ | ------------------- | ------------- | ----------- | ------------- | -------------- | --------------- | ------------- |
| Type                  | 0            | 1                   | 2             | 3           | 4             | 5              | 6               | Don't change  |
| animationSettings[0]  | Rainbow Mode | Spawn Position      | Gradient Mode | Color Red   | Bar Mode      | Rainbow Mode   | Gradient Mode   | Don't change  |
| animationSettings[1]  | /            | Spark Count         | Color Red 1   | Color Green | Color Red 1   | Sensor Value   | Sensor Value    | Don't change  |
| animationSettings[2]  | /            | Color Red           | Color Green 1 | Color Blue  | Color Green 1 | /              | Color Red 1     | Don't change  |
| animationSettings[3]  | /            | Color Green         | Color Blue 1  | /           | Color Blue 1  | /              | Color Green 1   | Don't change  |
| animationSettings[4]  | /            | Color Blue          | Color Red 2   | /           | Color Red 2   | /              | Color Blue 1    | Don't change  |
| animationSettings[5]  | /            | Friction            | Color Green 2 | /           | Color Green 2 | /              | Color Red 2     | Don't change  |
| animationSettings[6]  | /            | Fading              | Color Blue 2  | /           | Color Blue 2  | /              | Color Green 2   | Don't change  |
| animationSettings[7]  | /            | Tail                | /             | /           | /             | /              | Color Blue 2    | Don't change  |
| animationSettings[8]  | /            | Birth Rate          | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[9]  | /            | Spawn Variance      | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[10] | /            | Speed Variance      | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[11] | /            | Brightness Variance | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[12] | /            | Friction Variance   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[13] | /            | Fading Variance     | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[14] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[15] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[16] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[17] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[18] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[19] | /            | /                   | /             | /           | /             | /              | /               | Don't change  |
| animationSettings[20] | Reserved     | Reserved            | Reserved      | Reserved    | Reserved      | Reserved       | Reserved        | File ID       |
| animationSettings[21] | Reserved     | Reserved            | Reserved      | Reserved    | Reserved      | Reserved       | Reserved        | File ID       |
| animationSettings[22] | Reserved     | Reserved            | Reserved      | Reserved    | Reserved      | Reserved       | Reserved        | File ID       |
| animationSettings[23] | Reserved     | Reserved            | Reserved      | Reserved    | Reserved      | Reserved       | Reserved        | File ID       |
| animationSettings[24] | Reserved     | Reserved            | Reserved      | Reserved    | Reserved      | Reserved       | Reserved        | Previous type |

| Rainbow Mode | Value | Description                           |
| ------------ | ----- | ------------------------------------- |
| Solid        | 0     | All LEDs show the same rainbow color  |
| Linear       | 1     | Render a linear rainbow to the LEDs   |
| Center       | 2     | Render a centered rainbow to the LEDs |

| Spawn Position | Value | Description                                            |
| -------------- | ----- | ------------------------------------------------------ |
| Left           | 0     | All particles spawn on the left, use offset to adjust  |
| Right          | 1     | All particles spawn on the right, use offset to adjust |
| Center         | 2     | All particles spawn in the center                      |
| Random         | 3     | All particles spawn at a random position               |

| Gradient Mode | Value | Description                                                                   |
| ------------- | ----- | ----------------------------------------------------------------------------- |
| Linear        | 0     | The gradient is rendered from one side to the other                           |
| Center        | 1     | The gradient is rendered from the center to the sides or the other way around |

| Bar Mode              | Value | Description                                                                                        |
| --------------------- | ----- | -------------------------------------------------------------------------------------------------- |
| Linear, hard borders  | 0     | Color bars moving from one side to the other. Hard borders are rendered.                           |
| Linear, smooth border | 1     | Color bars moving from one side to the other. Soft borders are rendered.                            |
| Center, hard borders  | 2     | Color bars moving from the middle to the sides or the other way around. Hard borders are rendered. |
| Center, smooth border | 3     | Color bars moving from the middle to the sides or the other way around. Soft borders are rendered.  |