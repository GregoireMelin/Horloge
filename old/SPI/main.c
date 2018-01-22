#include <avr/io.h>
#include <util/delay.h>

#define BAUD 115200
#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
/*#define DD_SS       0b00000100
#define DD_MOSI     0b00001000
#define DD_MISO     0b00010000
#define DD_SCK      0b00100000
*/
#define DD_SS       0b00000000
#define DD_MOSI     0b00000000
#define DD_MISO     0b00000000
#define DD_SCK      0b00000000

void SPI_MasterInit(void)
{
  /*DDR_SPI &= ~( DD_MOSI | DD_MISO | DD_SS | DD_SCK );
  DDR_SPI |= (DD_MOSI | DD_SS | DD_SCK); */
  /* Set MOSI and SCK output, all others input*/
  DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
  /* Enable SPI, Master, set clock rate fck/16 */
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_MasterTransmit(char cData)
{
  /* Start transmission */
  SPDR = cData;
  /* Wait for transmission complete */
  while(!(SPSR & (1<<SPIF)));
}

void SPI_SlaveInit(void)
{
  /* Set MISO output, all others input */
  DDR_SPI = (1<<DD_MISO);
  /* Enable SPI */
  SPCR = (1<<SPE);
}
char SPI_SlaveReceive(void)
{
  /* Wait for reception complete */
  while(!(SPSR & (1<<SPIF)));
  /* Return Data Register */
  return SPDR;
}

int main()
{
  DDRB |= _BV(PB5);

  while(1)
    {
      SPI_MasterInit ();
      _delay_ms(200); //attente en ms
      PORTB |= _BV(PB3);// allume PB5
      _delay_ms(200);
      PORTB &= ~_BV(PB3); //eteint PB5
      unsigned char r;
      unsigned char cmd = 0b01111111;
/*PORTB |=_BV(PB3); //MOSI à 1
PORTB |=_BV(PB5); //SCK à 1
*/ SPCR=0b0110000;
      unsigned char numbers[] = {0, 1, 1, 0, 0};
      //(cmd >> 3)&1;

      /*DacA,1,x1: Vout=Vref*D/4096,SHDN :Vout available, data,4, */
      SPI_MasterTransmit(cmd);
      SPI_SlaveInit();
      r= SPI_SlaveReceive();

    }
  return 0;
}
