#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdint.h>


//#define MYUBRR 	F_CPU/16/38400-1
#define OFF 0x00
#define ON 0xFF
#define HALL PORTC0
#define bitset(var,bitno) ((var) |= (1 << (bitno)));

int latch=0, counter=0,count=0, cpt=0;
int lap_period=0, last_count=0;    //the period that one lap takes
int seconds=40,minutes=30,hours=2;//Time init


//--- USART ---//

void USART_Init(unsigned int ubrr) {

	//Baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

	// Enable transmission
	UCSR0B = _BV(RXEN0)|_BV(TXEN0);

	// Frame format >> 8data, 2stop bit
	UCSR0C = _BV(USBS0)|(3<<UCSZ00);
}

void USART_Transmit(unsigned char data) {
	// Get data and send in the buffer
  while (!( UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}

unsigned char USART_Receive() {
  // Get data from buffer and return it
  while ( !(UCSR0A & _BV(RXC0)) );
  return UDR0;
}


//--- SPI ---//

void SPI_MasterInit(void){
  // Set OE and LE output
  DDRD |= (1<<PD3)|(1<<PD4);
  // Set MOSI and SCK output
  DDRB = (1<<PB3)|(1<<PB5)|(1<<PB2);
  //set PC0 as input to read HALL
  DDRC&= ~(1<<PC0);
  //Enable SPI, Master, set clock rate fck/16
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
  PORTD &= ~_BV(PD3);
}

void SPI_MasterTransmit(char cData){
  //Start transmission
  SPDR = cData;
  // Wait for transmission complete
  while (!(SPSR & (1<<SPIF)));
}

//Change the led lights
void Display(uint8_t c1,uint8_t c2 ){
  SPI_MasterTransmit(c1);
  SPI_MasterTransmit(c2);
  PORTD |=  _BV(PD4);
  PORTD &=  ~_BV(PD4);
}

//Adjust time
int coef(int t,int part){
 if(t <part)
   return t +part;
 else
   return t - part;
}


// Set up the new leds order
void clockLight(){
  unsigned short Lresult = 0,Hresult = 0;
  unsigned short current_cpt= cpt*60/last_count;
  //Display watch hands
  if((current_cpt==coef( seconds,30))||(coef( current_cpt,30) <=seconds))
    {
      Lresult = 0x10;
    }
  else
      Lresult = 0x00;

  if (current_cpt == coef(minutes,30))
    {
      Lresult |= 0x0F;
      Hresult = 0xFF;
    }
  else if (current_cpt ==coef( hours*5,30))
    {
      Lresult |= 0x0;
      Hresult = 0xFF;
    }
      // Display hours (3, 6, 9, 12)
  if (current_cpt % 15 == 0)
    Lresult |= 0b11100000;

  // Display hours (1, 2, 4, 5, 7, 8, 10, 11)
  else if (current_cpt % 5 == 0)
    Lresult |= 0b11000000;

  // Round cercle display
  else
    Lresult |= 0b10000000;

  Display(Lresult,Hresult);

}

void SetUp_time(){
  seconds++;
  if(seconds>59){
    minutes++;
    seconds=0;
  }
  if(minutes>59){
    hours++;
    minutes=0;
  }
  if(hours>12){
    hours=1;
  }
}

// Called in each lap
ISR(PCINT1_vect){
  if(PINC&(1<<PINC0)){
    latch=TCNT1;
    lap_period=latch+cpt*OCR1A;
    last_count=cpt;
    cpt=0;
    TCNT1= 0x00;
    count=lap_period;
  }
}



//To compute time within a lap
ISR (TIMER1_COMPA_vect){
  cpt++;
}


//To calculate seconds, minutes and hours
ISR (TIMER0_COMPA_vect){
  //Update time
  counter++;
  if (counter ==125){
    SetUp_time();
    counter=0;
  }
}

void Timer1_interruptInit(){

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
    OCR1A=15999;//= (16000000/(1*1000))-1 // each 1ms
   // Mode 4, CTC on OCR1A
   TCCR1B |= (1 << WGM12);
   //Set interrupt on compare match
   TIMSK1 |= (1 << OCIE1A);
   // autoriser les overflow interrupt pour timer 1
   TIMSK1 |= (1<<TOIE1);
  // set prescaler to 1 and start the timer
   TCCR1B |= (1 << CS10);
}



void Timer0_interruptInit(){
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  // OCR0A = 124;// 124= (16*10^6) / (500*64) - 1 (must be <256)
  OCR0A = 124;// 124= (16*10^6) / (1024*125) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  // Set CS00 et CS02 bits for 1024 prescaler
  TCCR0B |=(1 << CS00)|(1 << CS02);

}
/*
void TimerBenchMark_Init()
{
 //set timer2 interrupt at 1Hz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  OCR2A=124;// 3999= (16*10^6) / (500*8) - 1 (must be <65356) incrementation every 8 ms
  // Mode 4, CTC on OCR2A
  TCCR2B |= (1 << WGM22);
  //Set interrupt on compare match
  TIMSK2 |= (1 << OCIE2A);
  // set prescaler to 8 and start the timer
  TCCR2B |=  (1 << CS22);
}

int TimerBenchMark_End()
{
  int measured_time = TCNT2 * 8; //measured time (ms);
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  return measured_time;
}
*/
void Pin_Change_interruptInit(){
  DDRC |= (0<<HALL); // set HALL as input
  PORTC &= ~(1<<HALL); // clear HALL
   PCICR |= (1<<PCIE1);
  PCMSK1 |= (1<<PCINT8); //enable HALL as interrupt source
}



int main(void)
{ cli();
  //Initiation
  // USART_Init(MYUBRR);
  SPI_MasterInit();
  // ----interrupt timers---------
  Timer1_interruptInit();
  Timer0_interruptInit();
  // ----interrupt pin change----
  Pin_Change_interruptInit();
  //---- enable interrupts----
  sei();

  while (1){
    clockLight();
  }
}
