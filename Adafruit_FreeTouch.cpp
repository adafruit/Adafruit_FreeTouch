/*
 * Adapted from the MicroPython NativeIO TouchIn code!
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "Adafruit_FreeTouch.h"





uint16_t Adafruit_FreeTouch::touchSelfcapSensorsMeasure(int p) {
  runInStandby(true);    //  enable_run_in_stdby();
  enablePTC(true);       //   enable_ptc();
  // check if in progress
  // set up NVIC
  enableWCOint(false);
  clearWCOintFlag();
  clearEOCintFlag();
  // enable_eoc_int(); // not using irq for now

  // set up sense resistor
  // set up prescalar
  // set up freq hopping

  return startPtcAcquire(p);
}

uint16_t Adafruit_FreeTouch::startPtcAcquire(int p) {
  ptcConfigIOpin(p);

  ptcAcquire();

  //snapshotRegsAndPrint(0x42004C00, 48); // ptc
  //snapshotRegsAndPrint(0x41004430, 5); // pmux
  //snapshotRegsAndPrint(0x41004440, 5); // pinconfig
  //Serial.println("\n\n");

  digitalWrite(12, HIGH);
  while (QTOUCH_PTC->CONVCONTROL.bit.CONVERT) {
    Serial.print(".");
  }
  digitalWrite(12, LOW);

  Serial.println("Conversion ended! Got: ");
  uint16_t result = *(uint8_t *)(PTC_REG_CONVRESULT_H);
  result <<= 8;
  result |= *(uint8_t *)(PTC_REG_CONVRESULT_L);
  Serial.println(result);

  return result;
}

void Adafruit_FreeTouch::ptcConfigIOpin(int ulPin) {
  uint32_t pin = g_APinDescription[ulPin].ulPin;
  uint32_t port = g_APinDescription[ulPin].ulPort;
  
  PORT->Group[port].PINCFG[pin].reg = 0x3;  // pmuxen + input

  uint8_t pmux = PORT->Group[port].PMUX[(pin - (port*32))/2].reg;
  if (pin & 1) {
    // pmuxodd
    pmux &= 0x0F; // keep the even mux data
    pmux |= 0x10; // set to mux B
  } else {
    // pmuxeven
    pmux &= 0xF0;  // keep the odd mux data
    pmux |= 0x01;  // set to mux B
  }
  PORT->Group[port].PMUX[(pin - (port*32))/2].reg = pmux;
}



void Adafruit_FreeTouch::runInStandby(boolean en) {
  sync_config();
  if (en) {
    QTOUCH_PTC->CONTROLA.bit.RUNINSTANDBY = 1;
  } else {
    QTOUCH_PTC->CONTROLA.bit.RUNINSTANDBY = 0;
  }
  //Serial.print("ControlA: "); Serial.println(QTOUCH_PTC->CONTROLA.reg, HEX);
  sync_config();
}


void Adafruit_FreeTouch::enablePTC(boolean en) {
  sync_config();
  if (en) {
    QTOUCH_PTC->CONTROLA.bit.ENABLE = 1;
  } else {
    QTOUCH_PTC->CONTROLA.bit.ENABLE = 0;
  }
  //Serial.print("ControlA: "); Serial.println(QTOUCH_PTC->CONTROLA.reg, HEX);
  sync_config();
}

void Adafruit_FreeTouch::enableWCOint(boolean en) {
  sync_config();
  if (en) {
    QTOUCH_PTC->INTENABLE.bit.WCO = 1;
  } else {
    QTOUCH_PTC->INTDISABLE.bit.WCO = 1;
  } 
  sync_config();
}

void Adafruit_FreeTouch::clearWCOintFlag(void) {
  sync_config();
  QTOUCH_PTC->INTFLAGS.bit.WCO = 1;
  sync_config();
}

void Adafruit_FreeTouch::clearEOCintFlag(void) {
  sync_config();
  QTOUCH_PTC->INTFLAGS.bit.EOC = 1;
  sync_config();
}


void Adafruit_FreeTouch::ptcAcquire(void) {
  Serial.println("\t****Acquiring****");
  sync_config();

  QTOUCH_PTC->CONVCONTROL.bit.CONVERT = 1;
  //Serial.print("ConvCtrl: "); Serial.println(QTOUCH_PTC->CONVCONTROL.reg, HEX);
  sync_config();
}


void Adafruit_FreeTouch::sync_config(void) {
  while (QTOUCH_PTC->CONTROLB.bit.SYNCFLAG) ;
}



/**************************** DEBUGGING ASSIST *************************/
void Adafruit_FreeTouch::snapshotRegsAndPrint(uint32_t base, uint8_t numregs) {
  volatile uint32_t addr = base;
  uint8_t datas[255];

  digitalWrite(LED_BUILTIN, HIGH);
  for (uint8_t i=0; i<numregs; i++) {
    datas[i] = *(uint8_t *)(addr+i);
  }
  digitalWrite(LED_BUILTIN, LOW);
  printPTCregs(base, datas, numregs);
  
  for (uint8_t i=0; i<numregs; i++) {
  //  Serial.print("$"); Serial.print(addr+i, HEX); Serial.print("\t0x");
  //  printHex(datas[i]); Serial.println();
  }
  
}

// Print a hex with leading zero
void Adafruit_FreeTouch::printHex(uint8_t h, boolean newline) {
  if (h < 0x10) Serial.print("0");
  Serial.print(h, HEX);
  if (newline)  Serial.println();
}

void Adafruit_FreeTouch::printPTCregs(uint32_t base, uint8_t *regs, uint8_t num) {
  Serial.println("--------------------------------------------------------");
  for (uint8_t i=0; i<num; i++) {
      switch (i + base) {
        case 0x41004430:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PMUX0:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004431:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PMUX1:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004432:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PMUX2:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004433:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PMUX3:\t\t0x"); printHex(regs[i], true); break; 

        case 0x41004440:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PCFG0:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004441:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PCFG1:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004442:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PCFG2:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004443:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PCFG3:\t\t0x"); printHex(regs[i], true); break; 
        case 0x41004444:  Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" PCFG4:\t\t0x"); printHex(regs[i], true); break; 


        case 0x42004C00: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Control A:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C01: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Sync:     \t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C04: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Prescaler:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C05: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Init:     \t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C08: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Disable Irq:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C09: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Enable Irq:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C0A: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Flags:     \t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C0C: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Freq Cntl:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C0D: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Conv Cntl:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C10: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Y Select1:\t\t0x"); printHex(regs[i], true); break; 
/*
        case 0x42004C11: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" Y Select2:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C12: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" X Select1:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C13: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" X Select2:\t\t0x"); printHex(regs[i], true); break; 
*/
        case 0x42004C14: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Y Enable1:\t\t0x"); printHex(regs[i], true); break; 
/*
        case 0x42004C15: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" Y Enable2:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C16: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" X Enable1:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C17: Serial.print("0x"); Serial.print(i+0x42004C00, HEX);
          Serial.print(" X Enable2:\t\t0x"); printHex(regs[i], true); break; 
*/
        case 0x42004C18: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Compcap L:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C19: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Compcap H:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C1A: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Intcap:   \t\t0x"); printHex(regs[i], true); break;
        case 0x42004C1B: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Sense res:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C1C: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Result L:\t\t0x"); printHex(regs[i], true); break; 
        case 0x42004C1D: Serial.print("0x"); Serial.print(i+base, HEX);
          Serial.print(" Result H:\t\t0x"); printHex(regs[i], true); break; 
        
       }
  }
}
