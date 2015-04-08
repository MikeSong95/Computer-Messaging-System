.data
.equ ADDR_JP1PORT, 		0x10000060
.equ ADDR_JP1PORT_DIR, 	0x10000064
.equ ADDR_JP1PORT_IE, 	0x10000068
.equ ADDR_JP1PORT_EDGE, 0x1000006C
.equ IRQ_JP1PA, 		0x00000800

.equ ADDR_JP2PORT, 		0x10000070
.equ ADDR_JP2PORT_DIR, 	0x10000074
.equ ADDR_JP2PORT_IE, 	0x10000078
.equ ADDR_JP2PORT_EDGE, 0x1000007C
.equ IRQ_JP2PA, 		0X00001000

.equ ADDR_PS2PORT, 		0x10000100
.equ ADDR_PS2PORT_DIR, 	0x10000104
.equ IRQ_PS2, 			0x00000080

.equ ADDR_PUSHBUTTONS, 		0x10000050
.equ ADDR_PUSHBUTTONS_EDGE, 0x1000005C
.equ IRQ_PUSH_BUTTON, 		0X00000002

.equ IRQ_PS2_JP1_PUSH_BUTTON, 	0X00000882
.equ ADDR_GREENLEDS, 			0x10000010

.text
.global main

main:
	/* Configure JP1 as input */	
	movia r8, ADDR_JP1PORT_DIR 						# Set up JP1 for input 
	stwio r0, 0(r8)

	/* Configure JP2 as output */ 
	movia r8, ADDR_JP2PORT_DIR 						# Set up JP2 for output 
	movia r9, 0xffffffff
	stwio r9, 0(r8)

SETUP_INTERRUPT_PUSH_BUTTON:
	movia r8,ADDR_PUSHBUTTONS
  	movia r9,0xe
  	stwio r9,8(r8)  			# Enable interrupts on push buttons 1

SETUP_INTERRUPT_JP1:			# Set up the interrupt for the GPIO JP1
	
	movia r8, ADDR_JP1PORT_IE	# Enable interrupt on first 8 bits
	movia r9, 0xffffffff 			
	stwio r9, 0(r8)

SETUP_PS2_INTERRUP:
	movia r8, ADDR_PS2PORT_DIR		# Enable interrupt on all bits
	movia r9, 0x1
	stwio r9, 0(r8)
	movia r9, 0x1

	movia r8, IRQ_PS2_JP1_PUSH_BUTTON
	wrctl ctl3, r8

	movia r8, 1
	wrctl ctl0 , r8				# Enable global interrupt

	call eraseTerminal			# call a fucntion to erase the terminal

POLL:	
	br POLL

# Interrput Handler
.section .exceptions, "ax"
IHANDLER:           
 
	rdctl et, ctl4				# Check if an external interrupt has occurred        
	beq et, r0, SKIP_EA_DEC 

	movia r8,IRQ_JP1PA			# Test for bit 11 interrupt         
	and   r8,et,r8
	bne   r8,r0,JP1_IHANDLER 	# If not, exit handler

	movia r8, IRQ_PS2
	and   r8,et,r8
	bne   r8,r0,PS2_IHANDLER

	movia r8, IRQ_PUSH_BUTTON
	and r8, et, r8
	bne r8, r0, PUSH_BUTTON_IHANDLER

	br EXIT_IHANDLER

JP1_IHANDLER:
	movia r9, ADDR_JP1PORT 		# Read port A data 
	ldwio r10,0(r9)  

	andi r4, r10, 0x00ff

	call printMessage

	movia r8,ADDR_JP1PORT_EDGE	# De-assert interrupt - write to edgecapture regs
	stwio r0,0(r8)

	br EXIT_IHANDLER 

PS2_IHANDLER:
	movia r8, ADDR_PS2PORT
	ldbio r9, 0(r8) 				# Either a make code, or F0, or E0
	andi r9, r9, 0x000000ff			# Mask just the data bits of register

	movia r10, 	ADDR_JP2PORT		# Write character to GPIO JP1 
	stbio r9, 	0(r10)
	mov r4, r9
	call printChar

	br EXIT_IHANDLER

PUSH_BUTTON_IHANDLER:
	movia r2, ADDR_PUSHBUTTONS
	ldwio r3, 0(r2)

	bne r3, r0, EXIT_IHANDLER

	call eraseTerminal

	movia r8, ADDR_PUSHBUTTONS_EDGE
	stwio r0, 0(r8)

	br EXIT_IHANDLER


EXIT_IHANDLER:
	subi ea,ea,4    	# Replay interrupted instruction for hw interrupts 
	
SKIP_EA_DEC:
	eret