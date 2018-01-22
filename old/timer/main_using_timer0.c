// this code sets up a timer0 for 4ms @ 16Mhz clock cycle
// an interrupt is triggered each time the interval occurs.
#include <avr/io.h> 
#include <avr/interrupt.h>
// compteur
volatile unsigned char cpt=0;
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

int main(void){
 
  SPI_MasterInit();
  // Set the Timer Mode to CTC
  TCCR0A |= (1 << WGM01);
  // Set the value that you want to count to
  OCR0A = 0xF9;
  TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect
  sei();         //enable interrupts
  // set prescaler to 256 and start the timer
  TCCR0B |= (1 << CS02);   
  while (1)
    {
      //main loop
      SPI_MasterTransmit(c1);
      SPI_MasterTransmit(c2);
      
      PORTD |=  _BV(PD4);
      PORTD &=  ~_BV(PD4);
    }
  return 0;
}

ISR (TIMER0_COMPA_vect) { // timer0 overflow interrupt
  //event to be exicuted every 4ms here
  cpt++;
  if (cpt == 124) { // 500ms/4ms = 125
    cpt = 0;
    c1=~c1;
    c2=~c2;
  }
}
