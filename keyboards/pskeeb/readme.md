# pskeeb

![pskeeb](to be uploaded)

The Portable Split Keeb is a 36 key diodeless split keyboard, designed with simplicity and portability in mind, aiming to provide a more ergonomic (stronger column stagger) corne-like layout with a simple, yet powerful, easy to assemble and cheap design.. It is inspired by the following projects:

 - [corne](https://github.com/foostan/crkbd)
 - [cantor](https://github.com/diepala/cantor)
 - [bgkeeb](https://github.com/sadekbaroudi/bgkeeb)


* Keyboard Maintainer: [Klesh Wong](https://github.com/klesh)
* Hardware Supported: Blackpill STM32F401
* Hardware Availability: https://github.com/diepala/pskeeb

Make example for this keyboard (after setting up your build environment):

    make pskeeb:default

Flashing example for this keyboard:

    make pskeeb:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the top left key and plug in the keyboard. For the right side, hold the top right key and plug the keyboard.
* **Physical reset button**: 
  * Press and hold the BOOT0 button.
  * Press and release the NRST button.
  * Release the BOOT0 button.
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
