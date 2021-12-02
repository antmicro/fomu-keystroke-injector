=======================
Fomu keystroke injector
=======================

Copyright (c) 2021 `Antmicro <https://www.antmicro.com>`_

What is Fomu keystroke injector?
--------------------------------

This is a project implementing keystroke injector as an application on the `Fomu`_ FPGA board.
It's built using the `LiteX`_ framework
It supports keystroke sequences described with the use of the `Duckyscript`_ language
and a reference encoder from the `USB Rubber Ducky`_ project.

Building
--------

Fomu keystroke injector requires a Python 3.7 (or newer) interpreter,
RISC-V toolchain with GCC compiler, Meson (tested with version 0.60) and an `iCE40 FPGA toolchain`_
with `Yosys`_, `nextpnr`_, `icepack`_ and `dfu-utils`_ programs.

All other dependencies are included as Git submodules, which must be also pulled
while cloning the repository::

    $ git clone --recurse-submodules https://github.com/antmicro/fomu-keystroke-injector.git

To build the injector execute the following commands::

    $ cd hw
    $ ./bitstream.py

After the building process the DFU image will be placed in the following path
`hw/build/kosagi_fomu_pvt/kosagi_fomu_pvt.dfu`.

Installation
------------

The injector uses the `Foboot`_ bootloader to allow loading the bitstream
to the board without requiring an external programmer.
As Foboot supports USB DFU class for programming, the bitstream can be downloaded
onto the board by using `dfu-util` program::

    $ dfu-util -D kosagi_fomu_pvt.dfu

.. note::
    Foboot v2.0.4 or newer is required for autoboot.

Usage
-----

The keystroke injector will present itself to the host as an HID keyboard device.

If the keystroke sequence payload is on the device, it can be executed
by bridging the two leftmost pads on Fomu for a moment.

To download the keystroke sequence payload to the device, you need to switch it to programming mode.
This can be done in two ways:

1. By bridging the two rightmost pads on Fomu while the injector application is starting,
   until the RGB LED starts glowing in blue color.
2. By sending a DFU detach request using `dfu-util`::

    $ dfu-util -e

After the application switches to the programming mode, you can use `dfu-util` to download
the payload or to update the keystroke injector bitstream/application.

Downloading the injector payload::

    $ dfu-util -a0 -D payload.bin

Updating the bitstream::

    $ dfu-util -a1 -D kosagi_fomu_pvt.dfu

.. note::
    Because writing to the flash memory isn't optimized yet in the application,
    it's recommended to update the bitstream from Foboot and use the programming mode
    in the application only for installing the payload.

Preparing keystroke payload
---------------------------

The Fomu keystroke injector is compatible with payloads for `USB Rubber Ducky`_.
`Duckyscript`_ syntax description and `encoder`_ downloads are available on `USB Rubber Ducky wiki`_.

Write an example script to file `test.txt`::

    STRING Hello, world!
    DELAY 500
    CTRL-ALT DEL

To encode the script you need to use the Duckyscript `encoder`_, which requires Java Runtime Environment installed::

    $ java -jar duckencoder.jar -i test.txt -o test.bin

The encoder will generate the `test.bin` file containing the payload with the keystroke sequences ready to be downloaded to the injector.


.. _`Foboot`: https://github.com/im-tomu/foboot
.. _`Fomu`: https://tomu.im/fomu.html
.. _`dfu-utils`: http://dfu-util.sourceforge.net/
.. _`Duckyscript`: https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript
.. _`encoder`: https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Downloads
.. _`iCE40 FPGA toolchain`: https://github.com/YosysHQ/oss-cad-suite-build
.. _`icepack`: https://github.com/YosysHQ/icestorm
.. _`LiteX`: https://github.com/enjoy-digital/litex
.. _`nextpnr`: https://github.com/YosysHQ/nextpnr
.. _`USB Rubber Ducky`: https://hak5.org/products/usb-rubber-ducky-deluxe
.. _`USB Rubber Ducky wiki`: https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript
.. _`Yosys`: https://github.com/YosysHQ/yosys
