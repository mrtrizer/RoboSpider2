#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#define SERVO_CENTER PC3
#define SERVO_RIGHT PC4
#define SERVO_LEFT PC5
#define SERVO_PORT PORTC
#define SERVO_DDR DDRC

#define LED PD5
#define LED_PORT PORTD
#define LED_DDR DDRD

volatile uint16_t g_servoCounter = 0;


struct Point {
    uint8_t center;
    uint8_t right;
    uint8_t left;
    uint8_t delay;
};

struct Point g_pointsStop[] = {
    {38, 40, 42, 20},
};

struct Point g_pointsFwd[] = {
//   c   r   l   d
    {33, 50, 51, 5},
    {33, 30, 33, 10},
    {43, 30, 33, 5},
    {43, 50, 51, 10},
};

struct Point g_pointsFwdLeft[] = {
//   c   r   l   d
    {33, 50, 51, 5},
    {33, 30, 33, 10},
    {43, 30, 33, 5},
    {43, 50, 51, 10},
};

struct Point g_pointsFwdRight[] = {
//   c   r   l   d
    {33, 50, 51, 5},
    {33, 30, 33, 10},
    {43, 30, 33, 5},
    {43, 50, 51, 10},
};

struct Point g_pointsBack[] = {
    {33, 30, 33, 5},
    {33, 50, 51, 10},
    {43, 50, 51, 5},
    {43, 30, 33, 10},
};

struct Point g_pointsLeft[] = {
	{35, 40 - 10, 32 + 10, 10},
	{35, 30 - 10, 42 + 10, 15},
	{45, 30 - 10, 42 + 10, 10},
	{45, 40 - 10, 32 + 10, 15},
};

struct Point g_pointsRight[] = {
	{35, 30 - 10, 42 + 10, 10},
	{35, 40 - 10, 32 + 10, 15},
	{45, 40 - 10, 32 + 10, 10},
	{45, 30 - 10, 42 + 10, 15},
};

struct Point g_point;
struct Point* g_points;
int g_pointsCount = 0;
uint8_t g_curPos = 0;
uint8_t g_active = 0;

void initUSART() {
    UCSRC = ( 1 << URSEL )
        | ( 0 << UCSZ2 )
        | ( 1 << UCSZ1 )
        | ( 1 << UCSZ0 )
        | ( 0 << UPM1 )
        | ( 0 << UPM0 )
        | ( 0 << USBS );

    UCSRB = (1<<RXEN)|(1<<TXEN);
    UBRRH=0x00;
    UBRRL=51;
}

void initLED() {
    LED_DDR |= (1 << LED);
}

void init() {

    initLED();
    initUSART();

    SERVO_DDR = (1 << SERVO_CENTER)
       | (1 << SERVO_RIGHT)
       | (1 << SERVO_LEFT);

    TCCR0 = (1 << CS00); // No prescaler
    TIMSK = (1 << TOIE0); // Enable interrupt

    sei();
}

void handleInput() {
		char rc;

    if ((UCSRA & (1<<7)))
    {
        rc = UDR;
        while ( !( UCSRA & (1<<5)) ) {}
        UDR = rc;
    }

    switch (rc)
    {
        case 's':
            g_curPos = 0;
            g_points = g_pointsStop;
            g_pointsCount = sizeof(g_pointsStop) / sizeof(struct Point);
        break;
        case 'f':
            g_curPos = 0;
            g_points = g_pointsFwd;
            g_pointsCount = sizeof(g_pointsFwd) / sizeof(struct Point);
        break;
        case '1':
            g_curPos = 0;
            g_points = g_pointsFwdLeft;
            g_pointsCount = sizeof(g_pointsFwdLeft) / sizeof(struct Point);
        break;
        case '2':
            g_curPos = 0;
            g_points = g_pointsFwdRight;
            g_pointsCount = sizeof(g_pointsFwdRight) / sizeof(struct Point);
        break;
        case 'b':
            g_curPos = 0;
            g_points = g_pointsBack;
            g_pointsCount = sizeof(g_pointsBack) / sizeof(struct Point);
        break;
        case 'l':
            g_curPos = 0;
            g_points = g_pointsLeft;
            g_pointsCount = sizeof(g_pointsLeft) / sizeof(struct Point);
        break;
        case 'r':
            g_curPos = 0;
            g_points = g_pointsRight;
            g_pointsCount = sizeof(g_pointsRight) / sizeof(struct Point);
        break;
    }
}

uint8_t pointsEqual(struct Point* lastPoint, struct Point* nextPoint) {
    return lastPoint->center == nextPoint->center
        && lastPoint->right == nextPoint->right
        && lastPoint->left == nextPoint->left;
}

main()
{
    init();

		g_points = g_pointsStop;
		g_pointsCount = sizeof(g_pointsStop) / sizeof(struct Point);

    LED_PORT |= (1 << LED);

    while(1)
    {

        handleInput();

        struct Point* nextPoint = &g_points[g_curPos];
        g_active = !pointsEqual(&g_point, nextPoint);
        g_point = *nextPoint;
//
        unsigned i = 0;
        for (; i < g_point.delay * 10; i++)
            _delay_ms(1);

        g_curPos++;
        if (g_curPos >= g_pointsCount)
            g_curPos = 0;
    }
}

ISR(TIMER0_OVF_vect)
{
    if (g_active) {
        if (g_servoCounter >= 627) {
            g_servoCounter = 0;
            SERVO_PORT |= (1 << SERVO_CENTER) | (1 << SERVO_RIGHT) | (1 << SERVO_LEFT);
        }
        if (g_servoCounter >= g_point.center)
            SERVO_PORT &= ~(1 << SERVO_CENTER);
        if (g_servoCounter >= g_point.right)
            SERVO_PORT &= ~(1 << SERVO_RIGHT);
        if (g_servoCounter >= g_point.left)
            SERVO_PORT &= ~(1 << SERVO_LEFT);
        g_servoCounter++;
    } else {
        // turn off servos if not active
        SERVO_PORT &= ~((1 << SERVO_CENTER) | (1 << SERVO_RIGHT) | (1 << SERVO_LEFT));
    }
}
