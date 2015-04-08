Computer to Computer Messaging System

By: Michael Song and Hassan Rezvani

Programmed in Assembly and C and implemented on a NIOS II Embedded Processor.
Run in the Altera Monitor Program.

ABOUT:
This project allows text to be inputted, transferred, and displayed between computers. Text is inputted through a PS/2 keyboard, and the message
is displayed on the Altera Monitor Program terminal.

SETUP:
- Two computer are each connected to an Altera DE2 board. 
- The DE2 boards are connected to each other by two GPIO ports (JP1 -> JP1, JP2 -> JP2).
- A PS/2 keyboard is connected to each DE2 board. 

FILES:
- The files "client.s" and "io.c" are intended to be part of one program run on one computer.
- The files "server.s" and "file_io.c" are intended to be part of one program run on the other computer.