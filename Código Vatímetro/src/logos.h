/*
 * logos.h
 *
 *  Created on: 24 sep. 2020
 *      Author: Adolfo
 * Para el correcto funcionamiento deben estar incluidas las siguientes librerias:
 *
 * stm32_ub_lcd_2x16.h
 * stm32_ub_lcd_2x16.c
 */

#include "stm32_ub_lcd_2x16.h"
#ifndef LOGOS_H_
#define LOGOS_H_

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

#endif /* LOGOS_H_ */
