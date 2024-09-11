#ifndef LOOP_ANTENNA_H
#define LOOP_ANTENNA_H

#include <Arduino.h>
#include "MCP6S2x.h"

#define TARGET 700
#define MINIMUM 350
#define MAXIMUM 900
#define FLAT 1
#define SAMPLES 200
#define GAINS_SIZE 21
#define ADC_CONVERT .003207

class LoopAntenna{
private:
    struct Pair{
        uint8_t gain1;
        uint8_t gain2;

        Pair(uint8_t g1, uint8_t g2) : gain1(g1), gain2(g2) {};
    };

    MCP6S2x* amp1;
    MCP6S2x* amp2;
    uint8_t amp1Ch;
    uint8_t amp2Ch;
    uint8_t i;
    uint8_t index;
    bool above;
    int amplitude;
    int totalGain;
    int mean;
    int readings[SAMPLES];
    int gainVals[GAINS_SIZE+1] = {1,2,4,5,8,10,16,20,25,32,40,50,64,80,
                                  100,128,160,256,320,512,1024,INT16_MAX};
    Pair gains[GAINS_SIZE] = {Pair(GAIN_1X, GAIN_1X),   // 1x
                              Pair(GAIN_2X, GAIN_1X),   // 2x
                              Pair(GAIN_2X, GAIN_2X),   // 4x
                              Pair(GAIN_5X, GAIN_1X),   // 5x
                              Pair(GAIN_4X, GAIN_2X),   // 8x
                              Pair(GAIN_5X, GAIN_2X),   // 10x
                              Pair(GAIN_4X, GAIN_4X),   // 16x
                              Pair(GAIN_5X, GAIN_4X),   // 20x
                              Pair(GAIN_5X, GAIN_5X),   // 25x
                              Pair(GAIN_8X, GAIN_4X),   // 32x
                              Pair(GAIN_8X, GAIN_5X),   // 40x
                              Pair(GAIN_10X, GAIN_5X),  // 50x
                              Pair(GAIN_8X, GAIN_8X),   // 64x
                              Pair(GAIN_10X, GAIN_8X),  // 80x
                              Pair(GAIN_10X, GAIN_10X), // 100x
                              Pair(GAIN_16X, GAIN_8X),  // 128x
                              Pair(GAIN_16X, GAIN_10X), // 160x
                              Pair(GAIN_16X, GAIN_16X), // 256x
                              Pair(GAIN_32X, GAIN_10X), // 320x
                              Pair(GAIN_32X, GAIN_16X), // 512x
                              Pair(GAIN_32X, GAIN_32X)};// 1024x
 
public: 
    LoopAntenna(MCP6S2x* ampA, MCP6S2x* ampB, 
                uint8_t ch1, uint8_t ch2);

    void measureAmplitude();
    void calcAmplitude();
    int getTotalGain();
    bool isFlat();
    void calcMean();
    int getAmplitude();
    float getOGAmplitude();
    
};


#endif