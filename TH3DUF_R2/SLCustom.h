#ifndef _SLCUSTOM_H_
#define _SLCUSTOM_H_

#include <SLSoftwareSerial.h>
#include "temperature.h"
#include "cardreader.h"
#include "SLCircularQueue.h"

#define SLCUSTOM_MSG_HELLO "Hello\n"
#define SLCUSTOM_BAUD_RATE 115200
#define SLCUSTOM_RUN_EVERY_MILLIS 10000 // print status every 10 seconds
#define SLCUSTOM_BUF_SIZE 512
#define SLCUSTOM_CONVERT_BUF_SIZE 16 // max characters for a number

// ONE WAY SERIAL - TX ONLY ON FREE PIN (requires soldering to unused header next to capacitor near center of board)
#define TX_PIN 29

class SLCustom {

  private:
  SLSoftwareSerial ser;
  unsigned long last_run_millis = 0;
  unsigned long now = 0;

  // output buffer, we only want to write 1 char per loop so we don't steal too much time from the main loop
  SLCircularQueue<char,1024> buffer;

  // use to convert numbers to string
  // this might be large... but be safe... (unless we run out of ram)
  char convertBuffer[SLCUSTOM_CONVERT_BUF_SIZE]; 
  

  public:
    SLCustom() : ser(TX_PIN) {
    }

    void setup(){
      ser.begin(SLCUSTOM_BAUD_RATE);
    }

    void loop(){
      now = millis();
      if ( now - last_run_millis > SLCUSTOM_RUN_EVERY_MILLIS ){
        printStatus();
        last_run_millis = now;
      }

      // maybe write one character from the buffer to the serial output this loop
      writeFromBuffer();
    }

    void m118(const char* string_arg){
      pushToBuffer("M ");
      pushToBuffer(string_arg);
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
