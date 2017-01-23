/****************************************************************************
 * timer.ino
 * Timer 4 setup and interrupt functions
 * Mike Hord @ SparkFun Electronics
 * 23 Jan 2017
 * https://github.com/sparkfun/Spectacle_Director_Board
 * 
 * Timer 4 is used to provide an independent blink mechanism for the LED on the
 * director board, so even if the processor gets hung in some section of code,
 * the error code can be displayed on the LED.
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/


// The author wishes to recognize arduino.cc forum user MartinL for providing
//  the example code used below for configuration and interrupt handling.
// See https://forum.arduino.cc/index.php?topic=332275.15

// Set up the GCLK resource and TC4 resource to provide an interrupt 1000 times
//  per second.
void setupTimer() 
{
   // Set up the generic clock (GCLK5) used to clock timers
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(48000) |      // Divide the 48MHz clock source by divisor 48000: 48MHz/48000=1kHz
                    GCLK_GENDIV_ID(5);            // Select Generic Clock (GCLK) 5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK5
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                     GCLK_GENCTRL_ID(5);          // Select GCLK5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK4 to TC4 and TC5
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK5 to TC4 and TC5
                     GCLK_CLKCTRL_GEN_GCLK5 |     // Select GCLK5
                     GCLK_CLKCTRL_ID_TC4_TC5;     // Feed the GCLK5 to TC4 and TC5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  
  REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT8;          // Set the counter to 8-bit mode
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization 

  REG_TC4_COUNT8_CC0 = 00;                        // Set the TC4 CC0 register to some arbitary value
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
  REG_TC4_COUNT8_CC1 = 0xAA;                      // Set the TC4 CC1 register to some arbitary value
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
  REG_TC4_COUNT8_PER = 200;                       // Set the PER (period) register to its maximum value
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization

  NVIC_SetPriority(TC4_IRQn, 2);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 2
  NVIC_EnableIRQ(TC4_IRQn);         // Connect TC4 to Nested Vector Interrupt Controller (NVIC) 

  REG_TC4_INTFLAG |= TC_INTFLAG_MC1 | TC_INTFLAG_MC0 | TC_INTFLAG_OVF;        // Clear the interrupt flags
  REG_TC4_INTENSET = TC_INTENSET_MC0;     // Enable TC4 MC0 interrupt
  
  REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV1  |     // Set prescaler to 1, 1kHz/1 = 1kHz
                   TC_CTRLA_ENABLE;               // Enable TC4
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);        // Wait for synchronization
}

void TC4_Handler()              // Interrupt Service Routine (ISR) for timer TC4
{     
  // Check for match counter 0 (MC0) interrupt
  if (TC4->COUNT8.INTFLAG.bit.MC0 && TC4->COUNT8.INTENSET.bit.MC0)             
  {
    // We're really only going to do something twice per second.
    if (++shortTick > 500)
    {
      shortTick = 0;                      // reset our secondary counter
      // This if/else implements the error code blinking functionality.
      //  Until blinkNum is reached, blink the LED at a 1Hz rate. When
      //  blinkNum cycles have passed, turn the LED off for another 4
      //  cycles (2 seconds). Then, start over.
      if (++blinkPause < blinkNum)        
      {
        if (digitalRead(7) == LOW) digitalWrite(7, HIGH);
        else                       digitalWrite(7, LOW);
      }
      else if (blinkPause < blinkNum + 4)
      {
        digitalWrite(7, HIGH);
      }
      else
      {
        blinkPause = 0;
      }
    }
    
  REG_TC4_INTFLAG = TC_INTFLAG_MC0;         // Clear the MC0 interrupt flag 
  while (TC4->COUNT8.STATUS.bit.SYNCBUSY);      // Wait for synchronization
  }
}
