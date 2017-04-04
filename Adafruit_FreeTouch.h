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



#define PTC_REG_CONVRESULT_L 0x42004C1C
#define PTC_REG_CONVRESULT_H 0x42004C1D

#define PTC_REG_XYSELECT_A_L 0x42004C10
#define PTC_REG_XYSELECT_A_H 0x42004C11
#define PTC_REG_XYSELECT_B_L 0x42004C12
#define PTC_REG_XYSELECT_B_H 0x42004C13

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
} Qtouch_Ptc;

#define QTOUCH_PTC  (( Qtouch_Ptc *)0x42004C00U)


#define PTC_REG_INTDISABLE 0x42004C08
#define PTC_REG_INTENABLE 0x42004C09
#define PTC_BIT_EOCINTEN  0x01
#define PTC_BIT_WCOINTEN  0x02

#define PTC_REG_INTFLAGS 0x42004C0A



class Adafruit_FreeTouch {
 public:

  void ptcConfigIOpin(int ulPin);
  uint16_t startPtcAcquire(int p);
  uint16_t touchSelfcapSensorsMeasure(int p);

  // debugging helper!
  void snapshotRegsAndPrint(uint32_t base, uint8_t numregs);
  void printPTCregs(uint32_t base, uint8_t *regs, uint8_t num);
  void printHex(uint8_t h, boolean newline);


  void runInStandby(boolean en);
  void enablePTC(boolean en);
  void enableWCOint(boolean en);
  void clearWCOintFlag(void);
  void clearEOCintFlag(void);
  void ptcAcquire(void);
  void sync_config(void);
};




#endif
