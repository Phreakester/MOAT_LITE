/* 
MOAT - Model 2O Awesome Teensy
mk. V.0
General code to oversee all functions of the Teensy
*/

#include <Arduino.h>

//Libraries
#include <SPI.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <string>
#include <ArduinoLog.h>
#include <SD.h>

//Classes
#include <Actuator.h>
#include <Radio.h>

//General Settings
  //Mode
  /*
  Operating 0 - For normal operation, initializes then runs main control fn in loop
    May disable logging object in its library config to free up memory, if relevant
  Diagnostic 1 - Runs diagnostic test 100 times, then stops, saves to sd
  */
  #define MODE 1

  //Startup
  #define WAIT_SERIAL_STARTUP 0
  #define RUN_DIAGNOSTIC_STARTUP 0

  //Log
  #define LOG_LEVEL LOG_LEVEL_VERBOSE
  #define SAVE_THRESHOLD 100 //Sets how often the log object will save to sd
  //Note: By default the log requires and outputs to the SD card, and can be changed in setup

  //Actuator
  #define HOME_ON_STARTUP 0

//<--><--><--><-->< Base Systems ><--><--><--><--><-->

//ODrive Settings
  #define odrive_starting_timeout 1000 //NOTE: In ms

  //PINS

  //CREATE OBJECT
  // ODrive odrive(Serial1);

//LOGGING AND SD SETTINGS
  //Create file to log to
  File logFile;
  //Cannot create logging object until init

//<--><--><--><-->< Sub-Systems ><--><--><--><--><-->
//ACTUATOR SETTINGS
  //PINS TEST BED
  #define PRINTTOSERIAL false
  // #define enc_A 20
  // #define enc_B 21
  // #define hall_inbound 12
  // #define hall_outbound 11
  // #define gearTooth_engine 15 //rn just attached to encoder B haha

  //PINS CAR
  #define enc_A 2
  #define enc_B 3
  #define hall_inbound 22
  #define hall_outbound 23
  #define gearTooth_engine 40
  #define gearTooth_gearbox 41


  //CREATE OBJECT

  static void external_count_egTooth();

  Actuator actuator(Serial1, enc_A, enc_B, gearTooth_engine, 0, hall_inbound, hall_outbound, &external_count_egTooth, PRINTTOSERIAL);

  static void external_count_egTooth(){
    actuator.count_egTooth();
  }
  
//FREE FUNCTIONS

void save_log() {
  //Closes and then opens the file stream
  logFile.close();
  logFile = SD.open("log.txt", FILE_WRITE);
}

void setup() {
  //-------------Wait for serial-----------------
  if(WAIT_SERIAL_STARTUP) {
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }
  //-------------Logging and SD Card-----------------
  SD.begin(BUILTIN_SDCARD);
  logFile = SD.open("log.txt", FILE_WRITE);

  Log.begin(LOG_LEVEL_VERBOSE, &logFile, false);
  Log.verbose("Initialization Started" CR);
  Log.verbose("Time: %d" CR, millis());

  save_log();

  //------------------Odrive------------------

  //At this time the following code is depricated, but until we make final decisions about the odrive class, we will leave it in

  // int odrive_init = actuator.odrive.init(1000);
  // if (odrive_init) {
  //   Log.error("ODrive Init Failed code: %d" CR, odrive_init);
  // }
  // else {
  //   Log.verbose("ODrive Init Success code: %d" CR, odrive_init);
  // }
  // Serial.println("ODrive init: " + String(odrive_init));
  // for (int i = 0; i < 20; i++) {
  //   Serial.println(odrive.get_voltage());
  // }
  // logFile.close();
  // logFile = SD.open("log.txt", FILE_WRITE);

  //-------------Actuator-----------------
  //General Init
  int o_actuator_init = actuator.init(odrive_starting_timeout);
  if(o_actuator_init) {
    Log.error("Actuator Init Failed code: %d" CR, o_actuator_init);
  }
  else {
    Log.verbose("Actuator Init Success code: %d" CR, o_actuator_init);
  }

  //Homing if enabled
  if (HOME_ON_STARTUP) {
    int* o_homing = actuator.homing_sequence();
    if (o_homing[0]) {
      Log.error("Homing Failed code: %d" CR, o_homing);
    }
    else {
      Log.verbose("Homing Success code: %d" CR, o_homing);
      Log.notice("Homing results, inbound: %d, outbound: %d" CR, o_homing[1], o_homing[2]);
    }
  }
  Log.verbose("Initialization Complete" CR);
  Log.notice("Starting mode %d" CR, MODE);
  save_log();
}

//OPERATING MODE
#if MODE == 0

int* o_control;
int save_count = 0;
void loop() {
  //Main control loop, with actuator
  o_control = actuator.control_function();
  //<status, rpm, actuator_velocity, inbound_triggered, outbound_triggered, time_started, time_finished>
  Log.notice("Status: %d  RPM: %d, Act Vel: %d, Inb Trig: %d, Otb Trig: %d, Start: %d, End: %d" CR,
    o_control[0], o_control[1], o_control[2], o_control[3], o_control[4], o_control[5], o_control[6]);
  
  //Save data to sd every SAVE_THRESHOLD
  if (save_count > SAVE_THRESHOLD) {
    int save_start = millis();
    save_log();
    save_count = 0;
    Log.verbose("Saved log in %d ms" CR, millis() - save_start);
  }
  save_count++;
}

//DIAGNOSTIC MODE
#elif MODE == 1

void loop() {
  Log.notice("DIAGNOSTIC MODE" CR);
  Log.verbose("Running diagnostic function" CR);

  for (int i = 0; i < 100; i++) {
    //Serial.println(actuator.diagnostic(false));
    Log.notice("%d", i);
    Log.notice((actuator.diagnostic(true)).c_str());
    delay(100);
  }

  Log.verbose("End of diagnostic function" CR);
  logFile.close();
  exit(0);
}

#endif


/*
//"When you join the MechE club thinking you could escape the annoying CS stuff like pointers and interrupts"
//                             __...------------._
//                          ,-'                   `-.
//                       ,-'                         `.
//                     ,'                            ,-`.
//                    ;                              `-' `.
//                   ;                                 .-. \
//                  ;                           .-.    `-'  \
//                 ;                            `-'          \
//                ;                                          `.
//                ;                                           :
//               ;                                            |
//              ;                                             ;
//             ;                            ___              ;
//            ;                        ,-;-','.`.__          |
//        _..;                      ,-' ;`,'.`,'.--`.        |
//       ///;           ,-'   `. ,-'   ;` ;`,','_.--=:      /
//      |'':          ,'        :     ;` ;,;,,-'_.-._`.   ,'
//      '  :         ;_.-.      `.    :' ;;;'.ee.    \|  /
//       \.'    _..-'/8o. `.     :    :! ' ':8888)   || /
//        ||`-''    \\88o\ :     :    :! :  :`""'    ;;/
//        ||         \"88o\;     `.    \ `. `.      ;,'
//        /)   ___    `."'/(--.._ `.    `.`.  `-..-' ;--.
//        \(.="""""==.. `'-'     `.|      `-`-..__.-' `. `.
//         |          `"==.__      )                    )  ;
//         |   ||           `"=== '                   .'  .'
//         /\,,||||  | |           \                .'   .'
//         | |||'|' |'|'           \|             .'   _.' \
//         | |\' |  |           || ||           .'    .'    \
//         ' | \ ' |'  .   ``-- `| ||         .'    .'       \
//           '  |  ' |  .    ``-.._ |  ;    .'    .'          `.
//        _.--,;`.       .  --  ...._,'   .'    .'              `.__
//      ,'  ,';   `.     .   --..__..--'.'    .'                __/_\
//    ,'   ; ;     |    .   --..__.._.'     .'                ,'     `.
//   /    ; :     ;     .    -.. _.'     _.'                 /         `
//  /     :  `-._ |    .    _.--'     _.'                   |
// /       `.    `--....--''       _.'                      |
//           `._              _..-'                         |
//              `-..____...-''                              |
//                                                          |
//                                mGk                       |
*/

