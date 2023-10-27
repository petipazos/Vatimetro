//--------------------------------------------------------------
// File     : stm32_ub_lcd_2x16.c
// Datum    : 15.04.2013
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.0
// Module   : GPIO
// Funktion : Text-LCD Funktionen (2x16 Zeichen)
//            im 4Bit-Mode
//            Chip : ST7066U/HD44780/SED1278/KS0066U/S6A0069X
// 
// Hinweis  : EL Display utiliza los pines de la  CPU:
//             PB8  -> LCD_RS
//             PC13 -> LCD_DB7
//             PE2  -> LCD_DB6
//             PE4  -> LCD_DB4
//             PE5  -> LCD_E
//             PE6  -> LCD_DB5
//--------------------------------------------------------------



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_lcd_2x16.h"



//--------------------------------------------------------------
// Funciones internas
//--------------------------------------------------------------
void P_LCD_2x16_InitIO(void);
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin);
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin);
void P_LCD_2x16_Clk(void);
void P_LCD_2x16_InitSequenz(void);
void P_LCD_2x16_Cmd(uint8_t wert);
void P_LCD_2x16_Data(uint8_t wert);
void P_LCD_2x16_Cursor(uint8_t x, uint8_t y);
void P_LCD_2x16_Delay(volatile uint32_t nCount);
 


//--------------------------------------------------------------
// Definicion de todos los pines para el Display
// Guarda el nombre puertos en la estructura: TLCD_NAME_t, y define los puertos y su funcionamiento.
//
// Init : [Bit_SET,Bit_RESET]
//--------------------------------------------------------------
LCD_2X16_t LCD_2X16[] = {
 // Name   ,PORT , PIN       , CLOCK              , Init
  {TLCD_RS ,GPIOB,GPIO_Pin_8 ,RCC_AHB1Periph_GPIOB,Bit_RESET},
  {TLCD_E  ,GPIOE,GPIO_Pin_5 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D4 ,GPIOE,GPIO_Pin_4 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D5 ,GPIOE,GPIO_Pin_6 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D6 ,GPIOE,GPIO_Pin_2 ,RCC_AHB1Periph_GPIOE,Bit_RESET},
  {TLCD_D7 ,GPIOC,GPIO_Pin_13,RCC_AHB1Periph_GPIOC,Bit_RESET},
};



//--------------------------------------------------------------
// Init vom Text-LCDisplay
//--------------------------------------------------------------
void UB_LCD_2x16_Init(void)
{
  // init aller IO-Pins
  P_LCD_2x16_InitIO();
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_INIT_PAUSE);
  // Init Sequenz starten
  P_LCD_2x16_InitSequenz();
  // LCD-Settings einstellen
  P_LCD_2x16_Cmd(TLCD_CMD_INIT_DISPLAY);
  P_LCD_2x16_Cmd(TLCD_CMD_ENTRY_MODE);
  // Display einschalten
  P_LCD_2x16_Cmd(TLCD_CMD_DISP_M1);
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// Borra todos los valores mostrados en el LCDisplay
//--------------------------------------------------------------
void UB_LCD_2x16_Clear(void)
{
  // Display l�schen
  P_LCD_2x16_Cmd(TLCD_CMD_CLEAR);
  // kleine Pause
  P_LCD_2x16_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// Stellt einen Display Mode ein
// mode : [TLCD_OFF, TLCD_ON, TLCD_CURSOR, TLCD_BLINK]
//--------------------------------------------------------------
void UB_LCD_2x16_SetMode(TLCD_MODE_t mode)
{
  if(mode==TLCD_OFF) P_LCD_2x16_Cmd(TLCD_CMD_DISP_M0);
  if(mode==TLCD_ON) P_LCD_2x16_Cmd(TLCD_CMD_DISP_M1);
  if(mode==TLCD_CURSOR) P_LCD_2x16_Cmd(TLCD_CMD_DISP_M2);
  if(mode==TLCD_BLINK) P_LCD_2x16_Cmd(TLCD_CMD_DISP_M3);
}


//--------------------------------------------------------------
// Salida de una cadena en la pantalla en la posici�n x, y
// x : 0 bis 15
// y : 0 bis 1
//--------------------------------------------------------------
void UB_LCD_2x16_String(uint8_t x, uint8_t y, char *ptr)
{
  // Colocar el cursor
  P_LCD_2x16_Cursor(x,y);
  // completar String de salida
  while (*ptr != 0) {
    P_LCD_2x16_Data(*ptr);
    ptr++;
  }
}
//--------------------------------------------------------------
// Speichern eines Sonderzeichens im CG-RAM vom Display
// nr : 0 bis 7 (nummer der sonderzeichens)
// pixeldata : 8 bytes mit Pixeldaten f�r das Zeichen
//--------------------------------------------------------------
void UB_LCD_2x16_WriteCG(uint8_t nr, uint8_t *pixeldata)
{
  uint8_t n;

  if(nr>7) nr=7;

  nr=(nr<<3);
  nr|=0x40;
  P_LCD_2x16_Cmd(nr);
  for(n=0;n<8;n++) {
    P_LCD_2x16_Data(pixeldata[n]);
  }
}
//--------------------------------------------------------------
// Ausgabe von einem Sonderzeichen auf dem Display an x,y Position
// x : 0 bis 15
// y : 0 bis 1
// nr : 0 bis 7 (nummer der sonderzeichens)
//--------------------------------------------------------------
void UB_LCD_2x16_PrintCG(uint8_t x, uint8_t y, uint8_t nr)
{
  if(nr>7) nr=7;

  // Cursor setzen
  P_LCD_2x16_Cursor(x,y);
  P_LCD_2x16_Data(nr);
}

//--------------------------------------------------------------
// interne Funktion
// init aller IO-Pins
//--------------------------------------------------------------
void P_LCD_2x16_InitIO(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  TLCD_NAME_t lcd_pin;
  
  for(lcd_pin=0;lcd_pin<TLCD_ANZ;lcd_pin++) {
    // Clock Enable
    RCC_AHB1PeriphClockCmd(LCD_2X16[lcd_pin].TLCD_CLK, ENABLE);

    // Config als Digital-Ausgang
    GPIO_InitStructure.GPIO_Pin = LCD_2X16[lcd_pin].TLCD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_2X16[lcd_pin].TLCD_PORT, &GPIO_InitStructure);

    // Default Wert einstellen
    if(LCD_2X16[lcd_pin].TLCD_INIT==Bit_RESET) {
      P_LCD_2x16_PinLo(lcd_pin);
    }
    else {
      P_LCD_2x16_PinHi(lcd_pin);
    }
  }  
}


//--------------------------------------------------------------
// interne Funktion
// Pin auf Lo setzen
//--------------------------------------------------------------
void P_LCD_2x16_PinLo(TLCD_NAME_t lcd_pin)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRH = LCD_2X16[lcd_pin].TLCD_PIN;
}


//--------------------------------------------------------------
// interne Funktion
// Pin auf Hi setzen
//--------------------------------------------------------------
void P_LCD_2x16_PinHi(TLCD_NAME_t lcd_pin)
{
  LCD_2X16[lcd_pin].TLCD_PORT->BSRRL = LCD_2X16[lcd_pin].TLCD_PIN;
}


//--------------------------------------------------------------
// interne Funktion
// Emitir un pulso de reloj
//--------------------------------------------------------------
void P_LCD_2x16_Clk(void)
{
  // Pin-E en Hi
  P_LCD_2x16_PinHi(TLCD_E);
  // Peque�a Pausa
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);
  // Pin-E en Lo
  P_LCD_2x16_PinLo(TLCD_E);
  // Peque�a pausa
  P_LCD_2x16_Delay(TLCD_CLK_PAUSE);  
}


//--------------------------------------------------------------
// interne Funktion
// init Sequenz f�r das Display
//--------------------------------------------------------------
void P_LCD_2x16_InitSequenz(void)
{
  // Init Sequenz
  P_LCD_2x16_PinHi(TLCD_D4);
  P_LCD_2x16_PinHi(TLCD_D5);
  P_LCD_2x16_PinLo(TLCD_D6);
  P_LCD_2x16_PinLo(TLCD_D7);
  // Erster Init Impuls
  P_LCD_2x16_Clk();
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Zweiter Init Impuls
  P_LCD_2x16_Clk();
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // Dritter Init Impuls
  P_LCD_2x16_Clk();
  P_LCD_2x16_Delay(TLCD_PAUSE);
  // LCD-Modus einstellen (4Bit-Mode)
  P_LCD_2x16_PinLo(TLCD_D4);
  P_LCD_2x16_PinHi(TLCD_D5);
  P_LCD_2x16_PinLo(TLCD_D6);
  P_LCD_2x16_PinLo(TLCD_D7);
  P_LCD_2x16_Clk();
  P_LCD_2x16_Delay(TLCD_PAUSE);
}


//--------------------------------------------------------------
// interne Funktion
// Kommando an das Display senden
//--------------------------------------------------------------
void P_LCD_2x16_Cmd(uint8_t wert)
{
  // RS=Lo (Command)
  P_LCD_2x16_PinLo(TLCD_RS);
  // Hi-Nibble ausgeben         
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7); else P_LCD_2x16_PinLo(TLCD_D7);//
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6); else P_LCD_2x16_PinLo(TLCD_D6);
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5); else P_LCD_2x16_PinLo(TLCD_D5);
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4); else P_LCD_2x16_PinLo(TLCD_D4);
  P_LCD_2x16_Clk();
  // Lo-Nibble ausgeben         
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7); else P_LCD_2x16_PinLo(TLCD_D7);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6); else P_LCD_2x16_PinLo(TLCD_D6);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5); else P_LCD_2x16_PinLo(TLCD_D5);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4); else P_LCD_2x16_PinLo(TLCD_D4);
  P_LCD_2x16_Clk();  
}


//--------------------------------------------------------------
// interne Funktion
// Daten an das Display senden
//--------------------------------------------------------------
void P_LCD_2x16_Data(uint8_t wert)
{
  // RS=Hi (Data)
  P_LCD_2x16_PinHi(TLCD_RS);
  // Hi-Nibble ausgeben          
  if((wert&0x80)!=0) P_LCD_2x16_PinHi(TLCD_D7); else P_LCD_2x16_PinLo(TLCD_D7);// con un mascara compara el valor de entrada
  if((wert&0x40)!=0) P_LCD_2x16_PinHi(TLCD_D6); else P_LCD_2x16_PinLo(TLCD_D6);// y con un and logico compara bit a bit con el valor de la
  if((wert&0x20)!=0) P_LCD_2x16_PinHi(TLCD_D5); else P_LCD_2x16_PinLo(TLCD_D5);//letra entrada poniendo en 1 o 0 el pin del Display
  if((wert&0x10)!=0) P_LCD_2x16_PinHi(TLCD_D4); else P_LCD_2x16_PinLo(TLCD_D4);
  P_LCD_2x16_Clk(); // Envia a los puertos D4,D5,D6 y D7 los primeros 4 bits obtenidos anteriormente y pone el pin enable en 1 y 0.
  // Lo-Nibble ausgeben        
  if((wert&0x08)!=0) P_LCD_2x16_PinHi(TLCD_D7); else P_LCD_2x16_PinLo(TLCD_D7);
  if((wert&0x04)!=0) P_LCD_2x16_PinHi(TLCD_D6); else P_LCD_2x16_PinLo(TLCD_D6);
  if((wert&0x02)!=0) P_LCD_2x16_PinHi(TLCD_D5); else P_LCD_2x16_PinLo(TLCD_D5);
  if((wert&0x01)!=0) P_LCD_2x16_PinHi(TLCD_D4); else P_LCD_2x16_PinLo(TLCD_D4);
  P_LCD_2x16_Clk(); // Envia a los puertos D4,D5,D6 y D7 los ultimos 4 bits obtenidos anteriormente mostrando la letra de 8bits
  //y ponele en enable en 1 para que aparesca la letra en la pantalla y luego lo pone en 0.
}


//--------------------------------------------------------------
// Funcion interna
// Poner cursor en x,y
//--------------------------------------------------------------
void P_LCD_2x16_Cursor(uint8_t x, uint8_t y)
{
  uint8_t wert;

  if(x>=TLCD_MAXX) x=0;
  if(y>=TLCD_MAXY) y=0;

  wert=(y<<6);
  wert|=x;
  wert|=0x80;
  P_LCD_2x16_Cmd(wert);
}


//--------------------------------------------------------------
// Peque�a pausa (ohne Timer)
//--------------------------------------------------------------
void P_LCD_2x16_Delay(volatile uint32_t nCount)
{
  while(nCount--)
  {
  }
}
