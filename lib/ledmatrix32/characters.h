#ifndef charcters_h
#define characters_h

byte zero[5] = {0b000,
               0b010,
               0b010,
               0b010,
               0b000};

byte one[5] = {0b101,
               0b001,
               0b101,
               0b101,
               0b000};

byte two[5] = {0b000,
              0b110,
              0b000,
              0b011,
              0b000};

byte three[5] = {0b000,
                0b110,
                0b000,
                0b110,
                0b000};

byte four[5] = {0b010,
               0b010,
               0b000,
               0b110,
               0b110};

byte five[5] = {0b000,
               0b011,
               0b000,
               0b110,
               0b000};

byte six[5] = {0b000,
              0b011,
              0b000,
              0b010,
              0b000};

byte seven[5] = {0b000,
                0b110,
                0b110,
                0b110,
                0b110};

byte eight[5] = {0b000,
                0b010,
                0b000,
                0b010,
                0b000};

byte nine[5] = {0b000,
               0b010,
               0b000,
               0b110,
               0b000};

byte colon[5] = {0b111,
                0b101,
                0b111,
                0b101,
                0b111};

byte dot[5] = {0b111,
              0b111,
              0b111,
              0b111,
              0b101};

byte procent[5] = {0b1111,
                  0b0110,
                  0b1101,
                  0b1011,
                  0b0110};

byte celsius[5] = {0b01000,
                  0b11011,
                  0b11011,
                  0b11011,
                  0b11000};

byte p[5] = {0b000,
            0b010,
            0b000,
            0b011,
            0b011};

byte m[5] = {0b01110,
            0b00100,
            0b01010,
            0b01110,
            0b01110};

byte *nums[10] = {zero, one, two, three, four, five, six, seven, eight, nine};

#endif