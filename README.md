# WordClock-15x15

## Upload data/ to SPIFFS:

```
pio run --target buildfs # only for the 1st time or when files changes
pio run --target uploadfs
```

Please remember that the full path for a file must be < 32 bits (it can't be `/js/a-very-very-very-very-long-filename.min.js`).

## Software

- [VS Code](https://code.visualstudio.com/download)
- [PlatformIO](https://platformio.org/)

## Libraries

- https://github.com/PaulStoffregen/Time.git
- TFT_eSPI@1.4.21
- Button2@1.0.0
- ArduinoJson
- ESP Async WebServer
- WebSockets
- https://github.com/dx168b/async-mqtt-client.git
- https://github.com/alanswx/ESPAsyncWiFiManager.git
- https://github.com/samguyer/FastLED.git
- https://github.com/Jorgen-VikingGod/LEDMatrix.git

All other required packages are provided by PlatformIO

## API

You can get and set all values by simple GET requests

### /all

You can query all current values by

```shell
http://<address>/all
```

Example response:

```javascript
[
  {
    name: 'power',
    label: 'Power',
    type: 'Boolean',
    section: { name: 'Global', layout: 'col-sm-12', color: 'shadow-sm' },
    value: 1,
  },
  {
    name: 'tourette',
    label: 'Tourette',
    type: 'Boolean',
    section: { name: 'Global', layout: 'col-sm-12', color: 'shadow-sm' },
    value: 1,
  },
  {
    name: 'daylight',
    label: 'Daylight Saving',
    type: 'Boolean',
    section: { name: 'Global', layout: 'col-sm-12', color: 'shadow-sm' },
    value: 0,
  },
  {
    name: 'autoBrightness',
    label: 'Auto Brightness',
    type: 'Boolean',
    section: { name: 'Global', layout: 'col-sm-12', color: 'shadow-sm' },
    value: 0,
  },
  {
    name: 'brightness',
    label: 'Brightness',
    type: 'Number',
    section: { name: 'Global', layout: 'col-sm-12', color: 'shadow-sm' },
    value: 125,
    min: 1,
    max: 255,
  },
  {
    name: 'colorEffect',
    label: 'Color Effect',
    type: 'Select',
    section: { name: 'Foreground', layout: 'col-xl-4 col-lg-6', color: 'border-primary shadow-sm' },
    value: 0,
    options: ['Solid', 'Random', 'Hue'],
  },
  {
    name: 'foregroundColor',
    label: 'Color',
    type: 'Color',
    section: { name: 'Foreground', layout: 'col-xl-4 col-lg-6', color: 'border-primary shadow-sm' },
    value: '255,255,255',
  },
  {
    name: 'background',
    label: 'Background',
    type: 'Select',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 3,
    options: ['Black', 'Solid Color', 'Plasma', 'Palette Noise'],
  },
  {
    name: 'backgroundBrightness',
    label: 'Brightness',
    type: 'Number',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 50,
    min: 1,
    max: 255,
  },
  {
    name: 'noisePaletteSpeedX',
    label: 'Speed X',
    type: 'Number',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 3,
    min: 0,
    max: 24,
  },
  {
    name: 'noisePaletteSpeedY',
    label: 'Speed Y',
    type: 'Number',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 2,
    min: 0,
    max: 24,
  },
  {
    name: 'noisePaletteSpeedZ',
    label: 'Speed Z',
    type: 'Number',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 3,
    min: 0,
    max: 24,
  },
  {
    name: 'backgroundColor',
    label: 'Color',
    type: 'Color',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: '0,0,0',
  },
  {
    name: 'palette',
    label: 'Palette',
    type: 'Select',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 0,
    options: [
      'Rainbow',
      'Rainbow Stripe',
      'Cloud',
      'Lava',
      'Ocean',
      'Forest',
      'Party',
      'Heat',
      'Ice',
      'Icy Blue',
      'Snow',
      'Red & White',
      'Blue & White',
      'Fairy',
      'Retro C9',
      'Red, Green & White',
      'Holly',
      'Sunset_Real',
      'es_rivendell_15',
      'es_ocean_breeze_036',
      'rgi_15',
      'retro2_16',
      'Analogous_1',
      'es_pinksplash_08',
      'Coral_reef',
      'es_ocean_breeze_068',
      'es_pinksplash_07',
      'es_vintage_01',
      'departure',
      'es_landscape_64',
      'es_landscape_33',
      'rainbowsherbet',
      'gr65_hult',
      'gr64_hult',
      'GMT_drywet',
      'ib_jul01',
      'es_vintage_57',
      'ib15',
      'Fuschia_7',
      'es_emerald_dragon_08',
      'lava',
      'fire',
      'Colorfull',
      'Magenta_Evening',
      'Pink_Purple',
      'es_autumn_19',
      'BlacK_Blue_Magenta_White',
      'BlacK_Magenta_Red',
      'BlacK_Red_Magenta_Yellow',
      'Blue_Cyan_Yellow',
      'Black & Green Striped',
      'Black & White Striped',
    ],
  },
  {
    name: 'cyclePalettes',
    label: 'Cycle Palettes',
    type: 'Boolean',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 0,
  },
  {
    name: 'paletteDuration',
    label: 'Palette Duration',
    type: 'Number',
    section: { name: 'Background', layout: 'col-xl-4 col-lg-6', color: 'border-success shadow-sm' },
    value: 60,
    min: 1,
    max: 255,
  },
  {
    name: 'touretteModes',
    label: 'Tourette Mode',
    type: 'Select',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 1,
    options: ['Start', 'End'],
  },
  {
    name: 'randomTouretteMode',
    label: 'Random Cycle',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 1,
  },
  {
    name: 'cycleTouretteMode',
    label: 'Cycle Mode',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 0,
  },
  {
    name: 'touretteCycleDuration',
    label: 'Cycle Duration',
    type: 'Number',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 60,
    min: 1,
    max: 255,
  },
  {
    name: 'touretteStartWords',
    label: 'Start Word',
    type: 'Select',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 2,
    options: [
      'sau',
      'bazi',
      'saubazi',
      'penner',
      'saupenner',
      'arsch',
      'fettarsch',
      'arschsaft',
      'sack',
      'saftsack',
      'fettsack',
      'trottel',
      'penis',
      'arschpenis',
      'saftpenis',
    ],
  },
  {
    name: 'randomTouretteStart',
    label: 'Random Start',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 1,
  },
  {
    name: 'cycleTouretteStart',
    label: 'Cycle Start',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 0,
  },
  {
    name: 'touretteMiddleWords',
    label: 'Middle Word',
    type: 'Select',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 6,
    options: [
      'verdammte',
      'kack',
      'verkackte',
      'fick',
      'verfickte',
      'scheiss',
      'fick scheiss',
      'kack scheiss',
      'verdammte scheiss',
      'verkackte scheiss',
      'verfickte scheiss',
    ],
  },
  {
    name: 'randomTouretteMiddle',
    label: 'Random Middle',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 1,
  },
  {
    name: 'cycleTouretteMiddle',
    label: 'Cycle Middle',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 0,
  },
  {
    name: 'touretteEndWords',
    label: 'End Word',
    type: 'Select',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 15,
    options: [
      'sau',
      'bazi',
      'drecksau',
      'drecks bazi',
      'drecksaubazi',
      'arsch',
      'arschloch',
      'penner',
      'drecks penner',
      'arschlochpenner',
      'kobold',
      'hodenkobold',
      'saukobold',
      'drecks kobold',
      'drecksaukobold',
      'arschkobold',
      'dummy',
      'drecks dummy',
      'arschdummy',
      'hodendummy',
    ],
  },
  {
    name: 'randomTouretteEnd',
    label: 'Random End',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 1,
  },
  {
    name: 'cycleTouretteEnd',
    label: 'Cycle End',
    type: 'Boolean',
    section: { name: 'Tourette', layout: 'col-xl-4 col-lg-6', color: 'border-secondary shadow-sm' },
    value: 0,
  },
];
```

### /get

You can get a value of a specific attribute by using the /get route with following parameters

#### single value

| Parameter | Description       | Example  |
| --------- | ----------------- | -------- |
| name      | name of attribute | tourette |

```shell
http://<address>/get?name=<attribute-name>
```

Example URI:

```shell
http://192.168.188.70/get?name=tourette
```

Example response:

```shell
1
```

#### color value

| Parameter | Description       | Example         |
| --------- | ----------------- | --------------- |
| name      | name of attribute | backgroundColor |

```shell
http://<address>/get?name=<attribute-name>
```

Example URI:

```shell
http://192.168.188.70/get?name=backgroundColor
```

Example response:

```shell
0,0,0
```

### /set

You can modify specific attribute by using the /set route with following parameters

#### single value

| Parameter | Description        | Example  |
| --------- | ------------------ | -------- |
| name      | name of attribute  | tourette |
| value     | value of attribute | 0        |

```shell
http://<address>/set?name=<attribute-name>&value=<attribute-value>
```

Example URI:

```shell
http://192.168.188.70/set?name=tourette&value=0
```

Example response:

```shell
0
```

#### color value

| Parameter | Description              | Example         |
| --------- | ------------------------ | --------------- |
| name      | name of attribute        | foregroundColor |
| r         | red value of attribute   | 255             |
| g         | green value of attribute | 0               |
| b         | blue value of attribute  | 127             |

```shell
http://<address>/set?name=<attribute-name>&r=<red>&g=<green>&b=<blue>
```

Example URI:

```shell
http://192.168.188.70/set?name=foregroundColor&r=255&g=0&b=127
```

Example response:

```shell
"255,0,127"
```
