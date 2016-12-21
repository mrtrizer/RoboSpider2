#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#define SERVO_CENTER PB0
#define SERVO_RIGHT PB1
#define SERVO_LEFT PB2
#define ECHO PB3
#define LED PB4
#define TRIG PB5

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
       | (0 << ECHO)
       | (1 << LED)
       | (1 << TRIG);

  TCCR0A |= (1 << WGM01); // Configure timer 1 for CTC mode
  TCCR0B |= (1 << CS00); // No prescaler
  TIMSK0 |= (1 << OCIE0A); // Enable CTC interrupt
  OCR0A = 95; // Set CTC compare value

  sei();
}

main()
{
  init();

  uint8_t curPos = 0;

  while(1)
  {
    g_point = g_points[curPos];

    _delay_loop_2(g_point.delay * 20);

    curPos++;
    if (curPos >= (sizeof(g_points) / sizeof(struct Point)))
      curPos = 0;
  }
}

ISR(TIM0_COMPA_vect)	// 100 KHz interrupt frequency
{
  if(g_servoCounter >= 500) {
    g_servoCounter = 0;
    PORTB |= (1 << SERVO_CENTER) | (1 << SERVO_RIGHT) | (1 << SERVO_LEFT);
  }
  if (g_servoCounter >= g_point.center)
    PORTB &= ~(1 << SERVO_CENTER);
  if (g_servoCounter >= g_point.right)
    PORTB &= ~(1 << SERVO_RIGHT);
  if (g_servoCounter >= g_point.left)
    PORTB &= ~(1 << SERVO_LEFT);
  g_servoCounter++;
}
