// NOTE: The crossbar register should be configured before any  
// of the digital peripherals are enabled. 
//This pinout is for the printed CDB554_U board
#include <C8051F320.h>	

void config (void) 
	{
	XBR0 = 0x07;	// Crossbar Register 1   
	XBR1 = 0x78;	// Crossbar Register 2   
                      
    P0MDOUT = 0x19; // Output configuration for P0    
    P1MDOUT = 0x1B; // Output configuration for P1    
    P2MDOUT = 0x01; // Output configuration for P2    
    P3MDOUT = 0x00; // Output configuration for P3    

    P0MDIN = 0xFF;  // Input configuration for P0
    P1MDIN = 0xFF;  // Input configuration for P1
    P2MDIN = 0xFF;  // Input configuration for P2
    P3MDIN = 0xFF;  // Input configuration for P3

    P0SKIP = 0xCF;  //  Port 0 Crossbar Skip Register	
    P1SKIP = 0x81;  //  Port 1 Crossbar Skip Register   
    P2SKIP = 0x07;  //  Port 2 Crossbar Skip Register	
	
    CPT0MX = 0x00;   // Comparator 0 MUX Selection Register
    CPT0MD = 0x02;   // Comparator 0 Mode Selection Register
    CPT0CN = 0x00;   // Comparator 0 Control Register

    CPT1MX = 0x00;   // Comparator 1 MUX Selection Register
    CPT1MD = 0x02;   // Comparator 1 Mode Selection Register
    CPT1CN = 0x00;   // Comparator 1 Control Register
	
	OSCXCN = 0x00;	// EXTERNAL Oscillator Control Register	
//    CLKSEL = 0x00;  // Oscillator Clock Select Register
//	OSCICN = 0x83;	// Internal Oscillator Control Register
	
	SPI0CFG = 0x40;	// SPI Configuration Register
	SPI0CKR = 0x06;	// SPI Clock Rate Register
    SPI0CN = 0x09;	// SPI Control Register

	REF0CN = 0x00;	// Reference Control Register

    AMX0P = 0x00;	// AMX0 Positive Select Register
	AMX0N = 0x00;	// AMX0 Negative Select Register
	ADC0CF = 0xF8;	// ADC Configuration Register
	ADC0CN = 0x00;	// ADC Control Register
	
    ADC0H = 0x00;   // ADC Data MSB
    ADC0L = 0x00;   // ADC Data LSB
	ADC0LTH = 0x00;	// ADC Less-Than High Byte Register
	ADC0LTL = 0x00;	// ADC Less-Than Low Byte Register
	ADC0GTH = 0xFF;	// ADC Greater-Than High Byte Register
	ADC0GTL = 0xFF;	// ADC Greater-Than Low Byte Register

    SCON0 = 0x00;       // Serial Port Control Register

    PCON = 0x00;        // Power Control Register

    SMB0CF = 0x43;  // Master only,  clock timer2, not enabled            
    SMB0DAT = 0x00; // SMBus Data Register
    SMB0CN = 0x00;  // SMBus Control Register

    PCA0MD = 0x00;      // PCA Mode Register			 //was 0x06
    PCA0L = 0x00;       // PCA Counter/Timer Low Byte
    PCA0H = 0x00;       // PCA Counter/Timer High Byte	    
    PCA0CN = 0x00;      // PCA Control Register			 //was 0x40
	

    //Module 0
    PCA0CPM0 = 0x00;    // PCA Capture/Compare Register 0
    PCA0CPL0 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH0 = 0x00;    // PCA Counter/Timer High Byte

    //Module 1
    PCA0CPM1 = 0x00;    // PCA Capture/Compare Register 1
    PCA0CPL1 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH1 = 0x00;    // PCA Counter/Timer High Byte

    //Module 2
    PCA0CPM2 = 0x00;    // PCA Capture/Compare Register 2
    PCA0CPL2 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH2 = 0x00;    // PCA Counter/Timer High Byte

    //Module 3
    PCA0CPM3 = 0x00;    // PCA Capture/Compare Register 3
    PCA0CPL3 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH3 = 0x00;    // PCA Counter/Timer High Byte

    //Module 4
    PCA0CPM4 = 0x00;    // PCA Capture/Compare Register 4
    PCA0CPL4 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH4 = 0x00;    // PCA Counter/Timer High Byte

// Timers 0 SET UP FOR I2C.  TIMER 1 SET UP FOR COUNTER 1           
    CKCON = 0x04;   // Clock Control Register    
    TL0 = 0xFF;     // Timer 0 Low Byte
    TL1 = 0x00;     // Timer 1 Low Byte
    TH0 = 0xE8;     // Timer 0 High Byte
    TH1 = 0x00;     // Timer 1 High Byte    
    TMOD = 0x52;    // Timer Mode Register
    TCON = 0x50;    // Timer Control Register 
		
    TMR2RLL = 0xE0; // Timer 2 Reload Register Low Byte
    TMR2RLH = 0xB1; // Timer 2 Reload Register High Byte
    TMR2L = 0xE0;   // Timer 2 Low Byte
    TMR2H = 0xB1;   // Timer 2 High Byte    
    TMR2CN = 0x00;  // Timer 2 Control Register

    TMR3RLL = 0x00; // Timer 3 Reload Register Low Byte
    TMR3RLH = 0x00; // Timer 3 Reload Register High Byte
    TMR3L = 0x00;   // Timer 3 Low Byte
    TMR3H = 0x00;   // Timer 3 High Byte    
    TMR3CN = 0x00;  // Timer 3 Control Register

	RSTSRC = 0x00;	// Reset Source Register

    IE = 0x80;         //Interrupt Enable
    IP = 0x00;         //Interrupt Priority
//    EIE1 = 0x00;       //EXtended Interrupt Enable
//    EIP1 = 0x00;       //EXtended Interrupt Priority
    IT01CF = 0x01;     //INT0/INT1 Configuration Register

}
