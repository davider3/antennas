#include "LoopAntenna.h"


/** @brief Constructor for the LoopAntenna class.
 *  @param ampA Pointer to the first amplifier.
 *  @param ampB Pointer to the second amplifier.
 *  @param ch1  Channel for the first amplifier.
 *  @param ch2  Channel for the second amplifier.
 */
LoopAntenna::LoopAntenna(MCP6S2x* ampA, MCP6S2x* ampB, 
                         uint8_t ch1, uint8_t ch2){
    amp1 = ampA;
    amp2 = ampB;
    amp1Ch = ch1;
    amp2Ch = ch2;
    amp1->setGain(GAIN_1X);
    amp2->setGain(GAIN_1X);
    totalGain = 1;
    state = INITIAL_CHECK;
}


/**
 * TODO: docstring
 */
LoopAntenna::GainInfo LoopAntenna::tree[GAINS_SIZE] ={
    GainInfo(80, GAIN_10X, GAIN_8X),     // 80x
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
    GainInfo(40, GAIN_8X, GAIN_5X)       // 40x
};


/** @brief Measures the amplitude of the signal.
 *  Dynamically adjusts the gain to maintain the signal within a specified range.
 */
void LoopAntenna::measureAmplitude(){
    amp1->changeChannel(amp1Ch);
    amp2->changeChannel(amp2Ch);

    amp1->setGain(GAIN_1X);
    amp2->setGain(GAIN_1X);
    totalGain = 1;

    above = false;
    i = 0;

    // DYNAMICALLY CHANGE THE GAIN
    while(1){

        // CALCULATE MAX AMPLITUDE
        calcAmplitude();
        
        // IF THE SIGNAL IS LOW INCREASE GAIN
        if(amplitude < MINIMUM){

            if(amplitude == 0){
                return;
            }

            // CALCULATE DESIRED GAIN
            totalGain = (TARGET / amplitude) * totalGain;

            // FIND THE CLOSEST POSSIBLE GAIN
            bst(totalGain);

            // SET GAIN
            amp1->setGain(tree[index].gain1);
            amp2->setGain(tree[index].gain2);

        // IF THE SIGNAL IS HIGH DROP GAIN TO 1
        }else if(amplitude > MAXIMUM){
            // IF GAIN ALREADY AT 1 AND STILL ABOVE MAX
            if(above)
                return;

            amp1->setGain(GAIN_1X);
            amp2->setGain(GAIN_1X);
            totalGain = 1;

            above = true;
        
        // IF THE SIGNAL IS IN THE DESIRED RANGE THEN RETURN
        }else return;
    }
}


/** @brief Calculates the maximum amplitude of the sine wave input
 */
void LoopAntenna::calcAmplitude(){
  temp = amp2->getValue();
  amplitude = temp;
  exitLoop = false;
  count = 0;
  j = 0;
  while(1){
    switch(state) {

      case INITIAL_CHECK:
        temp = amp2->getValue();
        if(temp > amplitude + 5){
          state = GET_AMPLITUDE;
        } else{
          state = WAITING;
        }
        break;

      case WAITING:
        if(temp <= amplitude){
          count = 0;
          temp = amp2->getValue();
        } else if(count > 10){
          state = GET_AMPLITUDE;
          count = 0;
          break;
        } else{
          count += 1;
          temp = amp2->getValue();
        }
        break;

      case GET_AMPLITUDE:
        if(temp >= amplitude){
          count = 0;
          amplitude = temp;
          temp = amp2->getValue();
        } else if(count > 50){
          state = INITIAL_CHECK;
          delay(100);
          exitLoop = true;
          break;
        } else{
          count += 1;
          temp = amp2->getValue();
        }
        break;

    }

    if(exitLoop) break;

    ++j;
    if(j >= MAX_ITER){
        amplitude = 0;

        amp1->setGain(GAIN_1X);
        amp2->setGain(GAIN_1X);

        break;
    }
  }
}


/** @brief Finds the closest gain value that is less than or equal to the 
 *         desired value.
 *  
 *  This function navigates through a binary search tree of gain values 
 *  stored in an array format. Starting at the root (index 0), it compares 
 *  the desired value with the current gain. If the desired value is greater
 *  than the current gain, it moves to the right child; otherwise, it moves 
 *  to the left child. It continues this search until it finds the closest
 *  gain value less than or equal to the desired gain.
 * 
 *  @param desired The desired gain value to search for.
 */
void LoopAntenna::bst(int desired){
    i = 0;
    index = 15;
    totalGain = 1;
    while(i < GAINS_SIZE){
        if(desired > tree[i].gain){
            totalGain = tree[i].gain;
            index = i;
            i = i * 2 + 2;
        }else{
            i = i * 2 + 1;
        }
    }
}


/** @brief Gets the total gain applied to the signal.
 *  @return The total gain as an integer.
 */
int LoopAntenna::getTotalGain(){
    return totalGain;
}


/** @brief Gets the current amplitude.
 *  @return The current amplitude value.
 */
int LoopAntenna::getAmplitude(){
    return amplitude;
}


/** @brief Gets the original amplitude before gain adjustments.
 *  @return The original amplitude as a floating-point number.
 */
float LoopAntenna::getOGAmplitude(){
    return (2 * ((amplitude * ADC_CONVERT) / totalGain));
}
