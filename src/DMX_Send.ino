// - - - - -
// ESPDMX - A Arduino library for sending and receiving DMX using the builtin serial hardware port.
//
// Copyright (C) 2015  Rick <ricardogg95@gmail.com>
// This work is licensed under a GNU style license.
//
// Last change: Musti <https://github.com/IRNAS> (edited by Musti)
//
// Documentation and samples are available at https://github.com/Rickgg/ESP-Dmx
// Connect GPIO02 - TDX1 to MAX3485 or other driver chip to interface devices
// Pin is defined in library
// - - - - -

// General include
#include <vector>

// Special libraries
#include <ESPDMX.h>

// Split up source files
#include "Color.ino"
#include "Hue_Color.ino"
#include "Wheel_Color.ino"


DMXESPSerial dmx;

#include "Lamp_Multiple.ino"


// Global variables and consts

// std::vector<Lamp> lamps;

#define LAMPS_LENGTH 6
static Lamp lamps[LAMPS_LENGTH] = {Lamp({121, 1, 241}), Lamp({141, 21, 261}), Lamp({161, 41, 281}), Lamp({181, 61, 301}), Lamp({201, 81, 321}), Lamp({221, 101, 341})}; //I think this has to be hardcoded :(

static const size_t num_lamp_parts = 4 * LAMPS_LENGTH; //18;

size_t color_wheel_status = 0;

size_t status = 0;
size_t global_status = 0;
size_t waiting = 100;
size_t locked = 0;
size_t locked_at = 0;
static const size_t global_status_max = 5;
char incomingChar;
String incomingString = "";
//********************************************************************//
//                                                                    //
//-------------------------------Setup--------------------------------//
//                                                                    //
//********************************************************************//

void setup() {
    Serial.begin(115200);
    dmx.init(512);            // initialization for complete bus
    delay(200);               // wait a while (not necessary)


    /*
    lamps.reserve(18);
    lamps.push_back(Lamp(1)); // DMX Address space starts at one
    for (size_t i = 1; i < 18; ++i) {
        lamps.push_back(Lamp(20 * i));
    }
    */
    for (size_t i = 1; i < LAMPS_LENGTH; ++i) {
        lamps[i].init_parts();
    }
}


// Helper functions

void lamps_clear(Color c)
{
    for (size_t i = 0; i < LAMPS_LENGTH; ++i) {
        // Serial.print("Setting lamp ");
        // Serial.println(i);
        // Serial.print("Size of lamps: ");
        // Serial.println(LAMPS_LENGTH);
        lamps[i].set(c);
        // dmx.update(); /////////////////////////////////////temporarily
    }
}

void lamps_clear()
{
    lamps_clear(Color(0, 0, 0));
}

void set_lamp_part(size_t i, const Color& c)
{
    if (i >= num_lamp_parts)
        return;
    lamps[i / 4].set_part(i % 4, c);
}


// Lamp effect functions

void BGSnake() {

    Color c1 = Color(0, 255, 0);
    Color c2 = Color(0, 0, 255);

    for (size_t t = 0; t < 4 * 10; ++t) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t i = 0; i < num_lamp_parts; ++i) {
                lamps[i].set_part(0, j == 0 ? c1 : c2);
                lamps[i].set_part(1, j == 1 ? c1 : c2);
                lamps[i].set_part(2, j == 2 ? c1 : c2);
                lamps[i].set_part(3, j == 3 ? c1 : c2);
            }
            dmx.update();          // update the DMX bus
            delay(250);            // wait for 1s
        }
    }
}

void LoadingBar(Color c1, Color c2, bool direction_right, size_t *status) {
    lamps_clear(c2); // We want to start from a clean background
    
    for (size_t i = 0; i <= *status; ++i) {
        if(direction_right) {
            set_lamp_part(i, c1);
        }
        else {
            set_lamp_part(num_lamp_parts - i, c1); // Go reverse
        }
    }

    ++*status;
    if(*status >= num_lamp_parts) {
        *status = 0;
    }
}

size_t ColorWheelLoadingBar(size_t start_value, bool direction_right, size_t *status) {
    Color c1 = WheelColor(start_value, 255);
    for (size_t i = 0; i < *status; ++i) {
        c1 = WheelColor(start_value + i, 255);
        if(direction_right) {
            set_lamp_part(i, c1);
        }
        else {
            set_lamp_part(num_lamp_parts - i, c1); // Go reverse
        }
    }

    ++*status;
    if(*status >= num_lamp_parts) {
        *status = 0;
    }
    return (start_value + num_lamp_parts % 255);
}

void KnightRider(bool direction_right, size_t *status) {
    Color c1 = Color(255, 0, 0);
    Color c2 = Color(0, 0, 0);
    size_t tail_length = 4;
    size_t tail_decrease = 60;      
    size_t i = *status;
    lamps_clear(c2);
    if(direction_right) {
        set_lamp_part(i, c1);
    }
    else {
        set_lamp_part(num_lamp_parts - i, c1);
    }
    for (size_t j = 1; j <= tail_length; j++) {
        if(direction_right && i - j > 0) {
            set_lamp_part(i - j, software_dim(c1, 255 - i * tail_decrease ));
        }
        if (!direction_right && i + j <= num_lamp_parts) {
            set_lamp_part((num_lamp_parts - i) + j, software_dim(c1, 255 - i * tail_decrease ));
        }
    }
    ++*status;
    if(*status >= num_lamp_parts) {
        *status = 0;
    }
}

void Flashing(Color c1, size_t count, size_t *status) {
    if(*status % 2 == 0) {
        lamps_clear(Color(0, 0, 0));
    }
    else {
        lamps_clear(c1);
    }
    ++*status;
    if(*status >= num_lamp_parts) {
        *status = 0;
    }
}


void SparkleRGB() {

    lamps_clear();

    unsigned intensity = 255;

    size_t pix = 12;

    uint32_t seed = random(10000000);

    std::default_random_engine rng1(seed);
    std::default_random_engine rng2(seed);
    std::default_random_engine rng3(seed);

    for (size_t t = 0; t < 20 * 10; ++t) {
        for (size_t i = 0; i < pix; ++i) {
            set_lamp_part(rng1() % num_lamp_parts, WheelColor(rng3(), intensity));
        }
        dmx.update();
        delay(25);

        for (size_t i = 0; i < pix; ++i) {
            set_lamp_part(rng2() % num_lamp_parts, 0);
        }
        dmx.update();
        delay(25);

        if (pix < 18)
            ++pix;
    }
}

//********************************************************************//
//                                                                    //
//-----------------------------Main loop------------------------------//
//                                                                    //
//********************************************************************//

void loop() {
    for(size_t i = 0; i < waiting; i++) {
        if (Serial.available() > 0) {
            incomingChar = Serial.read();
            Serial.print(incomingChar);
            if(incomingChar == '\n') {
                status = 0;
                global_status = (size_t) incomingString.toInt();
                incomingString = "";
            }
            else if (isDigit(incomingChar)) {
                incomingString += incomingChar;
            }
            else if (incomingChar == 'l') {
                if(locked == 0) {
                    if(incomingString == "") {
                        locked = 1;
                    }
                    else {
                        locked = (size_t) incomingString.toInt();
                    }
                    locked_at = global_status;
                    Serial.println("\nLocked!");
                }
                else {
                    locked = 0;
                    Serial.println("\nUnlocked!");
                }
            }
            else if (incomingChar == '\b') {
                incomingString = "";
                Serial.println("Clear!");
            }
        }
        delay(1);
    }

    if(global_status == 0) {
        KnightRider(true, &status);
    }
    else if(global_status == 1) {
        KnightRider(false, &status);
    }
    else if(global_status == 2) {
        LoadingBar(Color(0, 255, 0), Color(0, 0, 255), true, &status);
    }
    else if(global_status == 3) {
        LoadingBar(Color(0, 0, 255), Color(0, 255, 0), false, &status);
    }
    else if(global_status == 4) {
        color_wheel_status = ColorWheelLoadingBar(color_wheel_status, true, &status);
    }
    else if(global_status == 5) {
        color_wheel_status = ColorWheelLoadingBar(color_wheel_status, false, &status);
    }
    else if(global_status == 6) {
        Flashing(Color(255, 255, 255), 10, &status);
    }

   // This is what we always need
   if(status == 0) {
       ++global_status;
       if(locked == 0) {
           if(global_status > global_status_max) {
               global_status = 0;
           }
       }
       else {
           if(global_status > locked_at + locked) {
               global_status = locked_at;
           }
       }
   }
   dmx.update();
}

