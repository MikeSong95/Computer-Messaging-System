#include <stdio.h>   /* Required for file operations */

#define RLEDs ((volatile long *) 0x10000000)
#define GLEDs ((volatile long *) 0x10000010)

#define ADDR_JP2PORT ((volatile char *) 0x10000070)

#define TIMER 250000

// Save next character when F0 break code is received
int saveNextCharacter = 0;
int saveNextChar = 0;

// Received message array
char message [999999]; 
int messageSize = 0;

// To handle edge cases dealing with first messages
int firstTime = 1;

// Message to send array
char messageToSend [999999];
int mts_index = 0;

// Green LEDs flash when KEY3 is pressed
void flashGreenLEDs() {
	*GLEDs = 0xc0;

	int delay = 0;
	double garbage = 0;
	for (; delay < 999999; delay++) {
		*GLEDs = 0xc0;
		*GLEDs = 0xc0;
		*GLEDs = 0x00000;
	}
		
	*GLEDs = 0x00000;
}

// Red LEDs flash when text is received
void flashRedLEDs() {
	*RLEDs = 0xfffff;
	int delay = 0;
	double garbage = 0;
	for (; delay < 999999; delay++) {
		*RLEDs = 0xfffff;
		*RLEDs = 0xfffff;
		*RLEDs = 0x00000;
	}
		
	*RLEDs = 0x00000;
}
// Red LEDs flash down the line
void flashRedLEDLine() {
	int delay = 0;
	unsigned int bitsToWrite = 524288;
	
	while (bitsToWrite != 0) {
		for (; delay < TIMER; delay++) {
			*RLEDs = bitsToWrite;
		}
		
		delay = 0;
		bitsToWrite >>= 1;
	}
	
	*RLEDs = 0x0000;
}

// Prints character to terminal as user inputs it
void printChar(unsigned input){
	char character;
	
	if (input == 0x1c) {
		character = 'a';
	} else if (input == 0x32) {
		character = 'b';
	} else if (input == 0x21) {
		character = 'c';
	} else if (input == 0x23) {
		character = 'd';
	} else if (input == 0x24) {
		character = 'e';
	} else if (input == 0x2B) {
		character = 'f';
	} else if (input == 0x34) {
		character = 'g';
	} else if (input == 0x33) {
		character = 'h';
	} else if (input == 0x43) {
		character = 'i';
	} else if (input == 0x3B) {
		character = 'j';
	} else if (input == 0x42) {
		character = 'k';
	} else if (input == 0x4B) {
		character = 'l';
	} else if (input == 0x3A) {
		character = 'm';
	} else if (input == 0x31) {
		character = 'n';
	} else if (input == 0x44) {
		character = 'o';
	} else if (input == 0x4D) {
		character = 'p';
	} else if (input == 0x15) {
		character = 'q';
	} else if (input == 0x2D) {
		character = 'r';
	} else if (input == 0x1B) {
		character = 's';
	} else if (input == 0x2C) {
		character = 't';
	} else if (input == 0x3C) {
		character = 'u';
	} else if (input == 0x2A) {
		character = 'v';
	} else if (input == 0x1D) {
		character = 'w';
	}  else if (input == 0x22) {
		character = 'x';
	} else if (input == 0x35) {
		character = 'y';
	} else if (input == 0x1A) {
		character = 'z';
	} else if (input == 0x45) {
		character = '0';
	} else if (input == 0x16) {
		character = '1';
	} else if (input == 0x1E) {
		character = '2';
	} else if (input == 0x26) {
		character = '3';
	} else if (input == 0x25) {
		character = '4';
	} else if (input == 0x2E) {
		character = '5';
	} else if (input == 0x36) {
		character = '6';
	} else if (input == 0x3D) {
		character = '7';
	} else if (input == 0x3E) {
		character = '8';
	} else if (input == 0x46) {
		character = '9';
	} else if (input == 0x0E) {
		character = '`';
	} else if (input == 0x4E) {
		character = '-';
	} else if (input == 0x55) {
		character = '=';
	} else if (input == 0x5D) {
		character = '\\';
	} else if (input == 0x29) {
		character = ' ';
	} else if (input == 0x0D) {
		character = '	';
	} else if (input == 0x5A) {
		character = '\n';
	}
	
	// F0 from break code received, want to save next input (will be make code) 	
	if (input == 0xf0) {
		saveNextCharacter = 1;
		return;
	}

	if (saveNextCharacter) {
		printf("%c", character);
		
		// Create an array of the characters the user typed
		// Represents the message to send
		messageToSend[mts_index] = character;
		
		mts_index++;
		
		// User typed "Enter" key
		// Make a new line, new prompt (>>:) and reset the message array
		if (character == '\n') {
			flashRedLEDLine();
			
			printf(">>: ");
			mts_index = 0;
			memset(&messageToSend[0], 0, sizeof(messageToSend));
		} 
		
		saveNextCharacter = 0;
	}
}

// Creates an array of the message received by the user and prints it to the terminal
// when the sender has hit the "Enter" key
void printMessage (unsigned input) {
	// If the sender has cleared their terminal before hitting "Enter" key, 
	// reset the received message array
	if (input == 0xfa) {
		messageSize = 0;
		memset(&message[0], 0, sizeof(message));
	} else {
		char character;
		
		if (input == 0x1c) {
			character = 'a';
		} else if (input == 0x32) {
			character = 'b';
		} else if (input == 0x21) {
			character = 'c';
		} else if (input == 0x23) {
			character = 'd';
		} else if (input == 0x24) {
			character = 'e';
		} else if (input == 0x2B) {
			character = 'f';
		} else if (input == 0x34) {
			character = 'g';
		} else if (input == 0x33) {
			character = 'h';
		} else if (input == 0x43) {
			character = 'i';
		} else if (input == 0x3B) {
			character = 'j';
		} else if (input == 0x42) {
			character = 'k';
		} else if (input == 0x4B) {
			character = 'l';
		} else if (input == 0x3A) {
			character = 'm';
		} else if (input == 0x31) {
			character = 'n';
		} else if (input == 0x44) {
			character = 'o';
		} else if (input == 0x4D) {
			character = 'p';
		} else if (input == 0x15) {
			character = 'q';
		} else if (input == 0x2D) {
			character = 'r';
		} else if (input == 0x1B) {
			character = 's';
		} else if (input == 0x2C) {
			character = 't';
		} else if (input == 0x3C) {
			character = 'u';
		} else if (input == 0x2A) {
			character = 'v';
		} else if (input == 0x1D) {
			character = 'w';
		}  else if (input == 0x22) {
			character = 'x';
		} else if (input == 0x35) {
			character = 'y';
		} else if (input == 0x1A) {
			character = 'z';
		} else if (input == 0x45) {
			character = '0';
		} else if (input == 0x16) {
			character = '1';
		} else if (input == 0x1E) {
			character = '2';
		} else if (input == 0x26) {
			character = '3';
		} else if (input == 0x25) {
			character = '4';
		} else if (input == 0x2E) {
			character = '5';
		} else if (input == 0x36) {
			character = '6';
		} else if (input == 0x3D) {
			character = '7';
		} else if (input == 0x3E) {
			character = '8';
		} else if (input == 0x46) {
			character = '9';
		} else if (input == 0x0E) {
			character = '`';
		} else if (input == 0x4E) {
			character = '-';
		} else if (input == 0x55) {
			character = '=';
		} else if (input == 0x5D) {
			character = '\\';
		} else if (input == 0x29) {
			character = ' ';
		} else if (input == 0x0D) {
			character = '	';
		} else if (input == 0x5A) {
			character = '\n';
		}
		
		// F0 from break code received, want to save next input (will be make code) 
		if (input == 0xf0) {
			saveNextChar = 1;
			return;
		}
		
		// If the user has not hit "Enter" key
		// save the character in the received message array
		if (saveNextChar && input != 0x5A) {
			message[messageSize] = character;
			saveNextChar = 0;
			messageSize++;
		}
		
		// If the user has it "Enter" key
		if (input == 0x5A && saveNextChar == 0) {
			int i = 0;
			
			flashRedLEDs(); 
			
			// If message received while nothing has been typed
			// clear >: from line
			if (mts_index == 0) {
				// Clear terminal line
				printf("%c[2K", 27);
				printf("%c[%dD", 27, 4);
				printf("<<: ");
			
				// Print out received message
				for (; i < messageSize; i++) {
					printf("%c", message[i]);
				}
				
				if (firstTime) {
					firstTime = 0;
				}
			
				printf("\n>>: ");
				
			} else {	// There's input in the terminal that has not been sent
				// Clear terminal line
				printf("%c[2K", 27);
				printf("%c[%dD", 27, mts_index + 4);
				printf("<<: ");
			
				// Print out received message
				for (; i < messageSize; i++) {
					printf("%c", message[i]);
				}
				
				int index = 0;
			
				printf("\n>>: ");
			
				// Print out terminal input that had not been sent
				for (; index < mts_index; index++) {
					printf("%c", messageToSend[index]);
				}
				
				if (firstTime) {
					firstTime = 0;
				}
			} 
			
			// Reset received message array
			messageSize = 0;
			memset(&message[0], 0, sizeof(message));
			saveNextChar = 0;
		}
	} 
}

// Erase everything on the terminal when KEY3 is pressed
void eraseTerminal() {
	printf("%c[2J", 27);
	flashGreenLEDs();
	printf(">>: ");	

	*ADDR_JP2PORT = 0xfa;
}
