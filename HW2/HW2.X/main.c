#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

void initSPI();
unsigned char spi_io(unsigned char o);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LATAbits.LATA4 = 0;
    
    initSPI();
    
    __builtin_enable_interrupts();
    
    //one period (1 second) of 1Hz triangle wave in 128 segments
    unsigned short triangleWave[128];
    int t = 0;
    while (t < 128){
        //first 0.5s of wave
        if (t < 64){
            triangleWave[t] = 64*t;
        }
        else if (t == 64){  //4096 (maximum voltage) was too high and would output 0
            triangleWave[t] = 4095;
        }
        //last 0.5s of wave
        else{
            triangleWave[t] = 8192-(64*t);
        }
        t++;
    }

    //1 periods (0.5 second) of 2Hz sine wave in 64 segments
    unsigned short sineWave[64] = {2048, 2249, 2448, 2643, 2832, 3013, 3186, 3347,
    3496, 3631, 3751, 3854, 3940, 4008, 4057, 4086, 4095, 4086, 4057, 4008, 3940,
    3854, 3751, 3631, 3496, 3347, 3186, 3013, 2832, 2643, 2448, 2249, 2048, 1847,
    1648, 1453, 1264, 1083, 910, 749, 600, 465, 345, 242, 156, 88, 39, 10, 0, 10,
    39, 88, 156, 242, 345, 465, 600, 749, 910, 1083, 1264, 1453, 1648, 1847};

    unsigned char ii = 0;
    unsigned char jj = 0;
    //channel a
    unsigned short a = 0b0111000000000000;
    unsigned short dataToA;
    //channel b
    unsigned short b = 0b1111000000000000;
    unsigned short dataToB;
    while (1) {
        dataToA = a|sineWave[jj];
        dataToB = b|triangleWave[ii];
        
        //send data to channel a
        LATAbits.LATA0 = 0;
        spi_io(dataToA>>8);
        spi_io(dataToA);
        LATAbits.LATA0 = 1;
        
        //send data to channel b
        LATAbits.LATA0 = 0;
        spi_io(dataToB>>8);
        spi_io(dataToB);
        LATAbits.LATA0 = 1;
        
        ii++;
        jj++;
        if (ii == 127) {
            ii = 0;
        }
        if (jj == 63) {
            jj = 0;
        }
        _CP0_SET_COUNT(0);
        //wait for 1/128 seconds
        while (_CP0_GET_COUNT() < 48000000/256) { //300 makes the waves closer to the correct frequency
            
        }
    }
}

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn of analog pins
    ANSELA = 0;
    // Make an output pin for CS
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 1;    
    // Set SDO1
    RPA1Rbits.RPA1R = 0b0011;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}

unsigned char spi_io(unsigned char o) {
    SPI1BUF = o;
    while (!SPI1STATbits.SPIRBF){
        ;
    }
    return SPI1BUF;
}