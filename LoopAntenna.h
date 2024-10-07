#ifndef LOOP_ANTENNA_H
#define LOOP_ANTENNA_H

#include <Arduino.h>
#include "MCP6S2x.h"

#define TARGET 700
#define MINIMUM 350
#define MAXIMUM 900
#define FLAT 4
#define GAINS_SIZE 21
#define ADC_CONVERT .0032111
#define MAX_ITER 1023

class LoopAntenna{
private:
    struct GainInfo{
        int gain;
        uint8_t gain1;
        uint8_t gain2;

        GainInfo(int g, uint8_t g1, uint8_t g2) : gain(g), gain1(g1), gain2(g2) {};
    };
    enum States {INITIAL_CHECK, WAITING, GET_AMPLITUDE};

    MCP6S2x* amp1;
    MCP6S2x* amp2;
    uint8_t amp1Ch;
    uint8_t amp2Ch;
    uint8_t i;
    int j;
    uint8_t index;
    uint8_t count;
    bool above;
    bool exitLoop;
    int amplitude;
    int temp;
    int totalGain;
    int mean;
    States state;
    static GainInfo tree[GAINS_SIZE];
 
public: 
    LoopAntenna(MCP6S2x* ampA, MCP6S2x* ampB, 
                uint8_t ch1, uint8_t ch2);

    void measureAmplitude();
    void calcAmplitude();
    void bst(int desired);
    int getTotalGain();
    int getAmplitude();
    float getOGAmplitude();
    
};


#endif