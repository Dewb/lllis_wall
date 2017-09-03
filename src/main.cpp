#include "FastLED.h"
#include "plasma.h"

#define NUM_LEDS_A 50
#define NUM_LEDS_B 50
#define NUM_LEDS (NUM_LEDS_A+NUM_LEDS_B)

#define DATA_PIN_A 2
#define DATA_PIN_B 3

CRGB ledBuffer[NUM_LEDS_A + NUM_LEDS_B];

#define WALL_X 4
#define WALL_Y 12



class Cell
{
public:

   typedef enum {
      Inactive,
      Loading,
      Nominal,
      Overload,
      Error
   } State;

   State state;

   char count;
   bool switched;

   Cell()
   {
      state = random8() > 128 ? State::Loading : State::Inactive;
      count = random8(0, 32);
      switched = false;
   }

   void setState(State newState, char newCount)
   {
      state = newState;
      count = newCount;
      switched = true;
   }

   void tick()
   {
      switched = false;

      switch(state)
      {
         case Inactive:
         {
            if (count-- <= 0) { setState(State::Loading, random8(8, 14)); }
         }
         break;

         case Loading:
         {
            if (count-- <= 0) { setState(State::Nominal, random8(12, 36)); }
         }
         break;

         case Nominal:
         {
            if (random8() < 3)     { setState(State::Error, random8(4, 64)); }
            else if (count-- <= 0) { setState(State::Overload, random8(16, 36)); }
         }
         break;

         case Overload:
         {
            if (random8() < 30) { setState(State::Nominal, random8(4, 24)); }
            if (count-- <= 0)   { setState(State::Error, random8(8, 36)); }
         }
         break;

         case Error:
         {
            if (random8() < 8)       { switched = true; state = State::Nominal;  count = random8(16, 36); }
            else if (random8() < 20) { switched = true; state = State::Overload; count = random8(16, 36); }
            else if (count-- <= 0)   { switched = true; state = State::Inactive; count = random8(36,64); }
         }
         break;

         default:
         {
            state = State::Error;
         }
      }

   }

   CRGB blink(CRGB color1, CRGB color2, unsigned long clock, int duty1, int duty2)
   {
      if (fmod(clock, duty1 + duty2) < duty1)
      {
         return color1;
      }
      else
      {
         return color2;
      }
   }

   CRGB getColor(unsigned long clock)
   {
      switch(state)
      {
         case Inactive:
         {
            return blink(CRGB::Goldenrod, CRGB::Black, clock, 200, 1800);
         }
         break;

         case Loading:
         {
            return blink(CRGB::Goldenrod, CRGB::Black, clock, 500, 500);
         }
         break;

         case Nominal:
         {
            return CRGB::Goldenrod;
         }
         break;

         case Overload:
         {
            return blink(CRGB::Red, CRGB::Goldenrod, clock, 400, 640);
         }
         break;

         case Error:
         {
            return blink(CRGB::Red, CRGB::Black, clock, 1200, 800);
         }
         break;
      }

      return CRGB::Black;
   }
};

Cell wall[WALL_X * WALL_Y];

void setup() {

  	  FastLED.addLeds<WS2811, DATA_PIN_A, RGB>(ledBuffer, NUM_LEDS_A);
     FastLED.addLeds<WS2811, DATA_PIN_B, RGB>(ledBuffer + NUM_LEDS_A, NUM_LEDS_B);

}

void arrange(unsigned long clock)
{
   int i, x, y;
   i = 2; // skip pixels at start of strand A
   for (y = (WALL_Y / 2) - 1; y >= 0; y--)
   {
      if (y % 2 == 0)
      {
         for(x = 0; x < WALL_X; x++)
         {
            CRGB c = wall[WALL_X * y + x].getColor(clock);
            ledBuffer[i++] = c;
            ledBuffer[i++] = c;
         }
      } else {
         for(x = WALL_X - 1; x >= 0; x--)
         {
            CRGB c = wall[WALL_X * y + x].getColor(clock);
            ledBuffer[i++] = c;
            ledBuffer[i++] = c;
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
            CRGB c = wall[WALL_X * y + x].getColor(clock);
            ledBuffer[i++] = c;
            ledBuffer[i++] = c;
         }
      } else {
         for(x = WALL_X - 1; x >= 0; x--)
         {
            CRGB c = wall[WALL_X * y + x].getColor(clock);
            ledBuffer[i++] = c;
            ledBuffer[i++] = c;
         }
      }
   }

   FastLED.show();
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

unsigned long lastFrame = 0;
unsigned long frameLength = 2000;

void oldloop() {

   int x, y;
   unsigned long clock = millis();

   if (clock - lastFrame > frameLength) {
      lastFrame = clock;

      for(x = 0; x < WALL_X; x++)
      {
         for(y = 0; y < WALL_Y; y++)
         {
            wall[WALL_X * y + x].tick();
         }
      }

      for(y = 0; y < WALL_Y; y++)
      {
         uint8_t r = random8(0, 128 );
         if (r < 3)
         {
            wall[WALL_X * y + r + 1] = wall[WALL_X * y + r];
         }
         else if (r < 8 && y > 0)
         {
            for (x = 0; x < WALL_X; x++)
            {
               wall[WALL_X * y + x] = wall[WALL_X * (y - 1) + x];
            }
         }
      }

   }

   //testblink();

   arrange(clock);
}


void loop()
{
  unsigned long frameCount=25500;  // arbitrary seed to calculate the three time displacement variables t,t2,t3
  while(1) {

    for (uint8_t y = 0; y < NUM_LEDS; y++) {
        uint8_t x = 0;
        //int color = plasma_rgb(frameCount, x, y);

        uint8_t p = plasma(frameCount, x, y);

        uint8_t d = abs(p - 128);
        uint8_t s = lerp8by8(0, 255, p);
        uint8_t l = lerp8by8(0.8 * 255, 255, p);

        uint8_t smix = lerp8by8(255, 179, d);
        uint8_t lmix = lerp8by8(255, 101, d);

        uint16_t index = y;

        CRGB color(0);
        color.setHSV(lerp8by8(0.7 * 255, 1.0 * 255, p), smix, lmix);

        // channel debugging
        //color.setHSV(y * 30, 255, 255);

        ledBuffer[index] = color; // color.r << 16 | color.g << 8 | color.b);

    }
    frameCount++;

    FastLED.show();
  }
}
