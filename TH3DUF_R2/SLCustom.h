#ifndef _SLCUSTOM_H_
#define _SLCUSTOM_H_

#include <SLSoftwareSerial.h>

#define SLCUSTOM_MSG_HELLO "Hello\n"
#define SLCUSTOM_BAUD_RATE 115200
#define SLCUSTOM_RUN_EVERY_MILLIS 10000

// ONE WAY SERIAL - TX ONLY ON FREE PIN (requires soldering to unused header next to capacitor)
#define TX_PIN 29

class SLCustom {

  private:
  SLSoftwareSerial ser;
  unsigned long last_run_millis = 0;
  

  public:
    SLCustom() : ser(TX_PIN) {
      ser.begin(SLCUSTOM_BAUD_RATE);
    }

    void setup(){}

    void loop(){
      last_run_millis = millis();
      if ( millis() - last_run_millis > SLCUSTOM_RUN_EVERY_MILLIS ){
        ser.print("S ");
        ser.print(123.0); // end temp
        ser.print(' ');
        ser.print(205.0); // end target temp
        ser.print('\n');        
      }
    }


    void tone(const uint16_t &duration, const uint16_t &frequency=0) {
      // Command: B {duration} {frequency}

      // runtime is 2+5+1+5+1 bytes is 144us@115200 baud?  check math. 
      ser.print("B ");
      ser.print(duration);
      ser.print(' ');
      ser.print(frequency);
      ser.print('\n');
    }

    

};


#endif
