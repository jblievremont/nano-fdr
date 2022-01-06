nano-fdr
========

A minimalistic autonomous flight data recorder for home-made aircraft 🛩️ and
rockets 🚀.

The main goal is to have a single battery-powered board that can:

 - Record sensor data (mainly accelerometer) to persistent memory during flight
 - Allow retrieval of recorded data for off-board analysis


Hardware
--------

This project is meant to run on the [Adafruit Circuit Playground Classic](https://www.adafruit.com/product/3000).
It was chosen due to its comprehensiveness in terms of sensors, accessibility in
terms of UI (buttons, RGB NeoPixels) and affordability. Additionally, it can be
powered by a LiPo battery or with a 3xAAA battery pack.

The on-board EEPROM has a capacity of 1024 bytes. In the current version, each
data point stores 3 `float`s (one for each accelerometer axis), which takes 12
bytes.

The current number of stored data points is stored in the EEPROM's first byte,
which leaves room for 1023 / 12 = 85 data points. With a sampling period of
60ms, this allows storage of a bit more than 5 seconds of accelerometer data.


User manual
-----------

The on-board switch is used to select the mode at startup: when the switch is
set to ➕ (left position when holding the USB port up), the board starts in
recording mode; otherwise, the board starts in retrieval mode.

The actual state of the board will be reflected using the onboard NeoPixels:
* Green 🟢 pixels indicate that the board is in retrieval mode; at this stage,
  the board opens the serial port (at 115200 bauds) and waits for the other end
  to open the port, which will trigger the board to dump the stored data as a
  JSON array. A black ⚫ pixel cycles during the dump, all pixels turn off once
  done.
* Yellow 🟡 pixels indicate that the board is in recording standby mode;
  at this stage pressing the right button will start recording.
* One yellow 🟡 cycling pixel indicates that the board is recording data.
* Red 🔴 pixels indicate that the board's memory is full.

The dumped JSON looks like this:

    [
      {"x":-0.57,"y":-0.84,"z":9.31},
      {"x":-0.69,"y":-2.15,"z":5.52},
      /* ... */
      {"x":-0.50,"y":-0.57,"z":7.32}
    ]

Dump utility
------------

Folder `nano-fdr-dump` contains a Python script that relies on
[pySerial](https://github.com/pyserial/pyserial) to dump the recorded data.

Script has been tested on Windows 11 with Python 3.10.
