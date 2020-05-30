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

//scale scales how many pixels there are per number value returned by the accelerometer
void bar_x(signed short accel_x, unsigned char scale){ 
    int x_length = scale*accel_x/512; //512 numbers per pixel in x direction of screen
    int i;    
    for (i=0; i<64; i++){
        if (x_length < 0){
            if (i<=(-x_length)){
                ssd1306_drawPixel(63+i, 15, 1);
            } else {
                ssd1306_drawPixel(63+i, 15, 0);
            }
            ssd1306_drawPixel(i, 15, 0);
        }
        else {
            if (i<=x_length){
                ssd1306_drawPixel(63-i, 15, 1);
            } else {
                ssd1306_drawPixel(63-i, 15, 0);
            }
            ssd1306_drawPixel(127-i, 15, 0);
        }
    }
    ssd1306_update();
}

void bar_y(signed short accel_y, unsigned char scale){
    int y_length = scale*accel_y/2048; //2048 numbers per pixel in y direction of screen
    int i;
    for (i=0; i<15; i++){
        if (y_length < 0){
            if (i<=(-y_length)){
                ssd1306_drawPixel(63, 15-i, 1);
            } else {
                ssd1306_drawPixel(63, 15-i, 0);
            }
            ssd1306_drawPixel(63, 31-i, 0);
        } else{
            if (i<=y_length){
                ssd1306_drawPixel(63, 15+i, 1);
            } else{
                ssd1306_drawPixel(63, 15+i, 0);
            }
            ssd1306_drawPixel(63, i, 0);
        }
    }
    ssd1306_update();
}