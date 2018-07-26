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

Lamp lamps[6] = {Lamp({121, 1, 241}), Lamp({141, 21, 261}), Lamp({161, 41, 281}), Lamp({181, 61, 301}), Lamp({201, 81, 321}), Lamp({221, 101, 341})}; //I think this has to be hardcoded :(

static const size_t num_lamp_parts = 4 * 6; //18;

bool flash = false;
size_t color_wheel_status = 0;

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
}


// Helper functions

void lamps_clear()
{
    for (size_t i = 0; i < sizeof(lamps); ++i) {
        lamps[i].set(0);
    }
}

void lamps_clear_color(Color c)
{
    for (size_t i = 0; i < sizeof(lamps); ++i) {
        lamps[i].set(c);
    }
}

void set_lamp_part(size_t i, const Color& c)
{
    if (i >= 18 * 4)
        return;
    lamps[i / 4].set_part(i % 4, c);
}

void enable_flash(size_t speed) {
    for (size_t i = 0; i < sizeof(lamps); ++i) {
        lamps[i].flash(speed);
    }
}

void disable_flash() {
    for (size_t i = 0; i < sizeof(lamps); ++i) {
        lamps[i].flash(0);
    }
}

// Lamp effect functions

void BGSnake() {

    Color c1 = Color(0, 255, 0);
    Color c2 = Color(0, 0, 255);

    for (size_t t = 0; t < 4 * 10; ++t) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t i = 0; i < 18; ++i) {
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

void LoadingBar(Color c1, Color c2, bool direction_right, size_t duration) {
    lamps_clear_color(c2); // We want to start from a clean background
    for (size_t i = 0; i < num_lamp_parts; ++i) {
        delay(duration/num_lamp_parts); // We want to iterate over every known lamp.
        if(direction_right) {
            set_lamp_part(i, c1);
        }
        else {
            set_lamp_part(num_lamp_parts - i, c1); // Go reverse
        }
        dmx.update();                         // update the DMX bus
    }
}

size_t ColorWheelLoadingBar(size_t start_value, bool direction_right, size_t duration) {
    Color c1 = WheelColor(start_value, 255);
    for (size_t i = 0; i < num_lamp_parts; ++i) {
        c1 = WheelColor(start_value + i, 255);
        delay(duration/num_lamp_parts); // We want to iterate over every known lamp.
        if(direction_right) {
            set_lamp_part(i, c1);
        }
        else {
            set_lamp_part(num_lamp_parts - i, c1); // Go reverse
        }
        dmx.update();                         // update the DMX bus
    }
    return (start_value + num_lamp_parts % 255);
}

void ToggleFlash(size_t speed) {
    flash = !flash;
    for (size_t i = 0; i < sizeof(lamps); ++i) {
        if(flash) {
            enable_flash(speed);
        }
        else {
            disable_flash();
        }
    }
    dmx.update();
}

/*
void KnightRider() {

    Color c1 = Color(255, 0, 0);
    Color c2 = Color(0, 0, 255);
    bool direction_right = true;
    
    for (size_t j = 0; j < 10; ++j) {
        lamps_clear_color(c2);

        for (size_t i = 0; i < num_lamp_parts; ++i) {
            if(direction_right) {
                set_lamp_part(i, c1);
            }
            else {
                set_lamp_part(num_lamp_parts - i, c1);
            }
            dmx.update();          // update the DMX bus
            delay(250);            // wait for 0.25s
            Serial.println("Switched Light");
        }
        direction_right = !direction_right;
    }
}
*/

void SparkleRGB() {

    lamps_clear();

    unsigned intensity = 255;

    size_t pix = 4;

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
    // Serial.println("BGSnake");
    // BGSnake();

    // Serial.println("SparkleRGB");
    // SparkleRGB();
    /*
    Serial.println("Loading Bar 1");
    LoadingBar(Color(0, 0, 255), Color(255, 0, 0), true, 1000);
    Serial.println("Loading Bar 2");
    LoadingBar(Color(0, 255, 0), Color(0, 0, 255), false, 1000);
    Serial.println("Loading Bar 3");
    LoadingBar(Color(255, 0, 0), Color(0, 255, 0), true, 1000);
    Serial.println("Loading Bar 4");
    Serial.println("Restarting");
    */
    color_wheel_status = ColorWheelLoadingBar(color_wheel_status, true, 1000);
    color_wheel_status = ColorWheelLoadingBar(color_wheel_status, false, 1000);
}
