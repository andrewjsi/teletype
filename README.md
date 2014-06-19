# JSS TeleType Connector

This program invoke a new shell and connect the standard output to a serial
port, where is a keyboard-less serial terminal monitor equipment connected, eg. DEC VT320,
VT420, VT510 or Chameleon. With this program, you can put the xterm content to
an external serial terminal.

## Usage

    teletype <serialport> <baud> [wtime]

* _wtime_ - Some serial terminals can not print the received data with native baud.
  When the data congested, the screen is break up. If wtime parameter is set,
  the writting to serial port is slowed down. After each bytes written, wait
  *'wtime'* millisecs before next byte write.

## License

[_GPL2_](https://www.gnu.org/licenses/gpl-2.0.html)

(C) Copyright 2010-2014 Andras Jeszenszky, [JSS & Hayer
IT](http://www.jsshayer.hu) All Rights Reserved.
