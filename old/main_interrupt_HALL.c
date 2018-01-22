#include <avr/io.h>
#include <avr/interrupt.h>

#define HALL	PORTC0

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


ISR(PCINT1_vect)
{
  c1=~c1;
  c2=~c2;
}

int main(void)
{
  SPI_MasterInit();
  DDRC |= (0<<HALL); // set HALL as input
  PORTC &= ~(1<<HALL); // clear HALL

  PCICR |= (1<<PCIE1);
  PCMSK1 |= (1<<PCINT8); //enable HALL as interrupt source
  sei();
  while(1)
    {
	     SPI_MasterTransmit(c1);
	     SPI_MasterTransmit(c2);

	     PORTD |=  _BV(PD4);
	     PORTD &=  ~_BV(PD4);
    }
}
