#include "FastLED.h"

#define NUM_LEDS_A 50
#define NUM_LEDS_B 50
#define NUM_LEDS (NUM_LEDS_A+NUM_LEDS_B)

#define DATA_PIN_A 2
#define DATA_PIN_B 3

CRGB ledBuffer[NUM_LEDS_A + NUM_LEDS_B];

#define WALL_X 4
#define WALL_Y 12

CRGB wall[WALL_X * WALL_Y];

void setup() {

  	  FastLED.addLeds<WS2811, DATA_PIN_A, RGB>(ledBuffer, NUM_LEDS_A);
     FastLED.addLeds<WS2811, DATA_PIN_B, RGB>(ledBuffer + NUM_LEDS_A, NUM_LEDS_B);

}

void arrange()
{
   int i, x, y;
   i = 2; // skip pixels at start of strand A
   for (y = (WALL_Y / 2) - 1; y >= 0; y--)
   {
      if (y % 2 == 0)
      {
         for(x = 0; x < WALL_X; x++)
         {
            ledBuffer[i++] = wall[WALL_X * y + x];
            ledBuffer[i++] = wall[WALL_X * y + x];
         }
      } else {
         for(x = WALL_X - 1; x >= 0; x--)
         {
            ledBuffer[i++] = wall[WALL_X * y + x];
            ledBuffer[i++] = wall[WALL_X * y + x];
         }
      }
   }
   i += 2; // skip pixels at start of strand B
   for (y = (WALL_Y / 2); y < WALL_Y; y++)
   {
      if (y % 2 == 1)
      {
         for(x = 0; x < WALL_X; x++)
         {
            ledBuffer[i++] = wall[WALL_X * y + x];
            ledBuffer[i++] = wall[WALL_X * y + x];
         }
      } else {
         for(x = WALL_X - 1; x >= 0; x--)
         {
            ledBuffer[i++] = wall[WALL_X * y + x];
            ledBuffer[i++] = wall[WALL_X * y + x];
         }
      }
   }
}

void testblink() {
   for (int i=0; i<NUM_LEDS;i++) {
      ledBuffer[i] = CRGB::Red;
   }

   FastLED.show();
   delay(500);

   for (int i=0; i<NUM_LEDS;i++) {
      ledBuffer[i] = CRGB::Black;
   }

   FastLED.show();
   delay(500);

   for (int i=0; i<NUM_LEDS;i++) {
      ledBuffer[i] = CRGB::White;
   }

   FastLED.show();
   delay(500);

   for (int i=0; i<NUM_LEDS;i++) {
      ledBuffer[i] = CRGB::Black;
   }

   FastLED.show();
   delay(500);
}

double time = 0;

void loop() {

   time += 0.1;

   for(int x = 0; x < WALL_X; x++)
   {
      for(int y = 0; y < WALL_Y; y++)
      {
         wall[WALL_X * y + x] =
            CRGB(
               int(x/(WALL_X * 1.0) * 255),
               (fmod(time, 24) / 24.0) * 255,
               int(y/(WALL_Y * 1.0) * 255)
            );
      }
   }
   testblink();
   //arrange();
   //FastLED.show();
}
