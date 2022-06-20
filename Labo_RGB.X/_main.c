/************************************
 * Name :     main.c
 * Author:    Jp Gouin
 * Version :  h2022
 ************************************/

#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include <string.h>

// Since the flag is changed within an interrupt, we need the keyword volatile.
static volatile int timer_1m = 0;
void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1ISR(void) {  
   timer_1m = 1;           //    Indique à la boucle principale qu'on doit traiter
   IFS0bits.T1IF = 0;      //    clear interrupt flag
}
#define TMR_TIME    0.001             // x us for each tick
void initialize_timer_interrupt(void) { 
  T1CONbits.TCKPS = 3;                //    256 prescaler value
  T1CONbits.TGATE = 0;                //    not gated input (the default)
  T1CONbits.TCS = 0;                  //    PCBLK input (the default)
  PR1 = (int)(((float)(TMR_TIME * PB_FRQ) / 256) + 0.5);   //set period register, generates one interrupt every 1 ms
                                      //    48 kHz * 1 ms / 256 = 188
  TMR1 = 0;                           //    initialize count to 0
  IPC1bits.T1IP = 2;                  //    INT step 4: priority
  IPC1bits.T1IS = 0;                  //    subpriority
  IFS0bits.T1IF = 0;                  //    clear interrupt flag
  IEC0bits.T1IE = 1;                  //    enable interrupt
  T1CONbits.ON = 1;                   //    turn on Timer5
} 
void init(){
    LCD_Init();
    RGBLED_Init();
    SWT_Init();
    LCD_DisplayClear();
    LED_Init();
    initialize_timer_interrupt();
    macro_enable_interrupts();
}

void main() {
    init();
    int count_1s = 0;
    int seconde = 0;  
    
    // Variables fournies
    int RGB = 0;
    int intensite = 255;
    char text[16];

    // Main loop
    while(1) {
        if(timer_1m) {               // Interruption à chaque 1 ms
            timer_1m = 0;            // Reset the compteur to capture the next event
            if (++count_1s >= 1000) {
                count_1s = 0;
                seconde++;
                
                LCD_CLEAR();  // effacer LCD
                                
                if (SWT_GetValue(0) == 0) {
                    intensite += 50;
                    if (intensite > 255) {
                        intensite = 0;
                    }
                    LCD_WriteStringAtPos("Rouge", 0, 0);
                    sprintf(text,"%d",intensite);
                    LCD_WriteStringAtPos(text, 1, 0);
                    // AFFICHER LA DEL ROUGE
                    
                }
                
                if (SWT_GetValue(0) == 1) {    
                    if (RGB == 0) {
                        LCD_WriteStringAtPos("Rouge", 0, 0);
                        sprintf(text,"%d",intensite);
                        LCD_WriteStringAtPos(text, 1, 0);
                        RGB = 1;
                        // AFFICHER LA DEL ROUGE
                        
                    }
                    else if (RGB == 1) {
                        LCD_WriteStringAtPos("Verte", 0, 0);
                        sprintf(text,"%d",intensite);
                        LCD_WriteStringAtPos(text, 1, 0);
                        RGB = 2;
                        // AFFICHER LA DEL VERTE
                        
                    }
                    else if (RGB == 2) {
                        LCD_WriteStringAtPos("Bleue", 0, 0);
                        sprintf(text,"%d",intensite);
                        LCD_WriteStringAtPos(text, 1, 0);
                        RGB = 3;
                        // AFFICHER LA DEL BLEUE
                        
                    }                
                    else {
                        RGBLED_SetValue(intensite, intensite, intensite);
                        LCD_WriteStringAtPos("Blanche", 0, 0);
                        sprintf(text,"%d",intensite);
                        LCD_WriteStringAtPos(text, 1, 0);
                        RGB = 0;
                        // AFFICHER LA DEL BLANCHE
                        
                    }
                }
            }
        }
    }
}


