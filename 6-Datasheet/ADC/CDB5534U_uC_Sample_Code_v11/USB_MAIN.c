//-----------------------------------------------------------------------------
// USB_MAIN.c    (copper CDB5534 BOARD, ENGR FUNCTIONS ALSO)
//-----------------------------------------------------------------------------
//The API memory model is SMALL.  Put the USB_API.LIB into 
//"Project, Target Build Configuration, Customize, Files to Link".
//In "Project, Tool Chain Integration, Compiler, Customize, select "Favor Small 
//Code". Use "Small data, Large 64K functions".  In linker using "no overlay".
// In compiler, using "optimize" zero.

//#pragma SRC	//will give you assembly source instead of an obj file.
#include <c8051f320.h>        
#include <stddef.h>       
#include "USB_API.h"

// Global CONSTANTS
sbit DUT_CSB = P0^0; 
sbit Led20 = P2^0; 	
sbit RDYB = P1^2;
code const BYTE MaxPower = 20;       // Max current = 40 MA (measured 36 MA)
code const BYTE PwAttributes = 0x80; // Bus-pwr, remote wakeup not supported
code const UINT bcdDevice = 0x100;   // Device release number 1.00

//function prototypes
void config(void);
void Suspend_Device(void);
void write_to_register(char command, char low, char mid, char high, char top);
void read_register(char command);
void DLA(short);
void read_var(char pac_z, char pac_1);

//global VARIABLES
BYTE Out_Packet[6] = {0,0,0,0,0,0};   // 6-byte packet from PC
idata BYTE In_Packet[64];   //idata is 256 bytes (including the 128 "data" bytes)
BYTE xdata dumm; 		//xdata is 2048 bytes. Stay below 0x3FF because USB uses above 0x3FF	           
char data WRFL;  
char data RDFL; 
char data RSFL;
char data UDFL;
char data FDFL;
char data FEFL;
char data SCFL;
char data CCFL;
char data CLFL;
char data k; 	   
char data TXFL;
short idata t2val;
char idata m;
char idata R_W;
char idata dcnt;


void main(void) 
{
char data pac_z;    //packet first byte value
char data pac_1;   // packet second byte value
char data pac_4;	//samples index
char data j;
short idata samples;
short idata n;	
	
	PCA0MD &= ~0x40;    // Disable Watchdog timer

//Cirrus Logic VendorID is 0x0429.
//void USB_Init(int VendorID, int ProductID, uchar *ManufacturerStr, uchar *ProductStr,
//			uchar *SerialNumberStr, byte MaxPower, byte PwAttributes, uint bcdDevice);
//Below, we pass "NULL" TO VendorID and ProductID because if you use our ID numbers, 
//the operating system will look in its list of drivers for Vendor 0x0429 and will not
//find the driver. 
//By passing "NULL" to VendorID and ProductID, the operating system asks for a path to the
//driver, and we must supply the (Silicon Labs) driver to the customer.
	USB_Init(0,0,NULL,NULL,NULL,MaxPower,PwAttributes, bcdDevice);   
	config();			//Ext Int0 as low priority, low level, not enabled, etc, etc
	CLKSEL |= 0x02;		//Change system clock to 24 MHz
	Led20 = 0;			
	pac_z = 0;	
	pac_1 = 0;
	WRFL = 0;  
	RDFL = 0; 
	RSFL = 0;
	UDFL = 0;
	FDFL = 0;
	FEFL = 0;
	SCFL = 0;
	CCFL = 0;
	CLFL = 0;
	t2val = 0;
	TXFL  = 0;
	USB_Int_Enable();	//API Library function    
	k = 0; 
	m = 0;
	while(1)
		{
		if(RSFL)
			{
			write_to_register(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
			write_to_register(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
			write_to_register(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
			write_to_register(0xFF, 0xFF, 0xFF, 0xFF, 0xFE);
			Led20 = 1;		
			RSFL = 0;
			}
		
		if(WRFL)   
			{
			write_to_register(Out_Packet[0], Out_Packet[1], Out_Packet[2],
			 											Out_Packet[3],Out_Packet[4]);
			WRFL = 0;
			}
		
		if(RDFL)   
			{
			read_register(Out_Packet[0]);         
			Block_Write(In_Packet, 4); 
			RDFL = 0;
			}		
		
		if(UDFL)	//read all offset or gain or chan setup registers
			{
			pac_z = Out_Packet[0];	//command 0x49, 0x4A, 0x4D
			pac_1 = Out_Packet[1];	//number of channels of DUT
			read_var(pac_z, pac_1);
			Block_Write(In_Packet, 4 * pac_1); 	
			UDFL = 0;
			}			

		if(SCFL)   //single convert mode (may require 0.6 seconds)
			{
			pac_z = Out_Packet[0];
			DUT_CSB = 0;				//lower Chip Select
			while(TXBMT == 0);	//check Tx buffer empty
			SPIF = 0;			//clear "shift finished" flag
			SPI0DAT = pac_z;  //shift out 80,88,90,98,A0,A8,B0, or B8 command
			while(!SPIF); 		//Wait for SPI shift complete
			while(RDYB);		//wait for SDO to drop (P1.2)                 
			SPIF = 0;			
			SPI0DAT = 0x00;  	//shift out zero to clear "SDO FLAG"
			while(!SPIF); 		
			SPIF = 0;
			SPI0DAT = 0xFE;     
			while(!SPIF); 			
			In_Packet[3] = SPI0DAT;	//Read SPI input buffer MSB
			SPIF = 0;
			SPI0DAT = 0xFE;     
			while(!SPIF); 	
			In_Packet[2] = SPI0DAT;	//Read SPI input buffer
			SPIF = 0;
			SPI0DAT = 0xFE;     
			while(!SPIF);	
			In_Packet[1] = SPI0DAT;	//Read SPI input buffer				 	                              
			SPIF = 0;
			SPI0DAT = 0xFE;     
			while(!SPIF);  			
			In_Packet[0] = SPI0DAT;	//Read SPI input buffer	LSB	 
			dumm = 1;
			dumm = 0;
			DUT_CSB = 1;				//raise Chip Select
			dumm = 1;
			dumm = 0;	
			Block_Write(In_Packet, 4); 
			SCFL = 0;
			}
		
		if(CCFL)   //continuous convert mode 
			{
			pac_1 = Out_Packet[1];		// command
			pac_4 = Out_Packet[4];		// samples_index
    	 	samples = 0x10 << pac_4;		
			
			while(TXBMT == 0);			// check Tx buffer empty
			DUT_CSB = 0;				// lower Chip Select			

			SPIF = 0;			
			SPI0DAT = pac_1;  			// command
			while(!SPIF); 				// Wait for SPI shift complete

			for(j = 0; j < 5; j++)		// Throw out 5 conversions - to flush residual filter coeffs (datasheet pg 36)
				{
				while(RDYB); 			// wait for SDO to drop (P1.2)
				                 
				SPIF = 0;			
				SPI0DAT = 0x00;  		// shift out zeros to release SDO
				while(!SPIF);
				 		
				SPIF = 0;
				SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
				while(!SPIF);
				pac_z = SPI0DAT;		// Read SPI input buffer MSB
				
				SPIF = 0;
				SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
				while(!SPIF);
				pac_z = SPI0DAT;		// Read SPI input buffer
				
				SPIF = 0;
				SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
				while(!SPIF);
				pac_z = SPI0DAT;		// Read SPI input buffer				 	                              
				
				SPIF = 0;
				SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
				while(!SPIF);
				pac_z = SPI0DAT;		//Read SPI input buffer	LSB
				
			// ========================
			// VKEROF: Jan 19 2005
				dumm = 1;
				dumm = 0;
				DUT_CSB = 1;				// raise Chip Select
				dumm = 1;
				dumm = 0;
				while(TXBMT == 0);			// check Tx buffer empty
				DUT_CSB = 0;				// lower Chip Select	
			// ========================

				}

			for(n = 0; n < samples; n += 15)
				{
				for(j = 0; j < 60; j += 4)
					{
					while(RDYB); 				//wait for SDO to drop (P1.2) 

					SPIF = 0;			
					SPI0DAT = 0x00;  			//shift out zeros to clear SDO (so converter remains in conversion mode)
					while(!SPIF); 		
					
					SPIF = 0;
					SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
					while(!SPIF); 			
					In_Packet[j + 3] = SPI0DAT;	//Read SPI input buffer MSB
					
					SPIF = 0;
					SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
					while(!SPIF); 	
					In_Packet[j + 2] = SPI0DAT;	//Read SPI input buffer
					
					SPIF = 0;
					SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
					while(!SPIF);	
					In_Packet[j + 1] = SPI0DAT;	//Read SPI input buffer				 	                              
					
					SPIF = 0;
					SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
					while(!SPIF);  			
					In_Packet[j] = SPI0DAT;		//Read SPI input buffer	LSB	
					
			// ========================
			// VKEROF: Jan 19 2005
				dumm = 1;
				dumm = 0;
				DUT_CSB = 1;				// raise Chip Select
				dumm = 1;
				dumm = 0;
				while(TXBMT == 0);			// check Tx buffer empty
				DUT_CSB = 0;				// lower Chip Select	
			// ========================

					}
					
				Block_Write(In_Packet, 60);		//15 samples per packet

				}
				
			while(RDYB); //wait for SDO to drop (P1.2)                 

			SPIF = 0;			
			SPI0DAT = 0xFF;  	//shift out ones to stop continuous convert
			while(!SPIF); 		

			SPIF = 0;
			SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
			while(!SPIF); 			
			pac_z = SPI0DAT;	//Read SPI input buffer MSB

			SPIF = 0;
			SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
			while(!SPIF); 	
			pac_z = SPI0DAT;	//Read SPI input buffer

			SPIF = 0;
			SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
			while(!SPIF);	
			pac_z = SPI0DAT;	//Read SPI input buffer				 	                              

			SPIF = 0;
			SPI0DAT = 0x00;     //vk:change 0xFE to 0x00
			while(!SPIF);  			
			pac_z = SPI0DAT;	//Read SPI input buffer	LSB	 

			dumm = 1;
			dumm = 0;
			DUT_CSB = 1;				//raise Chip Select
			dumm = 1;
			dumm = 0;
				
			CCFL = 0;
			}		
		
		
		if(FDFL)  				//I2C read 9 bytes from first of EPROM 
			{
			TR2 = 1;			//ENABLE TIMER2 (SCK ABOUT 2.5 KHz)        
			SMB0CF |= 0x80;		//enable the I2C peripheral
			EIE1 = EIE1 | 0x01;	//ENABLE I2C interrupt
			//zero the memory pointer in the EPROM
			dcnt = 2;			
			R_W = 0;			
			Out_Packet[1] = 0;	
			m = 1;				
			STA = 1;					
			while(m < 3);
			//read 9 bytes I2C from first of EPROM
			dcnt = 9;			
			R_W = 1;			
			m = 0;				
			STA = 1;	//this will cause I2C interrupt	when "START" is done				
			while(m < 10);	//give main something to do between handler runs	
			Block_Write(In_Packet, 9);  //send data from EPROM to PC	
			DLA(10); //100 milliseconds //allow time for stopbit before turning off timer2
			FDFL = 0;
			}
		
		if(FEFL)  				//Write only one mem byte of EPROM -- first byte is zero
			{
			TR2 = 1;			//ENABLE TIMER2 (SCK ABOUT 2.5 KHz)        
			SMB0CF |= 0x80;		//enable the I2C peripheral
			EIE1 = EIE1 | 0x01;	//ENABLE I2C interrupt
			dcnt = 3;			//count of I2C address byte, pointer byte, data byte
			R_W = 0;
			m = 1;
			STA = 1;			//this will cause I2C interrupt	when "START" is done				
			while(m < 4); 		//give main something to do between handler runs
			DLA(80); 			//800 milliseconds burn EPROM time
			FEFL = 0;
			}		
		
		if(CLFL)
			{
			pac_z = Out_Packet[0];
			DUT_CSB = 0;		//lower Chip Select
			while(TXBMT == 0);	
			SPIF = 0;			
			SPI0DAT = pac_z;  	//shift out cal command
			while(!SPIF); 		//Wait for SPI shift complete
			while(RDYB);		//wait for SDO to drop (P1.2)      	
			dumm = 1;
			dumm = 0;
			DUT_CSB = 1;		//raise Chip Select
			dumm = 1;
			dumm = 0;	
			CLFL = 0;
			}
		}
}


void I2C_HAN(void) interrupt 7 //  handler at 0x003B
{
	switch (SMB0CN & 0xF0) 	
		{
		case 0xE0:					//"start bit done" interrupt
		SMB0DAT = 0xA0 | R_W;		//I2C address + read/write bit
		STA = 0;					//clear the start bit
		SI = 0;						//clear the interrupt (shift I2C address)
		break;
		case 0xC0:			//(TX mode) "byte transferred" interrupt
		if(ACK)
			{
			if(m < dcnt)  
				{
				if(R_W == 0) SMB0DAT = Out_Packet[m];  
				}
			else 	
				{
				STO = 1;	
				}
			}
		SI = 0;
		m++;
		break;
		case 0x80:				//(RX mode) "byte transferred" interrupt 
		In_Packet[m - 1] = SMB0DAT;
		m++;
		if(m > 9)	
			{
			ACK = 0;
			STO = 1;
			}
		else ACK = 1;
		SI = 0;
		break;
		default:
		Led20 = 1;		
		SI = 0;
		break;
		}
}


//Timer2 counting sysclk/12 = 2 MHz. Sysclk corrected by USB traffic.
//Timer 2 will overflow every ten milliseconds causing interrupt.  
void DLA(short del)  //passed value "del" x 10 milliseconds is delay time
{

	ET2 = 1;		//ENABLE TIMER2 INTERRUPT 
	TR2 = 1;		//ENABLE TIMER2         
	while(t2val < del);  //wait for "del" overflows 
	ET2 = 0;		//DISABLE TIMER2 INTERRUPTS
	t2val = 0;		//global incremented by interrupt handler   	
}

// Timer2 overflow interrupt handler 
void TIM_TWO_HAN(void) interrupt 5	//timer2 overflow vector is 0x2B 
{									
	t2val++;   //increments a global short integer     
	TF2H = 0;  //you must clear the interrupt flag

}



void USB_HAN(void) interrupt 16  //USB INTERRUPT HANDLER
{
	BYTE INTVAL = Get_Interrupt_Source();  //clears ALL USB pending flags
	if (INTVAL & TX_COMPLETE)	
		{
		TXFL = 1;
		}
	if (INTVAL & RX_COMPLETE) //Micro has received an entire "Out_Packet" from PC	
		{                  
		Block_Read(Out_Packet, 6);	//Max Block_Read is 64 bytes, PC sends 6-byte packets
		switch(Out_Packet[0])
			{
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x05:
			case 0x07:
			case 0x11:
			case 0x12:
			case 0x15:
			case 0x21:
			case 0x22:
			case 0x25:
			case 0x31:
			case 0x32:
			case 0x35:
			WRFL = 1;
			break;
			
			case 0x08:
			case 0x09:	
			case 0x0A:
			case 0x0B:
			case 0x0C:
			case 0x0D:
			case 0x0F:
			case 0x19:
			case 0x1A:
			case 0x1D:
			case 0x29:
			case 0x2A:
			case 0x2D:
			case 0x39:
			case 0x3A:
			case 0x3D:
			RDFL = 1; 
			break;

			case 0xFF:
			RSFL = 1;
			break;
			
			case 0x49:
			case 0x4A:
			case 0x4D:
			UDFL = 1;
			break;

			case 0x80:  //single convert mode
			case 0x88:
			case 0x90:
			case 0x98:
			case 0xA0:
			case 0xA8:
			case 0xB0:
			case 0xB8:
			SCFL = 1;
			break;
			
			case 0xCC:
			CCFL = 1;
			break;
			
			case 0x81:  //CALIBRATIONS 
			case 0x82:					  
			case 0x85:					  
			case 0x86:					  
			case 0x89:					  
			case 0x8A:					  
			case 0x8D:					  
			case 0x8E:					  
			case 0x91:					  
			case 0x92:					  
			case 0x95:					  
			case 0x96:					  
			case 0x99:					  
			case 0x9A:					  
			case 0x9D:					  
			case 0x9E:					  
			case 0xA1:					  
			case 0xA2:					  
			case 0xA5:					  
			case 0xA6:					  
			case 0xA9:					  
			case 0xAA:					  
			case 0xAD:					 
			case 0xAE:					 
			case 0xB1:					 
			case 0xB2:					 
			case 0xB5:					 
			case 0xB6:					 
			case 0xB9:					 
			case 0xBA:					
			case 0xBD:					
			case 0xBE:  
			CLFL = 1;
			break;
			
			case 0xFD:	//Read EPROM I2C packet
			FDFL = 1;
			break;
		
			case 0xFE:	//burn EPROM I2C packet
			FEFL = 1;
			break;
			
			default:
			break;
			}
		}
	if (INTVAL & DEV_SUSPEND)
		{
		Suspend_Device();
		}
	if (INTVAL & DEV_CONFIGURED)
		{
		config();
		}
}




// this is a 32 bit "read variable number of integers"
void read_var(char command, char chans)
{
int j;
	
	while(TXBMT == 0);  	//wait for SPI Tx buffer empty
	DUT_CSB = 0;
	SPIF = 0;
	SPI0DAT = command;  	//write SPI transmit buffer 
	while(!SPIF); 			//Wait for SPI shift complete
	for(j = 0; j < chans; j++)
		{
		SPIF = 0;
		SPI0DAT = 0xFE;     //write "NOP" to SPI transmit buffer   
		while(!SPIF); 		//Wait for SPI shift complete 			
		In_Packet[4 * j + 3] = SPI0DAT;	//Read SPI input buffer
		SPIF = 0;
		SPI0DAT = 0xFE;     //write "NOP" to SPI transmit buffer   
		while(!SPIF); 		//Wait for SPI shift complete 			
		In_Packet[4 * j + 2] = SPI0DAT;	//Read SPI input buffer
		SPIF = 0;
		SPI0DAT = 0xFE;     //write "NOP" to SPI transmit buffer   
		while(!SPIF); 		//Wait for SPI shift complete 			
		In_Packet[4 * j + 1] = SPI0DAT;	//Read SPI input buffer				 	                              
		SPIF = 0;
		SPI0DAT = 0xFE;     //write "NOP" to SPI transmit buffer   
		while(!SPIF); 		//Wait for SPI shift complete 			
		In_Packet[4 * j] = SPI0DAT;	//Read SPI input buffer		                                
		}
	dumm = 1;
	dumm = 0;
	DUT_CSB = 1; 
	dumm = 1;
	dumm = 0;
	}


// this is a 32 bit "read DUT register"
void read_register(char command)
	{
	while(TXBMT == 0);  	//wait for SPI Tx buffer empty
	DUT_CSB = 0;
	
	SPIF = 0;
	SPI0DAT = command;  	//write SPI transmit buffer 
	while(!SPIF); 			//Wait for SPI shift complete
	
	SPIF = 0;
	SPI0DAT = 0xFE;     	//write "NOP" to SPI transmit buffer   
	while(!SPIF); 			//Wait for SPI shift complete 			
	In_Packet[3] = SPI0DAT;	//Read SPI input buffer
	
	SPIF = 0;
	SPI0DAT = 0xFE;     	//write "NOP" to SPI transmit buffer   
	while(!SPIF); 			//Wait for SPI shift complete 			
	In_Packet[2] = SPI0DAT;	//Read SPI input buffer
	
	SPIF = 0;
	SPI0DAT = 0xFE;     	//write "NOP" to SPI transmit buffer   
	while(!SPIF); 			//Wait for SPI shift complete 			
	In_Packet[1] = SPI0DAT;	//Read SPI input buffer				 	                              
	
	SPIF = 0;
	SPI0DAT = 0xFE;    		//write "NOP" to SPI transmit buffer   
	while(!SPIF); 			//Wait for SPI shift complete 			
	In_Packet[0] = SPI0DAT;	//Read SPI input buffer		                                
	
	dumm = 1;
	dumm = 0;
	DUT_CSB = 1; 
	dumm = 1;
	dumm = 0;
	}


// this is a 32-bit "write DUT register"
void write_to_register(char command,char low, char mid, char high, char top) 
	{
	while(TXBMT == 0);	//wait for Tx buffer empty
	DUT_CSB = 0;
	SPIF = 0;
	SPI0DAT = command;  //write SPI transmit buffer 
	while(!SPIF); //Wait for SPI shift complete
	SPIF = 0;
	SPI0DAT = top;	
	while(!SPIF); 
	SPIF = 0;
	SPI0DAT = high;	
	while(!SPIF); 
	SPIF = 0;
	SPI0DAT = mid;
	while(!SPIF); 
	SPIF = 0;
	SPI0DAT = low;
	while(!SPIF); 
	dumm = 1;
	dumm = 0;
	DUT_CSB = 1; 
	dumm = 1;
	dumm = 0;
	}		
  


// Suspend_Device is called when a DEV_SUSPEND interrupt is received.
void Suspend_Device(void)
{
// Disable peripherals before calling USB_Suspend()
   SPIEN = 0;							// DISABLE SPI0 PERIPHERAL
   TR2 = 0;								// DISABLE TIMER 2
   SMB0CF &= 0x7F;						// DISABLE I2C PERIPHERAL
   REN0 = 0;							// DISABLE UART0 RX
   P0MDIN = 0x00;                       // Port 0 configured as analog input
   P1MDIN = 0x00;                       // Port 1 configured as analog input
   P2MDIN = 0x00;                       // Port 2 configured as analog input
   P3MDIN = 0x00;                       // Port 3 configured as analog input
   USB_Suspend();                       // Put the device in suspend state
// Once execution returns from USB_Suspend(), device leaves suspend state.
// Reenable peripherals
   P0MDIN = 0xFF;
   P1MDIN = 0xFF;                       
   P2MDIN = 0xFF;
   P3MDIN = 0x01;
   SPIEN = 1;							// ENABLE SPI0
	TR2 = 1;							//ENABLE TIMER2 (SCK ABOUT 2.5 KHz)        
	SMB0CF |= 0x80;						//enable the I2C peripheral	  
   REN0 = 1;							// ENABLE UART0 RX
}
