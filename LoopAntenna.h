#ifndef LOOP_ANTENNA_H
#define LOOP_ANTENNA_H

#include <Arduino.h>
#include "MCP6S2x.h"

#define TARGET 700
#define MINIMUM 350
#define MAXIMUM 900
#define FLAT 4
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
    static GainInfo tree[GAINS_SIZE];
 
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