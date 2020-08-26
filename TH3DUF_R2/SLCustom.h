#ifndef _SLCUSTOM_H_
#define _SLCUSTOM_H_

#include <SLSoftwareSerial.h>
#include "temperature.h"
#include "cardreader.h"
#include "circularqueue.h"

#define SLCUSTOM_MSG_HELLO "Hello\n"
#define SLCUSTOM_BAUD_RATE 115200
#define SLCUSTOM_RUN_EVERY_MILLIS 10000 // print status every 10 seconds
#define SLCUSTOM_BUF_SIZE 512
#define SLCUSTOM_CONVERT_BUF_SIZE 16 // max characters for a number

// ONE WAY SERIAL - TX ONLY ON FREE PIN (requires soldering to unused header next to capacitor)
#define TX_PIN 29

class SLCustom {

  private:
  SLSoftwareSerial ser;
  unsigned long last_run_millis = 0;
  unsigned long now = 0;

  // output buffer, we only want to write 1 char per loop so we don't steal too much time from the main loop
  CircularQueue<char,255> buffer;

  // use to convert numbers to string
  // this might be large... but be safe... (unless we run out of ram)
  char convertBuffer[SLCUSTOM_CONVERT_BUF_SIZE]; 
  

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

      // maybe write one character from the buffer to the serial output this loop
      writeFromBuffer();
    }

    void pushToBuffer(char c){
      buffer.enqueue(c);
    }

    void pushToBuffer(const char* str){
      int len = strlen(str);
      for ( int i = 0; i < len; ++i ){
        pushToBuffer(str[i]);
      }
    }

    void pushToBuffer(uint16_t val){
      snprintf(convertBuffer, SLCUSTOM_CONVERT_BUF_SIZE, "%d", val);
      pushToBuffer(convertBuffer);
    }

    void pushToBuffer(uint8_t val){
      snprintf(convertBuffer, SLCUSTOM_CONVERT_BUF_SIZE, "%d", val);
      pushToBuffer(convertBuffer);
    }

    void pushToBuffer(int16_t val){
      snprintf(convertBuffer, SLCUSTOM_CONVERT_BUF_SIZE, "%d", val);
      pushToBuffer(convertBuffer);
    }

    void pushToBuffer(float val){
      dtostrf(val, 3, 2, convertBuffer);
      pushToBuffer(convertBuffer);
    }

    void pushToBuffer(uint32_t val){
      snprintf(convertBuffer, SLCUSTOM_CONVERT_BUF_SIZE, "%lu", val);
      pushToBuffer(convertBuffer);
    }

    void writeFromBuffer(){
      if ( !buffer.isEmpty() ){
        ser.print(buffer.dequeue());
      }
    }

    void tone(const uint16_t &duration, const uint16_t &frequency=0) {
      // Command: B {duration} {frequency}

      // runtime is 2+5+1+5+1 bytes is 144us@115200 baud?  check math. 
      // ser.print("B ");
      // ser.print(duration);
      // ser.print(' ');
      // ser.print(frequency);
      // ser.print('\n');
      pushToBuffer("B ");
      pushToBuffer(duration);
      pushToBuffer(' ');
      pushToBuffer(frequency);
      pushToBuffer('\n');
    }

    // S {hotEndTemp} {hotEndTargetTemp} {bedTemp} {bedTargetTemp} {isSDPrinting} {percentDone} {timeTaken} {filename}
    void printStatus(){
        // some helper functions in here could reduce code a few bytes...but not likely enough to matter.

        // S = Status
        pushToBuffer("S ");
        pushToBuffer(' ');
        
        // hot end temp
        pushToBuffer(thermalManager.degHotend(HOTEND_INDEX)); // end temp -- this will break if HOTENDS > 1
        pushToBuffer(' ');

        // hot end target temp
        pushToBuffer(thermalManager.degTargetHotend(HOTEND_INDEX)); // end target temp
        pushToBuffer(' ');

        // bed temp
        pushToBuffer(thermalManager.degBed());
        pushToBuffer(' ');

        // bed target temp
        pushToBuffer(thermalManager.degTargetBed());
        pushToBuffer(' ');

        // printing status
        if ( card.cardOK && card.sdprinting ){
          
          pushToBuffer('1');
          pushToBuffer(' ');

          // percent done
          pushToBuffer(card.percentDone());
          pushToBuffer(' ');

          duration_t elapsed = print_job_timer.duration();
          pushToBuffer(elapsed.value);

          card.printFilenameTo(convertBuffer, 100);
          pushToBuffer(convertBuffer);

        } else {
          ser.print("0 0 0 NA");
        }
        ser.print(' ');

        ser.print('\n');  // command ends with newline
    }

};


#endif
