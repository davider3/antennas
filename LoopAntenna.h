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
#define TREE_SIZE 31
#define ADC_CONVERT .003207

class LoopAntenna{
private:
    struct GainInfo{
        int gain;
        uint8_t gain1;
        uint8_t gain2;

        GainInfo(int g, uint8_t g1, uint8_t g2) : gain(g), gain1(g1), gain2(g2) {};
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
    GainInfo tree[GAINS_SIZE] = {GainInfo(80, GAIN_10X, GAIN_8X),     // 80x
                                 GainInfo(20, GAIN_5X, GAIN_4X),      // 20x
                                 GainInfo(256, GAIN_16X, GAIN_16X),   // 256x
                                 GainInfo(5, GAIN_5X, GAIN_1X),       // 5x
                                 GainInfo(50, GAIN_10X, GAIN_5X),     // 50x
                                 GainInfo(128, GAIN_16X, GAIN_8X),    // 128x
                                 GainInfo(512, GAIN_32X, GAIN_16X),   // 512x
                                 GainInfo(2, GAIN_2X, GAIN_1X),       // 2x
                                 GainInfo(10, GAIN_5X, GAIN_2X),      // 10x
                                 GainInfo(32, GAIN_8X, GAIN_4X),      // 32x
                                 GainInfo(64, GAIN_8X, GAIN_8X),      // 64x
                                 GainInfo(100, GAIN_10X, GAIN_10X),   // 100x
                                 GainInfo(160, GAIN_16X, GAIN_10X),   // 160x
                                 GainInfo(320, GAIN_32X, GAIN_10X),   // 320x
                                 GainInfo(1024, GAIN_32X, GAIN_32X),  // 1024x
                                 GainInfo(1, GAIN_1X, GAIN_1X),       // 1x
                                 GainInfo(4, GAIN_2X, GAIN_2X),       // 4x
                                 GainInfo(8, GAIN_4X, GAIN_2X),       // 8x
                                 GainInfo(16, GAIN_4X, GAIN_4X),      // 16x
                                 GainInfo(25, GAIN_5X, GAIN_5X),      // 25x
                                 GainInfo(40, GAIN_8X, GAIN_5X)};     // 40x
 
public: 
    LoopAntenna(MCP6S2x* ampA, MCP6S2x* ampB, 
                uint8_t ch1, uint8_t ch2);

    void measureAmplitude();
    void calcAmplitude();
    void bst(int desired);
    int getTotalGain();
    bool isFlat();
    void calcMean();
    int getAmplitude();
    float getOGAmplitude();
    
};


#endif