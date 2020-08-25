#ifndef _SLCUSTOM_H_
#define _SLCUSTOM_H_

#include <SLSoftwareSerial.h>
#include "temperature.h"
#include "cardreader.h"

#define SLCUSTOM_MSG_HELLO "Hello\n"
#define SLCUSTOM_BAUD_RATE 115200
#define SLCUSTOM_RUN_EVERY_MILLIS 10000 // print status every 10 seconds

// ONE WAY SERIAL - TX ONLY ON FREE PIN (requires soldering to unused header next to capacitor)
#define TX_PIN 29

class SLCustom {

  private:
  SLSoftwareSerial ser;
  unsigned long last_run_millis = 0;
  unsigned long now = 0;
  

  public:
    SLCustom() : ser(TX_PIN) {
      ser.begin(SLCUSTOM_BAUD_RATE);
    }

    void setup(){}

    void loop(){
      now = millis();
      if ( now - last_run_millis > SLCUSTOM_RUN_EVERY_MILLIS ){
        printStatus();
        last_run_millis = now;
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

    // S {hotEndTemp} {hotEndTargetTemp} {bedTemp} {bedTargetTemp} {isSDPrinting} {percentDone} {timeTaken} {filename}
    void printStatus(){
        // some helper functions in here could reduce code a few bytes...but not likely enough to matter.

        // S = Status
        ser.print("S ");
        ser.print(' ');

        // hot end temp
        ser.print(thermalManager.degHotend(HOTEND_INDEX)); // end temp -- this will break if HOTENDS > 1
        ser.print(' ');

        // hot end target temp
        ser.print(thermalManager.degTargetHotend(HOTEND_INDEX)); // end target temp
        ser.print(' ');

        // bed temp
        ser.print(thermalManager.degBed());
        ser.print(' ');

        // bed target temp
        ser.print(thermalManager.degTargetBed());
        ser.print(' ');

        // printing status
        if ( card.cardOK && card.sdprinting ){
          
          ser.print('1');
          ser.print(' ');

          // percent done
          ser.print(card.percentDone());
          ser.print(' ');

          char buffer[21];
          duration_t elapsed = print_job_timer.duration();
          elapsed.toString(buffer);
          ser.print(buffer);

          card.printFilenameTo(&ser);

        } else {
          ser.print("0 0 0 NA");
        }
        ser.print(' ');

        ser.print('\n');  // command ends with newline
    }

};


#endif
