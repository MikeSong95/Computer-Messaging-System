.data
.equ ADDR_JP1PORT, 		0x10000060
.equ ADDR_JP1PORT_DIR, 	0x10000064
.equ ADDR_JP1PORT_IE, 	0x10000068
.equ ADDR_JP1PORT_EDGE, 0x1000006C

.equ ADDR_JP2PORT, 		0x10000070
.equ ADDR_JP2PORT_DIR, 	0x10000074
.equ ADDR_JP2PORT_IE, 	0x10000078
.equ ADDR_JP2PORT_EDGE, 0x1000007C

.equ ADDR_PS2PORT, 		0x10000100
.equ ADDR_PS2PORT_DIR, 	0x10000104

.equ IRQ_JP2, 		0x00001000
.equ IRQ_PS2, 		0x00000080

.equ ADDR_PUSHBUTTONS, 		0x10000050
.equ ADDR_PUSHBUTTONS_EDGE, 0x1000005C
.equ IRQ_PUSH_BUTTON, 		0X00000002

.equ IRQ_PS2_JP2_PUSH_BUTTON, 0X00001082

.text
.global main

main:
	call eraseTerminal
	
/* Setup JP1 for output */

	movia r8, ADDR_JP1PORT_DIR 
	movia r9, 0xffffffff
	stwio r9, 0(r8)					# Set direction to output

/* Setup JP2 for input */

	movia r2,ADDR_JP2PORT_DIR
	stwio r0,0(r2)  				# Set direction to input
		
/* Enable NIOS 2 interrupts for JP2 and PS2 */

	movia r2,IRQ_PS2_JP2_PUSH_BUTTON
	wrctl ctl3,r2   				

/* Global interrupt */

	movia r8, 	1					# Enable global interrupt
	wrctl ctl0, r8

SETUP_INTERRUPT_PUSHBUTTON:
	movia r8,ADDR_PUSHBUTTONS
  	movia r9,0xe
  	stwio r9,8(r8)  			# Enable interrupts on push buttons 1
	
SETUP_INTERRUPT_PS2:				# Set up interrupt for the PS/2 keyboard
	movia r8, ADDR_PS2PORT_DIR		# Enable interrupt on all bits
	movia r9, 0x1
	stwio r9, 0(r8)
	movia r9, 0x1
	
SETUP_INTERRUPT_JP2:
	movia r2,ADDR_JP2PORT_IE
	movi  r3,0xffffffff
	stwio r3,0(r2)					# Enable interrupts on all pins

POLL_PS2_KEYBOARD:
	br POLL_PS2_KEYBOARD
	
/* ----------- Interrupt Handler ------------ */

.section .exceptions, "ax"
IHANDLER:           
	rdctl et, ctl4				# Check if an external interrupt has occurred        
	beq   et, r0, SKIP_EA_DEC 
	
	movia r8, IRQ_PS2			# Test for bit 7 interrupt - PS/2         
	and   r8, et, r8
	bne r8, r0, PS2_IHANDLER

	movia r8, IRQ_JP2        # Test for interrupt from JP2
	and   r8, et, r8
	bne r8, r0, JP2_IHANDLER
	
	movia r8, IRQ_PUSH_BUTTON	# Test for interrupt from push buttons
	and r8, et, r8
	bne r8, r0, PUSH_BUTTON_IHANDLER
	
	br EXIT_HANDLER

PUSH_BUTTON_IHANDLER:
	movia r2, ADDR_PUSHBUTTONS
	ldwio r3, 0(r2)

	bne r3, r0, EXIT_HANDLER

	call eraseTerminal		# If the pushbutton is pressed, clear the terminal screen
	
	movia r8, ADDR_PUSHBUTTONS_EDGE		# De-assert pushbutton interrupt
	stwio r0, 0(r8)

	br EXIT_HANDLER

PS2_IHANDLER:
	movia r9, ADDR_PS2PORT

	ldbio r10, 0(r9) 				# Either a make code, or F0, or E0
	andi r10, r10, 0x000000ff		# Mask just the data bits of register
	
	movia r12, ADDR_JP1PORT		# Write character to GPIO JP1 
	
	stbio r10, 0(r12)
	
	mov r4, r10
	call printChar			# Print character to the terminal

	br EXIT_HANDLER
	
JP2_IHANDLER:
	movia r9, ADDR_JP2PORT 		# Read port A data 
	ldwio r10,0(r9)  

	andi r4, r10, 0x00ff
	call printMessage			# Prints entire received message when the sender types "Enter" key

	movia r8,ADDR_JP2PORT_EDGE	# De-assert interrupt - write to edgecapture regs
	stwio r0,0(r8) 

EXIT_HANDLER:	
	subi ea, ea, 4	# Replay interrupted instruction for hw interrupts 
	
SKIP_EA_DEC:
	eret