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

#define TAD_NS 125UL   // you said Fosc=32 MHz, ADCLK=Fosc/4 => 1 TAD = 125 ns

static float adc_avg_with_acq(uint8_t acq_counts, uint16_t n)
{
    float sum = 0.0;

    // --- Bare-register way to load acquisition time ---
    ADACQ = acq_counts;     // acquisition window in TADs

    // Optional: one "throwaway" conversion to stabilize pipeline
    float voltage = adcc_mgr_get_single_conversion(ANC5);
    for (uint16_t i = 0; i < n; i++) {
        sum += adcc_mgr_get_single_conversion(ANC5);
    }
    return (float)(sum / n);
}

void sweep_adacq(uint16_t samples_each)
{
    static const uint8_t acqs[] = {0,2,4,8,12,16,24,32,48,64,96,128,192,255};

    for (uint8_t i = 0; i < sizeof(acqs); i++) {
        uint8_t  acq   = acqs[i];
        float voltage  = adc_avg_with_acq(acq, samples_each);
        char buf[50];
        sprintf(buf, "ADACQ count: %u, Voltage: %.2fV", acq, voltage);
        log_debug(buf);
    }
}


void task(void) {
    LED_SetHigh();
    IO_RC6_SetHigh();
    
    __delay_ms(2000);
    
    sweep_adacq(256);
    
////        // Reading the moisture sensor
//        float voltage = adcc_mgr_get_single_conversion(ANC5);
//        char buf[50];
//        sprintf(buf, "Voltage: %.2fV", voltage);
//        log_debug(buf);
//    
////        // Reading the moisture sensor
//        voltage = adcc_mgr_get_single_conversion(ANC5);
//        buf[0] = '\0';
//        sprintf(buf, "Voltage: %.2fV", voltage);
//        log_debug(buf);
//    
////        // Reading the moisture sensor
//        voltage = adcc_mgr_get_single_conversion(ANC5);
//        buf[0] = '\0';
//        sprintf(buf, "Voltage: %.2fV", voltage);
//        log_debug(buf);
    
    
    log_debug("Hello from the application!");
    lora_enable();
    
//    double data[] = {123.456};
//    lora_send(32, data, 1);
    IO_RC6_SetLow();
    LED_SetLow();
}
 
int main(void) {
    return run(task);   
}