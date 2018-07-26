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

//General include
#include <vector>

//Special libraries
#include <ESPDMX.h>

//Split up source files
#include "Color.ino"
#include "Hue_Color.ino"


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
static inline Color WheelColor(uint32_t i, uint8_t intensity) {
    if (intensity == 0)
        return Color(0);
    i &= 255;
    if (i < 85) {
        return Color(
            (256 - i * 3) * 255u / intensity, 0, (i * 3) * 255 / intensity);
    }
    if (i < 170) {
        i -= 85;
        return Color(
            0, (i * 3) * 255u / intensity, (256 - i * 3) * 255u / intensity);
    }
    i -= 170;
    return Color(
        (i * 3) * 255u / intensity, (256 - i * 3) * 255u / intensity, 0);
}

DMXESPSerial dmx;

class Lamp
{
public:
    Lamp(size_t addr)
        : addr_(addr)
    {
        dmx.write(addr_ + 0, 11);
        dmx.write(addr_ + 1, 255);
        dmx.write(addr_ + 2, 0);
    }

    void dimm(size_t v) {
        dmx.write(addr_ + 1, v);
    }

    void flash(size_t v) {

    }

    // all colors
    void set(size_t v) {
        for (size_t i = 0; i < 16; ++i) {
            dmx.write(addr_ + 3 + i, v);
        }
    }

    // RGBA colors
    void set(size_t r, size_t g, size_t b, size_t a) {
        for (size_t i = 0; i < 4; ++i) {
            dmx.write(addr_ + 3 + 4 * i + 0, r);
            dmx.write(addr_ + 3 + 4 * i + 1, g);
            dmx.write(addr_ + 3 + 4 * i + 2, b);
            dmx.write(addr_ + 3 + 4 * i + 3, a);
        }
    }

    // RGBA colors
    void set(const Color& c) {
        set(c.r, c.g, c.b, c.w);
    }

    // RGBA colors
    void set_part(size_t p, size_t r, size_t g, size_t b, size_t a) {
        dmx.write(addr_ + 3 + 4 * p + 0, r);
        dmx.write(addr_ + 3 + 4 * p + 1, g);
        dmx.write(addr_ + 3 + 4 * p + 2, b);
        dmx.write(addr_ + 3 + 4 * p + 3, a);
    }

    // RGBA colors
    void set_part(size_t p, const Color& c) {
        set_part(p, c.r, c.g, c.b, c.w);
    }

private:
    size_t addr_;
};

std::vector<Lamp> lamps;

static const size_t num_lamps = 4 * 18;

void lamps_clear()
{
    for (size_t i = 0; i < lamps.size(); ++i) {
        lamps[i].set(0);
    }
}

void lamps_clear_color(Color c)
{
    for (size_t i = 0; i < lamps.size(); ++i) {
        lamps[i].set(c);
    }
}

void set_lamp(size_t i, const Color& c)
{
    if (i >= 18 * 4)
        return;
    lamps[i / 4].set_part(i % 4, c);
}

void setup() {
    Serial.begin(115200);
    dmx.init(512);           // initialization for complete bus
    delay(200);               // wait a while (not necessary)

    lamps.reserve(18);
    lamps.push_back(Lamp(1));
    for (size_t i = 1; i < 18; ++i) {
        lamps.push_back(Lamp(20 * i));
    }
}

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
    lamps_clear_color(c2);
    for (size_t i = 0; i < num_lamps; ++i) {
        Serial.print("Waiting ");
        Serial.print(duration/num_lamps);
        Serial.print(" milliseconds at iteration ");
        Serial.print(i);
        Serial.println("...");
        delay(duration/num_lamps);
        if(direction_right) {
            set_lamp(i, c1);
        }
        else {
            set_lamp(num_lamps - i, c1);
        }
        dmx.update();                         // update the DMX bus
    }
}

/*
void KnightRider() {

    Color c1 = Color(255, 0, 0);
    Color c2 = Color(0, 0, 255);
    bool direction_right = true;
    
    for (size_t j = 0; j < 10; ++j) {
        lamps_clear_color(c2);

        for (size_t i = 0; i < num_lamps; ++i) {
            if(direction_right) {
                set_lamp(i, c1);
            }
            else {
                set_lamp(num_lamps - i, c1);
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
            set_lamp(rng1() % num_lamps, WheelColor(rng3(), intensity));
        }
        dmx.update();
        delay(25);

        for (size_t i = 0; i < pix; ++i) {
            set_lamp(rng2() % num_lamps, 0);
        }
        dmx.update();
        delay(25);

        if (pix < 18)
            ++pix;
    }
}

void loop() {
    // Serial.println("BGSnake");
    // BGSnake();

    // Serial.println("SparkleRGB");
    // SparkleRGB();
    
    Serial.println("Loading Bar 1");
    LoadingBar(Color(0, 0, 255), Color(0, 0, 0), true, 5000);
    Serial.println("Loading Bar 2");
    LoadingBar(Color(0, 255, 0), Color(0, 0, 255), false, 5000);
    Serial.println("Loading Bar 3");
    LoadingBar(Color(255, 0, 0), Color(0, 255, 0), true, 5000);
    Serial.println("Restarting");
}
