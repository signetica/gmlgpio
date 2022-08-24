# gmlgpio

Gemini Lake GPIO device driver for FreeBSD

# Description

The gmlgpio kernel module provides GPIO driver services for Intel's
Gemini Lake Soc.

# Installation

This driver can be installed directly via:

	make; make install

You may also install it as a port by unpacking gmlgpio.shar, changing into the
resulting *gmlgpio* directory, and building it in the usual fashion.

Finally, add 'gmlgpio_load="YES"' to /boot/loader.conf, and load the driver via
'kldload gmlgpio' (or a reboot).
