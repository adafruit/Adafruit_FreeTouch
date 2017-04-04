#include <Arduino.h>
#include "touch.h"
#include "touch_api_ptc.h"
#include "Adafruit_ASFcore.h"
#include "variant.h"
#include "gclk.h"
#include "clock.h"


#ifndef ADAFRUIT_FREETOUCH_H
#define ADAFRUIT_FREETOUCH_H



/*************** CONTROL A register ***************/
#define PTC_REG_CONTROLA   0x42004C00
#define PTC_BIT_ENABLE     0x02
#define PTC_BIT_RUNINSTBY  0x04


typedef union {
  struct {
    uint8_t   SWRESET:1;
    uint8_t   ENABLE:1;
    uint8_t   RUNINSTANDBY:1;
    uint8_t   __pad0__:5;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLA_Type;

/*************** CONTROL B register ***************/

#define PTC_REG_CONTROLB 0x42004C01
#define PTC_BIT_SYNCFLAG 0x80

typedef union {
  struct {
    uint8_t   __pad0__:7;
    uint8_t   SYNCFLAG:1;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLB_Type;

/*************** CONTROL C register ***************/

#define PTC_REG_CONTROLC 0x42004C05
#define PTC_BIT_INIT 0x01

typedef union {
  struct {
    uint8_t   INIT:1;
    uint8_t   __pad0__:7;
  } bit;
  uint8_t reg;
} PTC_REG_CONTROLC_Type;



/*************** INT registers ***************/

typedef union {
  struct {
    uint8_t   EOC:1;
    uint8_t   WCO:1;
    uint8_t   __pad0__:6;
  } bit;
  uint8_t reg;
} PTC_REG_INT_Type;


#define PTC_REG_INTDISABLE 0x42004C08
#define PTC_REG_INTENABLE 0x42004C09
#define PTC_BIT_EOCINTEN  0x01
#define PTC_BIT_WCOINTEN  0x02

#define PTC_REG_INTFLAGS 0x42004C0A
#define PTC_BIT_EOCINTFLAG 0x01
#define PTC_BIT_WCOINTFLAG 0x02


/*************** FREQ CONTROL reg ***************/

typedef union {
  struct {
    uint8_t   SAMPLEDELAY:4;
    uint8_t   FREQSPREADEN:1;
    uint8_t   __pad0__:3;
  } bit;
  uint8_t reg;
} PTC_REG_FREQCONTROL_Type;

#define PTC_REG_FREQCONTROL 0x42004C0C
#define PTC_BIT_FREQSPREADEN 0x10
#define PTC_REG_SAMPLEDELAY_MASK 0x0F

/*************** CONVERT CONTROL reg ***************/

typedef union {
  struct {
    uint8_t   ADCACCUM:3;
    uint8_t   __pad0__:4;
    uint8_t   CONVERT:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_CONVCONTROL_Type;


#define PTC_REG_CONVCONTROL 0x42004C0D
#define PTC_BIT_CONVSTARTED 0x80
#define PTC_REG_ADCACC_MASK 0x07


/*************** Y SELECT L+H reg ***************/

typedef union {
  struct {
    uint8_t   Y0:1;
    uint8_t   Y1:1;
    uint8_t   Y2:1;
    uint8_t   Y3:1;
    uint8_t   Y4:1;
    uint8_t   Y5:1;
    uint8_t   Y6:1;
    uint8_t   Y7:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_YSELECTL_Type;

typedef union {
  struct {
    uint8_t   Y8:1;
    uint8_t   Y9:1;
    uint8_t   Y10:1;
    uint8_t   Y11:1;
    uint8_t   Y12:1;
    uint8_t   Y13:1;
    uint8_t   Y14:1;
    uint8_t   Y15:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_YSELECTH_Type;

#define PTC_REG_YSELECT_L 0x42004C10
#define PTC_REG_YSELECT_H 0x42004C11

#define PTC_REG_YENABLE_L 0x42004C14
#define PTC_REG_YENABLE_H 0x42004C15


/*************** X SELECT L+H reg ***************/

typedef union {
  struct {
    uint8_t   X0:1;
    uint8_t   X1:1;
    uint8_t   X2:1;
    uint8_t   X3:1;
    uint8_t   X4:1;
    uint8_t   X5:1;
    uint8_t   X6:1;
    uint8_t   X7:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_XSELECTL_Type;

typedef union {
  struct {
    uint8_t   X8:1;
    uint8_t   X9:1;
    uint8_t   X10:1;
    uint8_t   X11:1;
    uint8_t   X12:1;
    uint8_t   X13:1;
    uint8_t   X14:1;
    uint8_t   X15:1;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_XSELECTH_Type;


#define PTC_REG_XSELECT_L 0x42004C12
#define PTC_REG_XSELECT_H 0x42004C13

#define PTC_REG_XENABLE_L 0x42004C16
#define PTC_REG_XENABLE_H 0x42004C17

/*************** Compensation Cap reg ***************/

typedef union {
  struct {
    uint8_t   value:8;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_COMPCAPL_Type;

typedef union {
  struct {
    uint8_t   value:6;
    uint8_t   __pad0__:2;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_COMPCAPH_Type;


/*************** Int Cap reg ***************/

typedef union {
  struct {
    uint8_t   value:6;
    uint8_t   __pad0__:2;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_INTCAP_Type;

/*************** Series resistor reg ***************/

typedef union {
  struct {
    uint8_t   RESISTOR:2;
    uint8_t   __pad0__:6;
  } bit;
  uint8_t reg;
} __attribute__ ((packed)) PTC_REG_SERRES_Type;

#define PTC_REG_SERIESRES    0x42004C1B



#define PTC_REG_CONVRESULT_L 0x42004C1C
#define PTC_REG_CONVRESULT_H 0x42004C1D


#define PTC_REG_XYENABLE     0x42004C16
#define PTC_BIT_XYENABLE     0x02

#define PTC_REG_BURSTMODE     0x42004C20
#define PTC_REG_BURSTMODE_MASK   0xF0
#define PTC_BIT_CTSLOWPOWER   0x04

#define PTC_REG_WCO_MODE     0x42004C21
#define PTC_REG_WCO_MODE_MASK   0x07

#define PTC_SET_WCO_THRESHHOLD_A_L 0x42004C24
#define PTC_SET_WCO_THRESHHOLD_A_H 0x42004C25
#define PTC_SET_WCO_THRESHHOLD_B_L 0x42004C26
#define PTC_SET_WCO_THRESHHOLD_B_H 0x42004C27



typedef struct {
  __IO PTC_REG_CONTROLA_Type CONTROLA;    // 0x42004C00
  __IO PTC_REG_CONTROLB_Type CONTROLB;    // 0x42004C01
  uint8_t   __pad2__;                // 0x42004C02 unknown
  uint8_t   __pad3__;                // 0x42004C03 unknown
  uint8_t   __pad4__;                // 0x42004C04 unknown
  __IO PTC_REG_CONTROLC_Type CONTROLC;    // 0x42004C05
  uint8_t   __pad6__;                // 0x42004C06 unknown
  uint8_t   __pad7__;                // 0x42004C07 unknown
  __IO PTC_REG_INT_Type      INTDISABLE;  // 0x42004C08
  __IO PTC_REG_INT_Type      INTENABLE;   // 0x42004C09
  __I  PTC_REG_INT_Type      INTFLAGS;    // 0x42004C0A
  uint8_t   __padb__;                // 0x42004C0B unknown
  __IO PTC_REG_FREQCONTROL_Type FREQCONTROL;  //0x42004C0C
  __IO PTC_REG_CONVCONTROL_Type CONVCONTROL;  // 0x42004C0D
  uint8_t   __pade__;                // 0x42004C0E unknown
  uint8_t   __padf__;                // 0x42004C0F unknown
  __IO PTC_REG_YSELECTL_Type YSELECTL;  // 0x42004C10
  __IO PTC_REG_YSELECTL_Type YSELECTH;  // 0x42004C11
  __IO PTC_REG_XSELECTL_Type XSELECTL;  // 0x42004C12
  __IO PTC_REG_XSELECTL_Type XSELECTH;  // 0x42004C13
  __IO PTC_REG_YSELECTL_Type YENABLEL;  // 0x42004C14
  __IO PTC_REG_YSELECTL_Type YENABLEH;  // 0x42004C15
  __IO PTC_REG_XSELECTL_Type XENABLEL;  // 0x42004C16
  __IO PTC_REG_XSELECTL_Type XENABLEH;  // 0x42004C17

  __IO PTC_REG_COMPCAPL_Type COMPCAPL;  // 0x42004C18
  __IO PTC_REG_COMPCAPH_Type COMPCAPH;  // 0x42004C19
  __IO PTC_REG_INTCAP_Type   INTCAP;    // 0x42004C1A
  __IO PTC_REG_SERRES_Type   SERRES;    // 0x42004C1B

} Qtouch_Ptc;

#define QTOUCH_PTC  (( Qtouch_Ptc *)0x42004C00U)


#define PTC_REG_INTDISABLE 0x42004C08
#define PTC_REG_INTENABLE 0x42004C09
#define PTC_BIT_EOCINTEN  0x01
#define PTC_BIT_WCOINTEN  0x02

#define PTC_REG_INTFLAGS 0x42004C0A



class Adafruit_FreeTouch {
 public:
  Adafruit_FreeTouch(int p, filter_level_t f = FILTER_LEVEL_4, rsel_val_t r = RSEL_VAL_0, freq_mode_sel_t fh = FREQ_MODE_NONE);
  bool begin(void);

  void ptcConfigIOpin(void);
  uint16_t startPtcAcquire(void);
  uint16_t touchSelfcapSensorsMeasure(void);

  // debugging helper!
  void snapshotRegsAndPrint(uint32_t base, uint8_t numregs);
  void printPTCregs(uint32_t base, uint8_t *regs, uint8_t num);
  void printHex(uint8_t h, boolean newline);

  void setFilterLevel(filter_level_t lvl);
  void setSeriesResistor(rsel_val_t res);
  void setFreqHopping(freq_mode_sel_t fh, freq_hop_sel_t hops = FREQ_HOP_SEL_1);

  void runInStandby(boolean en);
  void enablePTC(boolean en);
  void enableWCOint(boolean en);
  void clearWCOintFlag(void);
  void clearEOCintFlag(void);
  void ptcAcquire(void);
  void sync_config(void);

 private:
  int pin;           // arduino pin #
  int8_t yline;      // the Y select line (see datasheet)
  filter_level_t oversample;
  rsel_val_t seriesres;
  freq_mode_sel_t freqhop;
  freq_hop_sel_t hops;
};




#endif
