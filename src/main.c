#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#define SERVO_CENTER PC3
#define SERVO_RIGHT PC4
#define SERVO_LEFT PC5
#define LED PB6

volatile uint16_t g_servoCounter = 0;
volatile uint16_t g_distCounter = 0;
volatile uint16_t g_curDist = 0;

struct Point {
  uint8_t center;
  uint8_t right;
  uint8_t left;
  uint8_t delay;
};

struct Point g_points[] = {
  {14, 15, 13, 75},
  {18, 20, 18, 75},
};

struct Point g_point;

void init() {
  DDRB = (1 << SERVO_CENTER)
       | (1 << SERVO_RIGHT)
       | (1 << SERVO_LEFT)
       | (1 << LED);

  sei();
}

main()
{
  init();

  uint8_t curPos = 0;

  PORTB |= (1 << LED);

  while(1)
  {
    g_point = g_points[curPos];

    _delay_loop_2(g_point.delay * 20);

    curPos++;
    if (curPos >= (sizeof(g_points) / sizeof(struct Point)))
      curPos = 0;
  }
}

