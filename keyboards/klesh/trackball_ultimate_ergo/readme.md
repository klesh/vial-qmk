# Trackball Ultimate Ergo


* trackball Maintainer: [Klesh Wong](https://github.com/klesh)
* Hardware Supported: Atmega32u4
* Hardware Availability: [Github Repository](https://github.com/klesh/pskeeb)

Make example for this trackball (after setting up your build environment):

    qmk compile -kb klesh/trackball_ultimate_ergo -km default

Flashing example for this trackball:

    qmk flash -kb klesh/trackball_ultimate_ergo -km default

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the trackball
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
