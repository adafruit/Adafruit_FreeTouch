


#ifndef ADAFRUIT_FREETOUCH_H
#define ADAFRUIT_FREETOUCH_H

#include <Arduino.h>
#include <Adafruit_ASFcore.h>
#include "variant.h"
#include "gclk.h"
#include "clock.h"
#include "adafruit_ptc.h"

class Adafruit_FreeTouch {
 public:
  Adafruit_FreeTouch(int p, oversample_t f = OVERSAMPLE_4, series_resistor_t r = RESISTOR_0, freq_mode_t fh = FREQ_MODE_NONE);
  bool begin(void);

  uint16_t measure(void);
  uint16_t measureRaw(void);

  // debugging helper!
  void snapshotRegsAndPrint(uint32_t base, uint8_t numregs);
  void printPTCregs(uint32_t base, uint8_t *regs, uint8_t num);
  void printHex(uint8_t h, boolean newline);

  int  getYLine(void);
  void selectYLine(void);
  void setOversampling(oversample_t lvl);
  void setSeriesResistor(series_resistor_t res);
  void setFreqHopping(freq_mode_t fh, freq_hop_t hops = FREQ_HOP_1);
  void setCompCap(uint16_t cc);
  void setIntCap(uint8_t ic);

   private:
    int pin;           // arduino pin #
    struct adafruit_ptc_config config;
};

#endif
