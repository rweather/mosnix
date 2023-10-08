FAT32 filesystem
================

MOSnix can support SD cards in FAT32 format if the target implements the
SPI API.

The MOSnix "eater" target is designed for
<a href="https://eater.net/6502">Ben Eater's Breadboard 6502 Computer</a>
and supports SPI on port A of the 6522 VIA.

To connect an SD card to your breadboard computer, you will need an SD
module that is capable of operating at 5V.  Arduino suppliers like
<a href="https://www.adafruit.com/product/254">Adafruit</a> and
<a href="https://www.sparkfun.com/products/13743">Sparkfun</a>
have suitable modules, as do many other suppliers of Arduino gear.

It is assumed that the SD module is connected to the following pins:

* PA0 - SCLK - SPI clock.
* PA1 - MOSI - Master out, slave in.
* PA5 - SDCS - Chip select for the SD card.
* PA6 - MISO - Master in, slave out.

Capabilities
------------

The FAT filesystem driver in MOSnix supports a single FAT32 volume,
with no support for FAT12, FAT16, or exFAT at present.  Restricting
the FAT version simplifies the implementation.  FAT32-capable SD cards
are commonly available so this restriction isn't too onerous.

The driver only supports short 8.3 filenames.  Long filenames are not
handled yet.  Volume labels, hidden files, and other special files
are ignored.

The FAT32 partition must be the first partition on the SD card.  All other
partitions are ignored.  If the SD card does not have a partition table,
then the entire disk is used as the FAT32 partition.

There is no support for formatting a SD card with MOSnix.  You will
need to do that on a separate computer.

Other targets and media types
-----------------------------

To port to other targets, you need to provide an implementation of the
SPI API in "os/drivers/spi/spi.h" and define the following symbols in
the "target.h" file:

* CONFIG\_SD
* CONFIG\_SPI

At the moment, the FAT32 implementation assumes that the media is a
SD card.  Floppy disk drives, USB memory sticks, and other media that
support FAT32 could be used instead with some work.  At the moment
you will need to emulate the SD card API in "os/drivers/sdcard/sdcard.h".
The low-level API may be replaced with someting more generic if there
is a need for it.
