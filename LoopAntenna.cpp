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
}


/** @brief Measures the amplitude of the signal.
 *  Dynamically adjusts the gain to maintain the signal within a specified range.
 */
void LoopAntenna::measureAmplitude(){
    amp1->changeChannel(amp1Ch);
    amp2->changeChannel(amp2Ch);

    above = false;

    // DYNAMICALLY CHANGE THE GAIN
    while(1){

        // READ IN SIGNAL
        for(i=0; i<SAMPLES; ++i)
            readings[i] = amp2->getValue();
        
        // CALCULATE MAX AMPLITUDE
        calcAmplitude();
        
        // IF THE SIGNAL IS LOW INCREASE GAIN
        if(amplitude < MINIMUM){

            // IF THE WAVE HAS FLATTENED AMPLITUDE IS 0
            // if(isFlat()){
            //     amplitude = 0;
            //     return amplitude;
            // }
            
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


/** @brief Calculates the maximum amplitude from the sampled readings.
 */
void LoopAntenna::calcAmplitude(){
    amplitude = 0;
    for(i=0; i<SAMPLES; ++i){
        if(readings[i] > amplitude)
            amplitude = readings[i];
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


/** @brief Checks if the waveform has flattened.
 *  @return True if the waveform is flat, false otherwise.
 */
bool LoopAntenna::isFlat(){
    calcMean();

    if((amplitude - mean) < FLAT){
        return true;
    }else
        return false;

}


/** @brief Calculates the mean of the sampled readings.
 */
void LoopAntenna::calcMean(){
    int sum = 0;
    for(i=0; i<SAMPLES; ++i){
        sum += readings[i];
    }

    mean = sum / SAMPLES;
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
    return 2 * ((amplitude * ADC_CONVERT) / totalGain);
}
