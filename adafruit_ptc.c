/*
 * FreeTouch, a QTouch-compatible library - tested on ATSAMD21 only!
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Limor 'ladyada' Fried for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "adafruit_ptc.h"
// #include "pinmux.h"

// Spin-waits until a clock-synchronization boundary is reached. Needed when
// reading and writing memory registers in a peripheral that may potentially
// be clocked differently than the ARM CPU.

static void sync_config(Ptc const* module_inst) {
    while (module_inst->CONTROLB.bit.SYNCFLAG) ;
}

void adafruit_ptc_get_config_default(struct adafruit_ptc_config *config) {
    config->pin = 0xff;
    config->yline = -1;
    config->oversample = OVERSAMPLE_4;
    config->seriesres = RESISTOR_0;
    config->freqhop = FREQ_MODE_NONE;
    config->compcap = 0x2000;
    config->intcap = 0x3F;
}

void adafruit_ptc_init(Ptc* module_inst, struct adafruit_ptc_config const* config) {

    // Note: a precondition is that the pin in question must be set to mux position 1
    // without pullups. In this library, that functionality is handled by the OO class.

    sync_config(module_inst);
    // Disable PTC (needed for many register writes to be successful)
    module_inst->CONTROLA.bit.ENABLE = 0;


    // Configure a block of as-yet unidentified configuration registers. If
    // you know what these do, please contact Jeremy Gilbert
    sync_config(module_inst);
    module_inst->UNK4C04.reg &= 0xF7; //MEMORY[0x42004C04] &= 0xF7u;
    module_inst->UNK4C04.reg &= 0xFB; //MEMORY[0x42004C04] &= 0xFBu;
    module_inst->UNK4C04.reg &= 0xFC; //MEMORY[0x42004C04] &= 0xFCu;

    // Frequency control (unclear what this does in self-capacitance mode)
    sync_config(module_inst);
    module_inst->FREQCONTROL.reg &= 0x9F;       //MEMORY[0x42004C0C] &= 0x9Fu;
    sync_config(module_inst);
    module_inst->FREQCONTROL.reg &= 0xEF;       //MEMORY[0x42004C0C] &= 0xEFu;
    sync_config(module_inst);
    module_inst->FREQCONTROL.bit.SAMPLEDELAY = 0; //MEMORY[0x42004C0C] &= 0xF0u;

    // Initialize the entire peripheral
    // NB: The Atmel libraries do this FIRST and the Adafruit does it much later.
    // Leaving it that way since I'm assuming AF has inside info.
    // Why no sync_config() here?
    module_inst->CONTROLC.bit.INIT = 1;         //MEMORY[0x42004C05] |= 1u;
    module_inst->CONTROLA.bit.RUNINSTANDBY = 1; //MEMORY[0x42004C00] |= 4u;
    sync_config(module_inst);

    // Don't know what WCO interrupt does exactly but I think it is related to
    // autoreading
    module_inst->INTDISABLE.bit.WCO = 1;
    sync_config(module_inst);

    // EOC is the "end of conversion" interrupt.
    module_inst->INTDISABLE.bit.EOC = 1;
    sync_config(module_inst);

    // enable the sensor, only done once per line
    if (config->yline < 8) {
        sync_config(module_inst);
        module_inst->YENABLEL.reg |= 1 << config->yline;
        sync_config(module_inst);
    } else if (config->yline < 16) {
        module_inst->YENABLEH.reg |= 1 << (config->yline - 8);
    }
    sync_config(module_inst);

    // Re-enable the module, which has the likely side effect of latching some
    // of the above configuration into somewhere outside of the memory mapped
    // IO space. A similiar pattern is seen with the Atmel ADC module. 
    module_inst->CONTROLA.bit.ENABLE = 1;
    sync_config(module_inst);
}

void adafruit_ptc_start_conversion(Ptc* module_inst, struct adafruit_ptc_config const* config) {
    module_inst->CONTROLA.bit.RUNINSTANDBY = 1;
    sync_config(module_inst);
    module_inst->CONTROLA.bit.ENABLE = 1;
    sync_config(module_inst);
    module_inst->INTDISABLE.bit.WCO = 1;
    sync_config(module_inst);
    module_inst->INTFLAGS.bit.WCO = 1;
    sync_config(module_inst);
    module_inst->INTFLAGS.bit.EOC = 1;
    sync_config(module_inst);

    // set up pin!
    sync_config(module_inst);
    if (config->yline < 8) {
        module_inst->YSELECTL.reg = 1 << config->yline;
    } else {
        module_inst->YSELECTL.reg = 0;
    }

    if (config->yline > 7) {
        module_inst->YSELECTH.reg = 1 << (config->yline - 8);
    } else {
        module_inst->YSELECTH.reg = 0;
    }

    sync_config(module_inst);
    // set up sense resistor
    module_inst->SERRES.bit.RESISTOR = config->seriesres;
    sync_config(module_inst);
    // set up prescalar
    module_inst->CONVCONTROL.bit.ADCACCUM = config->oversample;
    sync_config(module_inst);
    // set up freq hopping
    if (config->freqhop == FREQ_MODE_NONE) {
        module_inst->FREQCONTROL.bit.FREQSPREADEN = 0;
        module_inst->FREQCONTROL.bit.SAMPLEDELAY = 0;
    } else {
        module_inst->FREQCONTROL.bit.FREQSPREADEN = 1;
        module_inst->FREQCONTROL.bit.SAMPLEDELAY = config->hops;
    }
    // set up compensation cap + int (?) cap
    sync_config(module_inst);
    module_inst->COMPCAPL.bit.VALUE = config->compcap & 0xFF;
    module_inst->COMPCAPH.bit.VALUE = (config->compcap>>8) & 0x3F;
    sync_config(module_inst);
    module_inst->INTCAP.bit.VALUE = config->intcap & 0x3F;
    sync_config(module_inst);

    module_inst->BURSTMODE.reg = 0xA4;
    sync_config(module_inst);

    module_inst->CONVCONTROL.bit.CONVERT = 1;
    sync_config(module_inst);
}

int adafruit_ptc_is_conversion_finished(Ptc* module_inst) {
    return module_inst->CONVCONTROL.bit.CONVERT == 0;
}

uint16_t adafruit_ptc_get_conversion_result(Ptc* module_inst) {
    sync_config(module_inst);
    return module_inst->RESULT.reg;
}
