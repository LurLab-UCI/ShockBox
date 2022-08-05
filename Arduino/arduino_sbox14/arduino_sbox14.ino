#include "Waveforms.h"
#define oneHzSample 1000000/maxSamplesNum  // sample for the 1Hz signal expressed in microseconds
//  audio amp  ------------------------------------
#define MAX9744_I2CADDR 0x4B

// Include the required Wire library for I2C
#include <Wire.h>
#include <Tone.h>


#define  SAMPLES   32
#define  NSAMPLES   8192
#define  CEILING   4096
uint16_t wav0[SAMPLES];
uint16_t wav1[SAMPLES];
uint16_t wavn0[NSAMPLES];
uint16_t wavn1[NSAMPLES];
uint16_t temp = 0;

uint16_t temp1 = 0;
uint16_t temp2 = 0;
uint16_t temp3 = 0;
uint16_t temp4 = 0;

uint32_t temp_time = 0;
uint32_t temph = 0;


float   tfl = 0;  
String volume = "VOLUME";

uint8_t noise_flag = 0;
uint8_t signal_off = 0;
uint8_t fpointer = 0;

uint16_t timer = 0;
uint32_t tmp = 0;
uint8_t signal_or_delay = 0; 


uint32_t shock_timer = 0;
uint32_t interval_timer = 0;
uint32_t delay_timer = 0;
uint32_t tone_timer = 0;

uint16_t number_of_cycles = 0;
uint16_t number_of_shocks = 0;
uint16_t shock_duration[1000];
uint32_t interval_duration[1000];
uint16_t sdelay[1000];
uint16_t tone_duration[1000];
uint16_t stone[1000];
uint8_t tone_before_shock = 0;  
uint8_t shock_active = 0;  
uint8_t delay_active = 0;  
uint8_t tone_active = 0;  

uint8_t current = 0;  
int8_t  vol = 0; 

// frequency pars
#define  NCHAR  16
#define  NITEMS  10
char* freqstr[][NCHAR]={" Noise           "," 2 kHz          ", " 4 kHz          " " 6 kHz          ", " 8 kHz          ",
"12 kHz          ", "16 kHz          ","24 kHz          ","32 kHz          ","Random          "};
float freq[9] = {1000,2000,4000,6000,8000,12000,16000,24000,32000};

// volume pars
#define  N_VOL_ITEMS  44
int8_t up_down_vol = 0;
uint8_t rbyte = 0;

uint8_t idx = 0;
uint8_t tdata = 0;

//Tone tone1;
int x = 0;
int y = 0;
byte skip = 1;

uint8_t flag = 1;

volatile int wave0 = 0, wave1 = 1;
volatile int wave2 = 2, wave3 = 3;
int wavel = 0, waver = 0;

const byte tout = 31;  
const byte tonepin = 3;  

int i = 0;
int sample;

//=========================================================
//pin assignments
const byte test_in = 45;  
const byte shock = 47;  
const byte shock_inv = 40;  
const byte tone_pin = 42;  
const byte tpin =  44;  
const byte tline = 34; 
const byte tpin53 = 53; 

const byte cur0 = 39; 
const byte cur1 = 37; 
const byte cur2 = 35; 
const byte cur3 = 33; 
const byte cur4 = 31; 
const byte cur5 = 29; 
const byte cur6 = 27; 
const byte cur7 = 25; 
//=========================================================



void setup() {


  pinMode(test_in, OUTPUT);
  digitalWrite(test_in, HIGH);
  pinMode(shock, OUTPUT);
  digitalWrite(shock, HIGH);
  pinMode(shock_inv, OUTPUT);
  digitalWrite(shock_inv, LOW);
  pinMode(tone_pin, OUTPUT);
  digitalWrite(tone_pin, LOW);
  pinMode(tpin, OUTPUT);
  digitalWrite(tpin, LOW);
  
 analogWriteResolution(12);  // set the analog output resolution to 12 bit (4096 levels)
  pinMode(tout, OUTPUT);
  digitalWrite(tout, LOW);  
  pinMode(3, OUTPUT);
  //digitalWrite(3, LOW);  

  pinMode(cur0, OUTPUT);
  pinMode(cur1, OUTPUT);
  pinMode(cur2, OUTPUT);
  pinMode(cur3, OUTPUT);
  pinMode(cur4, OUTPUT);
  pinMode(cur5, OUTPUT);
  pinMode(cur6, OUTPUT);
  pinMode(cur7, OUTPUT);
  digitalWrite(cur0, LOW);  
  digitalWrite(cur1, LOW);  
  digitalWrite(cur2, LOW);  
  digitalWrite(cur3, LOW);  
  digitalWrite(cur4, LOW);  
  digitalWrite(cur5, LOW);  
  digitalWrite(cur6, LOW);  
  digitalWrite(cur7, LOW);  

  pinMode(tpin53, OUTPUT);
  digitalWrite(tpin53, LOW);  


//=======================================================
//  set volume to 0 on power-up


  CreateSineWaveTable0();
  CreateSineWaveTable1();
  CreateNoise();
  
  setupDAC();
  float freq_hz = 32000; // Target: 200kHz
  setupTC(freq_hz);
  NVIC_EnableIRQ(DACC_IRQn);


  Serial.begin(115200);

  // Start the I2C Bus as Slave on address 9
// Wire.begin(9); 
  Wire.begin(); 
  // Attach a function to trigger when something is received.
//  Wire.onReceive(receiveEvent);

 Serial.println(PIN_WIRE_SCL);

  vol = 0;
  //setvolume(vol);

  if (! setvolume(vol)) {
    Serial.println("Failed to set volume, MAX9744 not found!");
    while (1);
  }



Serial.println("Rst");
idx = 0;
number_of_cycles = 0;

delay(100);
interval_timer = millis();;
shock_active = 0;
delay_active = 0;
tone_active = 0;

  
}
void loop() {

if (tdata == 0)
{
   tdata = 1;
  digitalWrite(tpin, HIGH);
}
else
{
  tdata = 0;
  digitalWrite(tpin, LOW);
}



temp_time = millis();
 
    if (number_of_cycles > 0)
    {

      // check for end of cycle 
          temph = (temp_time - interval_timer);
          if (temph > interval_duration[idx])
          {
            //start new shock
            idx = idx + 1;
            new_cycle();
          }

    if (tone_before_shock == 0)
    {
          // check for end of shock
          temph = (temp_time - shock_timer);
          if (temph > shock_duration[idx])
          { 
            // shock 
            digitalWrite(shock, HIGH);
            digitalWrite(shock_inv, LOW);
            shock_active = 0;
          }
    

          // check delay
           if (delay_active == 1)
           {
                    temph = (temp_time - delay_timer);
                    if (temph > sdelay[idx])
                    {
                      //delay expired
                      delay_active = 0;
          //            tone on
                      tone_active = 1;
                      tone_timer = temp_time;
                      digitalWrite(tline, HIGH);
                      tone_on();
                    }
           }

          // check TONE
           if (tone_active ==1)
           {
                    temph = (temp_time - tone_timer);
                    if (temph > tone_duration[idx])
                    {
          //            tone off
                      tone_active = 0;
                      digitalWrite(tline,LOW);
                      tone_off();
                    }
           }

 
    }
    else   // tone first  +++++++++++++++++++++++++++++++++++++++++++++++
    {

          // check for end of tone
          temph = (temp_time - tone_timer);
          if (temph > tone_duration[idx])
          { 
            // tone expired
            digitalWrite(tline, LOW);
            tone_off();
            tone_active = 0;
          }
    

          // check delay
           if (delay_active == 1)
           {
                    temph = (temp_time - delay_timer);
                    if (temph > sdelay[idx])
                    {
                      delay_active = 0;
          //          delay expired, shock on
                      shock_active = 1;
                      shock_timer = temp_time;
                      digitalWrite(shock, LOW);
                      digitalWrite(shock_inv, HIGH);
                    }
           }

          // check shock
           if (shock_active ==1)
           {
                    temph = (temp_time - shock_timer);
                    if (temph > shock_duration[idx])
                    {
          //         shock expired
                      shock_active = 0;
                      digitalWrite(shock,HIGH);
                      digitalWrite(shock_inv,LOW);
                    }
           }
    }
    }
    else
    {
            digitalWrite(shock,HIGH);
            digitalWrite(shock_inv,LOW);
            digitalWrite(tline,LOW);
            tone_active = 0;
            shock_active = 0;
    }

//Check if serial data available
    if ( Serial.available())
    decode_serial_data();
  }
//==================================================================================      
//void receiveEvent(int bytes) 
void decode_serial_data() {

    char inChar = (char)Serial.read();
    
    if (inChar == 'a')
    {
    while (!Serial.available()) {}
    fpointer = Serial.read();

    dacc_set_channel_selection(DACC, 0);
    dacc_enable_channel(DACC, 0);
    update_frequency();            
    return;
    }

    //---------------------------------
    if (inChar == 'b')
    {
      //Serial.println("bbbbbbbbbbbbb");
       dacc_set_channel_selection(DACC, 0);
       dacc_disable_channel(DACC, 0);
       return;      
    }
    //---------------------------------
      
    if (inChar == 'y')
    // number_of_s
    {
       idx = 0;
       new_cycle();
       number_of_cycles = number_of_shocks;
       return;
    }

    //---------------------------------
    if (inChar == 'z')
    {
       number_of_cycles = 0;
       digitalWrite(shock, HIGH);
       digitalWrite(shock_inv, LOW);
       shock_active = 0;
       tone_active = 0;
       tone_off();
       dacc_set_channel_selection(DACC, 0);
       dacc_disable_channel(DACC, 0);
       idx = 0;
       return;
    }

    //---------------------------------
    if (inChar == 'c')
    // number_of_shocks
    {
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       while (!Serial.available()) {}
       temp2 = Serial.read();   //on time
       number_of_shocks = temp1 + 256*temp2;
       while (!Serial.available()) {}
       tone_before_shock = Serial.read();   // mode
       while (!Serial.available()) {}
       current = Serial.read();   // mode
       current_to_port(); 
       Serial.println("Stp");
       return;
    }

    //---------------------------------
    // shock_durations
    if (inChar == 'd')
    {
       for (idx = 0; idx < number_of_shocks; idx++) 
      {      
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       while (!Serial.available()) {}
       temp2 = Serial.read();   //on time
       shock_duration[idx] = temp1 + 256*temp2;
//       shock_duration[idx] = 55;
       }
       Serial.println("aaa");
       return;
 }

    //---------------------------------
    // cycle_intervals
    if (inChar == 'e')
    {
       for (idx = 0; idx < number_of_shocks; idx++) 
      {      
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       while (!Serial.available()) {}
       temp2 = Serial.read();   //on time
       while (!Serial.available()) {}
       temp3 = Serial.read();   //on time
       interval_duration[idx] = temp1 + 256*temp2  + 65536*temp3;
//       interval_duration[idx] = 12;//temp1 + 256*temp2;
       }
       Serial.println("bbb");
       return;
    }

    //---------------------------------
    // delays
    if (inChar == 'f')
    {
       for (idx = 0; idx < number_of_shocks; idx++) 
      {      
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       while (!Serial.available()) {}
       temp2 = Serial.read();   //on time
       sdelay[idx] = temp1 + 256*temp2;
       }
       Serial.println("ccc");
       return;
    }
    //---------------------------------
    // tone durations
    if (inChar == 'g')
    {
       for (idx = 0; idx < number_of_shocks; idx++) 
      {      
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       while (!Serial.available()) {}
       temp2 = Serial.read();   //on time
       tone_duration[idx] = temp1 + 256*temp2;
       }
       Serial.println("ddd");
       return;
    }

    //---------------------------------
    // tones
    if (inChar == 'h')
    {
       for (idx = 0; idx < number_of_shocks; idx++) 
      {      
       while (!Serial.available()) {}
       temp1 = Serial.read();   // on  time
       stone[idx] = temp1;
      }
       Serial.println("eee");
       //number_of_cycles = number_of_shocks;
       return;
    }

//-----------------------------------------------
// set audio volume
      if (inChar == 'v')
      {
        while (!Serial.available()) {}
        vol = Serial.read();
        setvolume(vol);
        return;
      }



}  // end of decode serial data



// Incantations for DAC set-up for analogue wave using DMA and timer interrupt.
// http://asf.atmel.com/docs/latest/sam3a/html/group__sam__drivers__dacc__group.html
void setupDAC() {
  pmc_enable_periph_clk (DACC_INTERFACE_ID) ;   // Start clocking DAC.
  dacc_reset(DACC);
  dacc_set_transfer_mode(DACC, 0);
  dacc_set_power_save(DACC, 0, 1);              // sleep = 0, fast wakeup = 1
  dacc_set_analog_control(DACC, DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02) | DACC_ACR_IBCTLDACCORE(0x01));
  dacc_set_trigger(DACC, 1);
//  dacc_set_channel_selection(DACC, 1);
//  dacc_enable_channel(DACC, 1);
  dacc_set_channel_selection(DACC, 0);
  //dacc_enable_channel(DACC, 0);
  NVIC_DisableIRQ(DACC_IRQn);
  NVIC_ClearPendingIRQ(DACC_IRQn);
  NVIC_EnableIRQ(DACC_IRQn);
  dacc_enable_interrupt(DACC, DACC_IER_ENDTX);
  DACC->DACC_PTCR = 0x00000100;
}

void DACC_Handler(void) {
  if (noise_flag == 1)
  {
    DACC->DACC_TNPR = (uint32_t) wavn0;
    DACC->DACC_TNCR = NSAMPLES;                // Number of counts until Handler re-triggered
  }
  else
  {
    DACC->DACC_TNPR = (uint32_t) wav0;
    DACC->DACC_TNCR = SAMPLES;                // Number of counts until Handler re-triggered
  }
}

// System timer clock set-up for DAC wave.
void setupTC (float freq_hz) {  
  int steps = (420000000UL / freq_hz) / (10*SAMPLES);
  pmc_enable_periph_clk(TC_INTERFACE_ID);
  TcChannel * t = &(TC0->TC_CHANNEL)[0];
  t->TC_CCR = TC_CCR_CLKDIS;                // Disable TC clock.
  t->TC_IDR = 0xFFFFFFFF;
  t->TC_SR;                                 // Clear status register.
  t->TC_CMR =                               // Capture mode.
              TC_CMR_TCCLKS_TIMER_CLOCK1 |  // Set the timer clock to TCLK1 (MCK/2 = 84MHz/2 = 48MHz).
              TC_CMR_WAVE |                 // Waveform mode.
              TC_CMR_WAVSEL_UP_RC;          // Count up with automatic trigger on RC compare.
  t->TC_RC = steps;                         // Frequency.
  t->TC_RA = steps /2;                      // Duty cycle (btwn 1 and RC).
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | 
              TC_CMR_ACPA_CLEAR |           // Clear TIOA on counter match with RA0.
              TC_CMR_ACPC_SET;              // Set TIOA on counter match with RC0.
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;  // Enables the clock if CLKDIS is not 1.
}


//--------------------------------------------------------------------------------------------



void CreateSineWaveTable0() {
 for(int i = 0; i < SAMPLES; i++) 
  {
       tfl =  CEILING/2 + (CEILING/2)*sin(float(2*3.1416)*float(i+1)/SAMPLES);
       //Serial.println(tfl);
       temp = round(tfl);
//       Serial.println(temp);
       if (temp>(CEILING-1))
          temp = CEILING-1;

      wav0[i] = temp;
  }
}


void CreateSineWaveTable1() {
 for(int i = 0; i < SAMPLES; i++) 
  {
       tfl =  CEILING/2 + (CEILING/2)*sin(float(2*3.1416)*float(i+1)/SAMPLES);
       //Serial.println(tfl);
       temp = round(tfl);
//       Serial.println(temp);
       if (temp>(CEILING-1))
          temp = CEILING-1;

      wav1[i] = temp;
  }
}




void CreateNoise() {
 for(int i = 0; i < NSAMPLES; i++) 
  {
//       tfl =  CEILING/2 + (CEILING/2)*sin(float(2*3.1416)*float(i+1)/SAMPLES);
//       tfl =  CEILING/2 + (CEILING/2)*sin(float(2*3.1416)*float(i+1)/SAMPLES);
        tfl = random(0, 4095);
       //Serial.println(tfl);
       temp = round(tfl);
//       Serial.println(temp);
       if (temp>(CEILING-1))
          temp = CEILING-1;

      wavn0[i] = temp;
      wavn1[i] = temp;
  }
}

void CreateTriangleWaveTable1() {
  for(int i = 0; i < SAMPLES; i++) { int16_t v = (((1.0 / (SAMPLES - 1)) * (SAMPLES - 1 - i)) * CEILING); if (i > round(SAMPLES/2)) v*=-1;
    wav1[i] = v;
  }
}

// ------------------------------------------------------

void       update_frequency()            
{

      if (fpointer > 0)
      {

        if (fpointer == NITEMS-1)
        {
         // noise_flag = 0;
         // setupTC(10);
        }
        else
        {
          noise_flag = 0;
          setupTC(freq[fpointer]);
        }
      }
      else
      {
        noise_flag = 1;
        float freq_hz = 100;
        setupTC(freq_hz);
      }
      NVIC_EnableIRQ(DACC_IRQn);
      dacc_enable_interrupt(DACC, DACC_IER_ENDTX);
}
//===============================================================

void new_cycle()

    {

       Serial.println("ncl");

        //start new cycle
       if (tone_before_shock == 0)
        {
          // shock first
            shock_timer = millis();;
            if (shock_duration[idx] > 0)
            {
              digitalWrite(shock, LOW);
              digitalWrite(shock_inv, HIGH);
            }
            shock_active = 1;
            tone_active = 0;
        }
        else
        { 
          // tone first
            tone_timer = millis();;
            digitalWrite(tline, HIGH);
            tone_on();
            shock_active = 0;
            tone_active = 1;
        }
        
       number_of_cycles =  number_of_cycles - 1;
       delay_active = 1;
       interval_timer = millis();
       delay_timer = millis();
       if (number_of_cycles == 0)
       {
            tone_off();
            Serial.println("fff");
            
       }
       return;
    }



//===============================================================

void tone_on()
{
      fpointer = stone[idx];
      update_frequency();   
      dacc_set_channel_selection(DACC, 0);
      dacc_enable_channel(DACC, 0);
      digitalWrite(tone_pin, HIGH);
      
}
//===============================================================

void tone_off()
{
      dacc_set_channel_selection(DACC, 0);
      dacc_disable_channel(DACC, 0);

      NVIC_DisableIRQ(DACC_IRQn);
      dacc_disable_interrupt(DACC, DACC_IER_ENDTX);
      digitalWrite(tone_pin, LOW);
      
}

void       current_to_port()

{
    if (bitRead(current,0) == 0)
      digitalWrite(cur0, HIGH);
      else
      digitalWrite(cur0, LOW);
    
    if (bitRead(current,1) == 0)
      digitalWrite(cur1, HIGH);
      else
      digitalWrite(cur1, LOW);
    
    if (bitRead(current,2) == 0)
      digitalWrite(cur2, HIGH);
      else
      digitalWrite(cur2, LOW);
    
    if (bitRead(current,3) == 0)
      digitalWrite(cur3, HIGH);
      else
      digitalWrite(cur3, LOW);
    
    if (bitRead(current,4) == 0)
      digitalWrite(cur4, HIGH);
      else
      digitalWrite(cur4, LOW);
    
    if (bitRead(current,5) == 0)
      digitalWrite(cur5, HIGH);
      else
      digitalWrite(cur5, LOW);
    
    if (bitRead(current,6) == 0)
      digitalWrite(cur6, HIGH);
      else
      digitalWrite(cur6, LOW);
    
    if (bitRead(current,7) == 0)
      digitalWrite(cur7, HIGH);
      else
      digitalWrite(cur7, LOW);
}



// ------------------------------------------------------

// Setting the volume is very simple! Just write the 6-bit
// volume to the i2c bus. That's it!
boolean setvolume(int8_t v) {
    return true;
  
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
//  tcaselect(2);
//  Serial.print("Setting volume to ");
//  Serial.println(v);
  Wire.beginTransmission(MAX9744_I2CADDR);
  Wire.write(v);
  if (Wire.endTransmission() == 0) 
  {
    //Serial.println("true");
    return true;
  }
  else
  {
  //     Serial.println("false");
       return false;
  }
}
  
