#include <Arduino.h>
#include <oled.h>

// There are 2 different versions of the board
// OLED display=OLED(2,14,4); 
// OLED display=OLED(4,5,16);
// OLED display=OLED(4,5,16,0x3C,128,32,false);
OLED display=OLED(4,5,16,0x3C,128,64,true);
String first, second;

void setup()
{
    Serial.begin(115200);
    display.begin();
    delay(1000);  
}

void loop()
{
    // Draw hollow circles
    // for (uint_least8_t radius=3; radius<62; radius+=3)
    for (uint_least8_t radius=3; radius<62; radius+=3)
    {
        delay(20);
        display.draw_circle(64,32,radius);
        if (radius>15)
        {
            display.draw_circle(64,32,radius-15,OLED::SOLID,OLED::BLACK);
        }
        display.display();
    }
    delay(1000);
    display.clear();
    display.draw_string_P(0,0,PSTR("SDC"),OLED::DOUBLE_SIZE);
    display.display();
    delay(500);
    display.draw_string_P(32,24,PSTR("TECH"),OLED::DOUBLE_SIZE);
    display.display();
    delay(500);
    display.draw_string_P(72,50,PSTR("TALK"),OLED::DOUBLE_SIZE);
    display.display();

    delay(1000);
    for (int i=0; i<6; i++)
    {
        display.set_invert(true);
        delay(200);
        display.set_invert(false);
        delay(200);
    }

    display.set_scrolling(OLED::HORIZONTAL_RIGHT);
    delay(6000);
    display.set_scrolling(OLED::HORIZONTAL_LEFT);
    delay(6000);
    display.set_scrolling(OLED::DIAGONAL_RIGHT);
    delay(6000);
    display.set_scrolling(OLED::DIAGONAL_LEFT);
    delay(6000);
    display.set_scrolling(OLED::NO_SCROLLING);
    display.display();
    delay(2000);
    display.clear();
}

