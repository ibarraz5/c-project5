/*
 * delay.c
 *
 * SER486 Project 1
 * Ivan Barraza Duran
 *
 * The function of this file is to implement 2 instances of the delay timer and both of them use 
 * the 328p's timer0 and a resolution of 1ms
 *
 * public functions below:
 *    unsigned int delay_isdone(unsigned num);
 *
 *    void temp_set(unsigned num, unsigned msec);
 *
 *    unsigned delay_get(unsigned num);
 */



// define relevant registers so they can be used
#define TCCR0A (*((volatile char *)0x44))
#define TCCR0B (*((volatile char *)0x45))
#define TIMSK0 (*((volatile char *)0x6E))
#define OCR0A (*((volatile char *)0x47))
#define SREG (*((volatile char *)0x5F))

// define constant variable for 2 timers
#define NUM_TIMERS 2

 static unsigned int count[] = {0,0};
 static unsigned int limit[] = {0,0};

 /*
 delay_init()
 arguments:
    none
 returns:
    nothing
 changes:
    OCR0A register is changed to 249
    TCCR0A is changed to CTC mode
    TCCR0B register is changed to 64
    TIMSK0 register is changed to enable interrupts on compare A
*/
static void delay_init(){

    // set timer0 compare for 1ms tick = 1000hz
    // prescalar 64 and atmega328p has clock of 16mhz
    // 16M/64 = 250khz
    // compare = 250k/1000 - 1 = 249
    OCR0A = 249;

    // Set CTC mode
    TCCR0A |= 0x02;

    // Set clock divisor
    // need a prescalar of 64 which means setting prescalar
    // bit to 011b
    TCCR0B |= 0x03;

    // Enable interrupts on output compare A
    TIMSK0 |= 0x02;
}

/*
 get_global_interrupt_state()
 arguments:
    none
 returns:
    status of global interrupters 0 = off > 0 = on
 changes:
    nothing
*/
static unsigned char get_global_interrupt_state(){
    return (SREG & 0x80);
}

/*
 set_global_interrupt_state(unsigned char state)
 arguments:
    unsigned char - the already shifted value to or equal to SREG
 returns:
    none
 changes:
    global interrupt bit of the SREG register
*/
static void set_global_interrupt_state(unsigned char state){
    SREG |= state;
}

/*
__vector_14(void)
arguments:
    none
returns:
    nothing
NOTE:This function is only called by the Atmega 328p
*/
void __vector_14(void) __attribute__ ((signal, used, externally_visible));
void __vector_14(void){
    // increment the count for each timer as long as it is not greater than or equal to the limit
    for(int n = 0; n < NUM_TIMERS; n++ ){
        if(count[n] < limit[n]){
            count[n]++;
        }
    }
}



/*
delay_get()
This function gets the current count for the given timer
arguments:
    unsigned int num - the timer to get the count for either 0 or 1
returns:
    The count associated with the given timer
changes:
    Global interrupts are temporarily disabled. They are restored to previous state before returning.
NOTE: make sure to call delay_set before calling get otherwise the result
will be wrong.
*/
unsigned delay_get(unsigned num){
    // Get current global interrupt enable bit state
    unsigned char saved_interrupt_state = get_global_interrupt_state();

    // Disable interrupts
    __builtin_avr_cli();

    // Get the count[n] value
    unsigned int count_n = count[num];

    // Restore saved global interrupt state
    set_global_interrupt_state(saved_interrupt_state);

    // return the count
    return count_n;
}

/*
delay_set()
This function sets up a delay timer with a given limit
arguments:
    unsigned int num - the timer to setup either 0 or 1
    unsigned int msec - the limit to apply
returns:
    nothing
changes:
    Global interrupters are temporarily disabled. They are restored to previous state before returning.
*/
void  delay_set(unsigned int num, unsigned int msec){
    static int initialized = 0;
    // if delay has not been initialized, then call init()
    if(initialized == 0){
        delay_init();
        initialized = 1;
    }

    // get global interrupt state
    unsigned char saved_interrupt_state = get_global_interrupt_state();

    // disable interrupts
    __builtin_avr_cli();

    // set the limit[n] and reset count[n] to 0
    limit[num] = msec;
    count[num] = 0;

    // Restore saved state of global interrupts
    set_global_interrupt_state(saved_interrupt_state);


}

/*
delay_isdone()
This function checks whether the specified delay is complete
arguments:
    unsigned int num - the timer to get the result of either 0 or 1
returns:
    unsigned int - 0 = delay not done, 1 = delay is done
changes:
    None
NOTE: make sure to call delay_set before calling get otherwise the result
will be wrong.
*/
unsigned delay_isdone(unsigned int num){
    unsigned int result = 0;
    if(count[num] == limit[num]){
        result = 1;
    }

    return result;
}


