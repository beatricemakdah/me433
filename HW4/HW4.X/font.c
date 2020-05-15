#include <string.h>
#include <xc.h>
#include "font.h"

void drawChar(unsigned char x, unsigned char y, char c){
    int j;
    int k;
    for (j=0; j<=4; j++){
        for (k=0; k<=7; k++){
            ssd1306_drawPixel(x+j, y+k, ((ASCII[c - 0x20][j])>>k)&1);
        }
    }
    ssd1306_update();
}

void drawString(unsigned char x, unsigned char y, char *message){
    char s = 0;
    int n = sizeof(message);
    
    while (message[s] != 0){
        drawChar(x+(5*s), y, message[s]);
        s++;
    }
}