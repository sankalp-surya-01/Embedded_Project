#include <avr/io.h>
#include <util/delay.h>
#include "dio.h"
#include "uart.h"
#include "adc.h"

//7-segment pins and ports
char g = 2;
char f = 3;
char a = 4;
char b = 5;
char e = 6;
char c = 7;
char d = 0;
char gPort = 'D';
char fPort = 'D';
char aPort = 'D';
char bPort = 'D';
char ePort = 'D';
char cPort = 'D';
char dPort = 'B';

//LED1 pin and port
char led1 = 1;
char led1Port = 'B';

//LED2 pin and port
char led2 = 2;
char led2Port = 'B';

//Buzzer pin and port
char buzzer = 3;
char buzzerPort = 'B';

//Pushbutton pin and port
char pb = 4;
char pbPort = 'B';

//ADC channels of the analog sensors
char LDR = 0;
char potentiometer = 1;
char LM = 2;

//irrigation variables
int waterLvl;                    //selected to be 1, 2, or 3 by the user turning the knob of the potentiometer 
char waterLvl_str[10];           //'low', 'moderate', or 'high'
char times[5];                   //number of times the plant needs to be irrigated in a day
char buffer[10];                 //A generic character string to hold any numerical values that need to be sent as character arrays via UART through itoa()
int duration = 0;                //number of seconds the plant needs to be irrigated
int n = 1;                       //counter for the main loop to slow down the monitoring of the temprature and brightness
unsigned short brightness;       //digital value of LDR after ADC
unsigned short temperature;      //digital value of LM35 after ADC

void init();                     //one-time setup function
void dim();                      //turns off all 7-segments leds
void one();                      //forms 1 on the 7-segments display
void two();                      //forms 2 on the 7-segments display
void three();                    //forms 3 on the 7-segments display
void alertSequence();            //start and end sequence of irrigation mode
unsigned short getLightness();   //returns the value of LDR after ADC
unsigned short getTemperature(); //returns the value of LM35 after ADC
void sendEnvironmental(unsigned short LDR_val, unsigned short temp_val);              //prints the brightness and temprature information to the serial monitor
int getDuration(unsigned short LDR_val, unsigned short temp_val);                     //calculates the duration in seconds of the irrigation based on the value of brightness and temprature

int main(void) {

  init();

  while (1) {
    if (Dio_GetPinState(pbPort, pb) != 0) {                                         //Checks if the button is pressed. If so, enters irrigation mode.
      _delay_ms(200);                                                               //Short delay to prevent bouncing at the button (to be developed)
      Uart_SendString("You pressed the button.\nGood job on growing a NEW plant!\nPlease turn the knob to select the amount of water the plant needs. (1: low, 2: moderate, 3: high)\nPress the button when your selection is final.\n\n");
      
      while (Dio_GetPinState(pbPort, pb) == 0) {                                    //Checks if the button is pressed. If not, adjusts the 7-segments and water level based on the potentiometer value
        waterLvl = ((Adc_ReadChannel(potentiometer) * 3) / 1024) + 1;               //maps the potentiometer value into 1, 2, or 3
        if (waterLvl == 1) {                                                        //for each water level value,
          one();                                                                    //the 7-segments display is adjusted to reflect the current selection
          strcpy(waterLvl_str, "low");                                              //the current water level is converted into string and copied into the character array
          strcpy(times, "10");                                                      //the respective number of irrigation times is calculated and copied into the character array
        } else if (waterLvl == 2) {
          two();
          strcpy(waterLvl_str, "moderate");
          strcpy(times, "20");
        } else {
          three();
          strcpy(waterLvl_str, "high");
          strcpy(times, "30");
        }
      }                                                                             //the button is now pressed -> the user has finalized their water level selection
      
      _delay_ms(200); 
      Uart_SendString("You have pressed the button.\nYou indicated that your plant needs ");
      Uart_SendString(waterLvl_str);
      Uart_SendString(" water level. It will need irrigation ");
      Uart_SendString(times);
      Uart_SendString(" times a day.\n");
      
      Uart_SendString("The first irrigation process will start with the alert going off now.\nPlease press the button to turn the alert off once you start irrigating.\nKeep on going till the alert goes off again. Re-press the button to indicate that you stopped irrigation thus turing the alert off.\n");
      brightness = getLightness();
      temperature = getTemperature();
      sendEnvironmental(brightness, temperature);
      duration = getDuration(brightness, temperature);                         //calculating the duration of the irrigation

      Uart_SendString("Based on the environmental factors mentioned, you'll need to irrigate for ");
      itoa(duration, buffer, 10);                                              //converting from the numerical value of duration to string stored in buffer
      Uart_SendString(buffer);
      Uart_SendString(" seconds.\n\n");

      //the irrigation process starts and ends by the alertSequence which could be turned off by pressing the button
      while (Dio_GetPinState(pbPort, pb) == 0) alertSequence();
      Uart_SendString("Starting Irrigation!\n");    
        
      if (duration==5) _delay_ms(5000);
      else if (duration==7) _delay_ms(7000);
      else if (duration==3) _delay_ms(3000);
      else _delay_ms(9000);

      while (Dio_GetPinState(pbPort, pb) == 0) alertSequence();
      Uart_SendString("Done Irrigating!\n"); 
      _delay_ms(200);                          
    }
    //In normal mode, the brightness and temprature are being continously measured
    brightness = getLightness();
    temperature = getTemperature();

    //Setting a lower threshold for the brightness value, shown by the led lighting up.
    if(brightness<850) Dio_SetPinState(led1Port, led1, 1);
    else Dio_SetPinState(led1Port, led1, 0);
    //Setting an upper threshold for the temperature value, shown by the led lighting up.
    if(temperature>=46) Dio_SetPinState(led2Port, led2, 1);
    else Dio_SetPinState(led2Port, led2, 0);
    
    //Every 1000 loop, the environmental factors (brightness and temperature) will be sent to the user
    if (n % 1000 == 0) {
      if (n > 1000) n = 0;
      Uart_SendChar('c');
      sendEnvironmental(brightness, temperature);
    }
    n = n + 1;
  }

  return 0;
}

void init() {
  Uart_Init();
  Adc_Init();

  //Configuring all the 7-segment pins as output
  Dio_SetPinDirection(aPort, a, 'o');
  Dio_SetPinDirection(bPort, b, 'o');
  Dio_SetPinDirection(cPort, c, 'o');
  Dio_SetPinDirection(dPort, d, 'o');                     
  Dio_SetPinDirection(ePort, e, 'o');
  Dio_SetPinDirection(fPort, f, 'o');
  Dio_SetPinDirection(gPort, g, 'o');
  dim();                                                  //initializing the 7-segments to be completely turned off

  Dio_SetPinDirection(led1Port, led1, 1);                 //configuring LED1 as output (pin 9 on Uno)
  Dio_SetPinDirection(led2Port, led2, 1);                 //configuring LED2 as output (pin 10 on Uno)
  Dio_SetPinDirection(buzzerPort, buzzer, 1);             //configuring Buzzer as output (pin 11 on Uno)

  Dio_SetPinDirection(pbPort, pb, 0);                     //configuring Pushbutton as input (pin 12 on Uno)
}

//setting all the output pins of the 7-segments as low (turned off)
void dim() {
  Dio_SetPinState(aPort, a, 0);
  Dio_SetPinState(bPort, b, 0);
  Dio_SetPinState(cPort, c, 0);
  Dio_SetPinState(dPort, d, 0);
  Dio_SetPinState(ePort, e, 0);
  Dio_SetPinState(fPort, f, 0);
  Dio_SetPinState(gPort, g, 0);
}

//setting only leds b, c of the 7-segments to be high, forming number 1
void one() {
  Dio_SetPinState(aPort, a, 0);
  Dio_SetPinState(bPort, b, 1);
  Dio_SetPinState(cPort, c, 1);
  Dio_SetPinState(dPort, d, 0);
  Dio_SetPinState(ePort, e, 0);
  Dio_SetPinState(fPort, f, 0);
  Dio_SetPinState(gPort, g, 0);
}

//setting only leds a, b, d, e, g of the 7-segments to be high, forming number 2
void two() {
  Dio_SetPinState(aPort, a, 1);
  Dio_SetPinState(bPort, b, 1);
  Dio_SetPinState(cPort, c, 0);
  Dio_SetPinState(dPort, d, 1);
  Dio_SetPinState(ePort, e, 1);
  Dio_SetPinState(fPort, f, 0);
  Dio_SetPinState(gPort, g, 1);
}

//setting only leds a, b, c, d, g of the 7-segments to be high, forming number 3
void three() {
  Dio_SetPinState(aPort, a, 1);
  Dio_SetPinState(bPort, b, 1);
  Dio_SetPinState(cPort, c, 1);
  Dio_SetPinState(dPort, d, 1);
  Dio_SetPinState(ePort, e, 0);
  Dio_SetPinState(fPort, f, 0);
  Dio_SetPinState(gPort, g, 1);
}

//the alert sequence is 2 leds alternating with the buzzer always on. 
void alertSequence() {
  Dio_SetPinState(buzzerPort, buzzer, 'h');
  Dio_SetPinState(led1Port, led1, 'h');
  _delay_ms(200);
  Dio_SetPinState(led1Port, led1, 'l');
  Dio_SetPinState(led2Port, led2, 'h');
  _delay_ms(200);
  Dio_SetPinState(led2Port, led2, 'l');
  Dio_SetPinState(buzzerPort, buzzer, 'l');
}

unsigned short getTemperature() {
  return Adc_ReadChannel(LM);
}

unsigned short getLightness() {
  return Adc_ReadChannel(LDR);
}

void sendEnvironmental(unsigned short LDR_val, unsigned short temp_val) {
  Uart_SendString("Current lightness level is: ");
  itoa(LDR_val, buffer, 10);                            //converts the numerical variable LDR_val to character array and stores it at buffer
  Uart_SendString(buffer);
  Uart_SendChar('\n');

  Uart_SendString("Current temperature is: ");
  itoa(temp_val, buffer, 10);                           //converts the numerical variable temp_val to character array and stores it at buffer
  Uart_SendString(buffer);
  Uart_SendChar('\n');
}

//calculates the duration of the irrigation based on the brightness and temperature values
int getDuration(unsigned short LDR_val, unsigned short temp_val) {
  if (temp_val < 45 or LDR_val < 850) return 5;
  if (temp_val < 47 and LDR_val <900) return 7;
  if (temp_val < 49 and LDR_val > 900) return 3;
  else return 9;
}