/*
 * logos.c
 *
 *  Created on: 24 sep. 2020
 *      Author: Adolfo
* Para el correcto funcionamiento deben estar incluidas las siguientes librerias:
 *
 * stm32_ub_lcd_2x16.h
 * stm32_ub_lcd_2x16.c
 *
 */

//Librerias:
#include "stm32_ub_lcd_2x16.h"

//Variables:
uint8_t buff[8]="/0";

//Prototipo de funciones:
void logo_panelSolar(int x, int y,int simbolo); //caracter del panel solar
void logo_bateria_llena (int x, int y,int simbolo);  //caracter de la bateria
void logo_rayo_lleno (int x,int y, int simbolo);	//caracter del rayo
void logoUTN_2x2(int x);								//logo UTN de 2x2 caracteres
void logoUTN_3x2(int x);								//logo UTN de 3x3 caracteres
void logo_eficaz (int x,int y, int simbolo);			//logo valor eficaz
void logo_rayo_vacio (int x,int y, int simbolo);		//logo rayo vacio
void logo_iman (int x,int y, int simbolo);				//logo de iman iman
void logo_coseno_phi (int x,int y, int simbolo);		//logo coseno de fi
void logo_mujer (int x, int y, int simbolo);
void logo_varon (int x, int y, int simbolo);


//Funciones
void logo_panelSolar(int x, int y,int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x1F;
	buff[1]=0x15;
	buff[2]=0x1F;
	buff[3]=0x15;
	buff[4]=0x1F;
	buff[5]=0x15;
	buff[6]=0x1F;
	buff[7]=0x04;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}
void logo_bateria_llena (int x, int y,int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x0E;
	buff[1]=0x1B;
	buff[2]=0x11;
	buff[3]=0x11;
	buff[4]=0x1F;
	buff[5]=0x1F;
	buff[6]=0x1F;
	buff[7]=0x1F;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}
void logo_rayo_lleno (int x,int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x01;
	buff[1]=0x02;
	buff[2]=0x06;
	buff[3]=0x0F;
	buff[4]=0x1E;
	buff[5]=0x0C;
	buff[6]=0x08;
	buff[7]=0x10;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}
void logoUTN_2x2(int x) //toma los valores 0,1,2,3 de simbolos para este logo
{
	uint8_t buff[8];

	buff[0]=0x19;
	buff[1]=0x19;
	buff[2]=0x19;
	buff[3]=0x1D;
	buff[4]=0x0D;
	buff[5]=0x0F;
	buff[6]=0x07;
	buff[7]=0x1F;

	UB_LCD_2x16_WriteCG(0,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x,0,0);

	buff[0]=0x1F;
	buff[1]=0x07;
	buff[2]=0x0F;
	buff[3]=0x0D;
	buff[4]=0x1D;
	buff[5]=0x19;
	buff[6]=0x19;
	buff[7]=0x19;

	UB_LCD_2x16_WriteCG(1,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x,1,1);

	buff[0]=0x13;
	buff[1]=0x13;
	buff[2]=0x13;
	buff[3]=0x17;
	buff[4]=0x16;
	buff[5]=0x1E;
	buff[6]=0x1C;
	buff[7]=0x1F;

	UB_LCD_2x16_WriteCG(2,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+1,0,2);

	buff[0]=0x1F;
	buff[1]=0x1C;
	buff[2]=0x1E;
	buff[3]=0x16;
	buff[4]=0x17;
	buff[5]=0x13;
	buff[6]=0x13;
	buff[7]=0x13;

	UB_LCD_2x16_WriteCG(3,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+1,1,3);
}
void logoUTN_3x2(int x) //toma los valores 0,1,2,3,4 y 5 de simbolos para este logo
{
	uint8_t buff[8];

	buff[0]=0x0E;
	buff[1]=0x0E;
	buff[2]=0x0E;
	buff[3]=0x07;
	buff[4]=0x03;
	buff[5]=0x01;
	buff[6]=0x00;
	buff[7]=0x0F;

	UB_LCD_2x16_WriteCG(0,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x,0,0);

	buff[0]=0x0F;
	buff[1]=0x00;
	buff[2]=0x01;
	buff[3]=0x03;
	buff[4]=0x07;
	buff[5]=0x0E;
	buff[6]=0x0E;
	buff[7]=0x0E;

	UB_LCD_2x16_WriteCG(1,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x,1,1);

	buff[0]=0x0E;
	buff[1]=0x0E;
	buff[2]=0x0E;
	buff[3]=0x0E;
	buff[4]=0x0E;
	buff[5]=0x1F;
	buff[6]=0x1F;
	buff[7]=0x1F;

	UB_LCD_2x16_WriteCG(2,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+1,0,2);

	buff[0]=0x1F;
	buff[1]=0x1F;
	buff[2]=0x1F;
	buff[3]=0x0E;
	buff[4]=0x0E;
	buff[5]=0x0E;
	buff[6]=0x0E;
	buff[7]=0x0E;

	UB_LCD_2x16_WriteCG(3,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+1,1,3);

	buff[0]=0x0E;
	buff[1]=0x0E;
	buff[2]=0x0E;
	buff[3]=0x1C;
	buff[4]=0x18;
	buff[5]=0x10;
	buff[6]=0x00;
	buff[7]=0x1E;

	UB_LCD_2x16_WriteCG(4,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+2,0,4);

	buff[0]=0x1E;
	buff[1]=0x00;
	buff[2]=0x10;
	buff[3]=0x18;
	buff[4]=0x1C;
	buff[5]=0x0E;
	buff[6]=0x0E;
	buff[7]=0x0E;

	UB_LCD_2x16_WriteCG(5,buff); // store data as char 0
	UB_LCD_2x16_PrintCG(x+2,1,5);
}

void logo_eficaz (int x,int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x04;
	buff[1]=0x04;
	buff[2]=0x15;
	buff[3]=0x15;
	buff[4]=0x11;
	buff[5]=0x0A;
	buff[6]=0x0E;
	buff[7]=0x00;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}

void logo_rayo_vacio (int x,int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x01;
	buff[1]=0x02;
	buff[2]=0x06;
	buff[3]=0x09;
	buff[4]=0x12;
	buff[5]=0x0C;
	buff[6]=0x08;
	buff[7]=0x10;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}

void logo_iman (int x,int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x01;
	buff[1]=0x02;
	buff[2]=0x06;
	buff[3]=0x09;
	buff[4]=0x12;
	buff[5]=0x0C;
	buff[6]=0x08;
	buff[7]=0x10;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}

void logo_coseno_phi (int x,int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x06;
	buff[1]=0x15;
	buff[2]=0x15;
	buff[3]=0x15;
	buff[4]=0x15;
	buff[5]=0x0E;
	buff[6]=0x04;
	buff[7]=0x04;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);

}

void logo_varon (int x, int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x0F;
	buff[1]=0x0E;
	buff[2]=0x04;
	buff[3]=0x1F;
	buff[4]=0x04;
	buff[5]=0x04;
	buff[6]=0x0A;
	buff[7]=0x11;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);
}

void logo_mujer (int x, int y, int simbolo)
{
	uint8_t buff[8];

	buff[0]=0x0E;
	buff[1]=0x0E;
	buff[2]=0x04;
	buff[3]=0x1F;
	buff[4]=0x04;
	buff[5]=0x0E;
	buff[6]=0x1F;
	buff[7]=0x0A;

	UB_LCD_2x16_WriteCG(simbolo,buff);
	UB_LCD_2x16_PrintCG(x,y,simbolo);
}

