# trinket-midi-adapter

MIDI-to-USB interface with a 5V Trinket.
Based on [TrinketMIDI](https://github.com/jokkebk/TrinketMIDI).

Upload this to your Trinket, hook up the MIDI signal on pin `PB2`
and you're ready to go!

Detailed building instructions coming soon in the blog.

## Design

The Trinket doesn't have a UART, so it has to be emulated in
software. For this, a special version of `SoftwareSerial` is
used, that makes use of the `INT0` interrupt to avoid conflict
with V-USB.

Parts:

 - `main.cpp`: Main program.
 - `SoftwareSerial.*`: Library for serial RX (MIDI signal)
 - `usbdrv`, `usbconfig.h`, `trinketusb.*`: Driver allowing Trinket to become a low-speed USB device
 - `vusbmidi.*`: USB-MIDI specific descriptors, for the driver to use.

# Licensing

USB initialization and oscillator calibration methods in trinketusb.c are from
Adafruit TrinketKeyboard library and licensed under [LGPL v3](LICENSE_LGPL3)

V-USB MIDI device descriptors and V-USB skeletop functions in vusbmidi.c are from Martin
Homuth-Rosemann's V-USB MIDI project and licenced under [GPL v2](LICENSE_VUSBMIDI)

Code from @jokkebk and me (@mild_sunrise) licenced under [GPL v3](LICENSE) which should be
compatible with both of the above. Enjoy!

EVERYTHING COMES AS-IS, USE AT YOUR OWN RESPONSIBILITY! See source and licence
files for additional disclaimers.
