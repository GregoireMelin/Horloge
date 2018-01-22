// this code sets up timer1 for a 1s  @ 16Mhz Clock (mode 4)


#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t c1 = 0b10000001;//0 = led qui sont éteintes
uint8_t c2 = 0b00000110;

void SPI_MasterInit(void){
        /* Set OE and LE output */
        DDRD |= (1<<PD3)|(1<<PD4);
        /* Set MOSI and SCK output, all others input */
        DDRB = (1<<PB3)|(1<<PB5)|(1<<PB2);
	/**/
	DDRC&= ~(1<<PC0);

        /* Enable SPI, Master, set clock rate fck/16 */
        SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
        PORTD &= ~_BV(PD3);
}

void SPI_MasterTransmit(char cData){
        /* Start transmission */
        SPDR = cData;
        /* Wait for transmission complete */
        while (!(SPSR & (1<<SPIF)));
}


int main(void)
{  SPI_MasterInit();

    OCR1A = 0x3D08;

    TCCR1B |= (1 << WGM12);
    // Mode 4, CTC on OCR1A

    TIMSK1 |= (1 << OCIE1A);
    //Set interrupt on compare match

    TCCR1B |= (1 << CS12) | (1 << CS10);
    // set prescaler to 1024 and start the timer


    sei();
    // enable interrupts


    while (1)
    {
      //main loop
      SPI_MasterTransmit(c1);
      SPI_MasterTransmit(c2);
      
      PORTD |=  _BV(PD4);
      PORTD &=  ~_BV(PD4);
    }
}

ISR (TIMER1_COMPA_vect)
{
  // action to be done every 1 sec
  c1=~c1;
  c2=~c2;
}
