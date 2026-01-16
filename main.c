/**
 * A few things to note here: 
 * 
 * 1. This file must contain the config bits from MCC
 * 2. Otherwise this file delegates the configuration to the BaseBoardCore package
 * 
 * In order to link the two, you'll need to:
 * 
 * 1. Right Click "Libraries" and add the BaseBoardCore.x library
 * 2. Go to Properties -> XC8 Compiler -> Include Directories and add the 
 *    BaseBoardCore.x library
 * 
 */


#include "run.h"
#include "log.h"
#include "lora.h"
#include "adcc_manager.h"
#include <stdio.h>
#include "mcc_generated_files/system/config_bits.h"


// These are the lower (wet) and upper (dry) values I saw when doing a basic 
// test. These need to be device specific, and probably added to eeprom via
// some mechanism. 
uint16_t LOW_VALUE = 440;
double HIGH_VALUE = 824.0;

void task(void) {
    LED_EN_SetHigh();
    IO1_SetHigh();
    __delay_ms(2000);
    
    // Reading the moisture sensor
    uint16_t raw = adcc_mgr_get_raw_conversion(ADCC_1);
    char buf[50];
    sprintf(buf, "Raw: %d", raw);
    log_debug(buf);
    
    // Clamp the raw value to the upper and lower bound.  
    if(raw < LOW_VALUE) {
        raw = LOW_VALUE;
    }
    
    if(raw > HIGH_VALUE) {
        raw = HIGH_VALUE;
    }
    
    // Adjust the raw voltage down by the low threshold. Then create a ratio 
    // between the adjusted value and the high value (adjusted down). Since the
    // wet indication is low and dry indication high, we need to reverse the
    // percentage to show what percentage of water is present.
    uint16_t adjusted = raw - LOW_VALUE;
    double percent = (1 - adjusted / (HIGH_VALUE - LOW_VALUE)) * 100;
    
    IO1_SetLow();
    lora_enable();
    double data[] = {percent};
    lora_send(32, data, 1);
    
    LED_EN_SetLow();
    set_sleep_period(THIRTY_MINUTES);
}
 
int main(void) {
    return run(task);   
}