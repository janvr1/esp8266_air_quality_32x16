#ifndef ledmatrix_h
#define ledmatrix_h

#include "Arduino.h"
#include "SPI.h"
#include <Ticker.h>

class ledMatrix{
    public:
        //ledMatrix();
        void init(int Ton=100, int Toff=0, int outputEnable=D8, int latch=D6,
                  int line_A=D1, int line_B=D2, int line_C=D3, int line_D=D4);
        void setPixel(int val, int x, int y, int c);
        void clearFrame(void);
        void drawGUI1(int HH, int mm, int ss, int DD, int MM, int YY);
        void drawGUI2(int co2, int hum, int temp);
        void showFrame(void);
        void drawChar(int x, int y, byte character[5], int color, int width=3);
        int RED = 0;
        int GREEN = 1;
        int OE;
        int LAT;
        int A;
        int B;
        int C;
        int D;
    private:
        uint32_t _frame[2][16];
        Ticker _displayUpdater;
        void _transferLine(int i);
        void _selectLine(int i);
        void _latchOutput(void);
        void _outputEnable(void);
        void _outputDisable(void);
        int _calculateTon(void);
        int _Ton;
        int _Toff;
};

#endif