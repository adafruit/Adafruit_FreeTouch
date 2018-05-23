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

static void sync_config(Ptc const* module_inst) {
    while (module_inst->CTRLB.bit.SYNCFLAG);
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
    // Configure the pin.
    PortGroup* group = &PORT->Group[config->pin / 32];
    uint8_t pin = config->pin % 32;
    uint32_t pin_mask = (1UL << pin);
    uint32_t wr_pin_mask;
    uint32_t half_word_select;
    if (pin > 15) {
        wr_pin_mask = pin_mask >> 16;
        half_word_select = PORT_WRCONFIG_HWSEL;
    } else {
        wr_pin_mask = pin_mask & 0xffff;
        half_word_select = 0;
    }
    // Atomically change the pin config and the pin mux.
    group->WRCONFIG.reg = wr_pin_mask |
                          PORT_WRCONFIG_WRPINCFG |
                          PORT_WRCONFIG_WRPMUX |
                          PORT_WRCONFIG_PMUXEN |
                          PORT_WRCONFIG_PMUX(1) |
                          half_word_select;
    // Make sure output is off.
    group->DIRCLR.reg = pin_mask;

    sync_config(module_inst);
    module_inst->CTRLA.bit.ENABLE = 0;
    sync_config(module_inst);

    module_inst->UNK4C04.reg &= 0xF7; //MEMORY[0x42004C04] &= 0xF7u;
    module_inst->UNK4C04.reg &= 0xFB; //MEMORY[0x42004C04] &= 0xFBu;
    module_inst->UNK4C04.reg &= 0xFC; //MEMORY[0x42004C04] &= 0xFCu;
    sync_config(module_inst);
    module_inst->FREQCTRL.reg &= 0x9F;       //MEMORY[0x42004C0C] &= 0x9Fu;
    sync_config(module_inst);
    module_inst->FREQCTRL.reg &= 0xEF;       //MEMORY[0x42004C0C] &= 0xEFu;
    sync_config(module_inst);
    module_inst->FREQCTRL.bit.SAMPLEDELAY = 0; //MEMORY[0x42004C0C] &= 0xF0u;
    module_inst->CTRLC.bit.INIT = 1;         //MEMORY[0x42004C05] |= 1u;
    module_inst->CTRLA.bit.RUNINSTANDBY = 1; //MEMORY[0x42004C00] |= 4u;
    sync_config(module_inst);
    module_inst->INTDISABLE.bit.WCO = 1;
    sync_config(module_inst);
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
    module_inst->CTRLA.bit.ENABLE = 1;
    sync_config(module_inst);
}

void adafruit_ptc_start_conversion(Ptc* module_inst, struct adafruit_ptc_config const* config) {
    module_inst->CTRLA.bit.RUNINSTANDBY = 1;
    sync_config(module_inst);
    module_inst->CTRLA.bit.ENABLE = 1;
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
    module_inst->CONVCTRL.bit.ADCACCUM = config->oversample;
    sync_config(module_inst);
    // set up freq hopping
    if (config->freqhop == FREQ_MODE_NONE) {
        module_inst->FREQCTRL.bit.FREQSPREADEN = 0;
        module_inst->FREQCTRL.bit.SAMPLEDELAY = 0;
    } else {
        module_inst->FREQCTRL.bit.FREQSPREADEN = 1;
        module_inst->FREQCTRL.bit.SAMPLEDELAY = config->hops;
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

    module_inst->CONVCTRL.bit.CONVERT = 1;
    sync_config(module_inst);
}

bool adafruit_ptc_is_conversion_finished(Ptc* module_inst) {
    return module_inst->CONVCTRL.bit.CONVERT == 0;
}

uint16_t adafruit_ptc_get_conversion_result(Ptc* module_inst) {
    sync_config(module_inst);
    return module_inst->RESULT.reg;
}
