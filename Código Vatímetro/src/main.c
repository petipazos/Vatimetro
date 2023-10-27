//APAZA ADOLFO
//CHOQUE AARON
//PAZOS ALDANA

//TIM 3 TRGO
//DMA CANAL 0
//EL TIMER SE UTILIZA PARA DISPARAR EL ADC, EN UN INTERVALO REGULAR
//PD15 BOTÓN MANEJO DE INTERFÁZ

//PINES DEL ADC
//PC1 tension
//PC0 corriente

//Includes

#include <stm32f4xx_tim.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_adc.h>
#include <stm32f4xx_dma.h>
#include <misc.h>
#include <math.h>
#include "arm_math.h"
#include <string.h>
#include "stdio_ext.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32_ub_lcd_2x16.h"
#include "logos.h"


//Defines

#define PULSADOR 	GPIO_Pin_15
#define	MUESTRAS	250						//Muestras tomadas por ciclo
#define CICLOS		4						//Cantidad de ciclos
#define N			(MUESTRAS * CICLOS) 	//Cantidad de muestras que se tomarán para cada medición 250*4
#define VREF		2.94
#define MAX_ADC		4095.0
#define V_SCALE		246
#define I_SCALE		6.7


//Funciones a utilizar

void TIM_Config(void); 					//Configuración de timers
void Delay(int time);					//Demora seleccionable
void ADC_Config(void);					//Configuración ADC
void DMA_Config(void);					//Configuración DMA
void NVIC_Config();						//Configuración interrupcion DMA2 TC
void PULS_Config(void);					//Configuracion PA0
void Menu(void);						//Mostrar datos



//Variables de medición

float32_t P=0,Pq=0,Ps=0,FP=0;	//Potencias y factor de potencia
float32_t Vef=0,Ief=0; 			//Valores eficaces de las señales
int a1=0,a2=0;					//Auxiliares al sprintf


//Variables del ADC y DMA

uint16_t DMA[2*N]={0};
int muestreado=0;

//Variables de control

float32_t V[N];		//Vector de tensión: 1000 muestras tomadas       -->    250 muestras por cada ciclo
float32_t I[N];		//Vector de corriente: 1000 muestras tomadas
float32_t VAC[N];	//Vector de tensión sin el offset de entrada
float32_t IAC[N];	//Vector de corriente sin el offset de entrada
float32_t Vinst[N];	//Vector de tensión escalado al valor real
float32_t Iinst[N];	//Vector de corriente escalado al valor real
float32_t Vmed=0.0;	//Valor medio de tensión
float32_t Imed=0.0;	//Valor medio de corriente
uint32_t i=0; 		//Variable auxiliar
uint32_t t=0;
uint32_t tecla=1;	//Variable de control de menu


//Cadenas de caracteres

char strLCD[16];
char Vchar[16];
char Ichar[16];
char Pchar[16];
char Qchar[16];
char Schar[16];
char FPchar[16];


int main(void)
{

	// Inicializamos el Display
	UB_LCD_2x16_Init();	// Inicializamos el Display

	TIM_Config();	//Activamos la configuración de timers
	ADC_Config();	//Configuracion del ADC
	DMA_Config();	//Configuracion de DMA
	PULS_Config();	//Configuracion de PA0
	NVIC_Config();	//Configuracion de la interrupcion

	logoUTN_3x2(6);
	Delay(30000000);
	UB_LCD_2x16_Clear();

	UB_LCD_2x16_String(3,0,"Comision 6");
	UB_LCD_2x16_String(1,1,"Aldana Pazos");
	logo_mujer(14, 1, 3);
	Delay(30000000);

	UB_LCD_2x16_Clear();
	UB_LCD_2x16_String(1,0,"Aaron Choque");
	logo_varon(14, 0, 4);
	UB_LCD_2x16_String(1,1,"Adolfo Apaza");
	logo_varon(14, 1, 4);
	Delay(30000000);
	UB_LCD_2x16_Clear();


	while (1)
	{
		if(muestreado) //muestreado==1 implica el inicio de hacer calculos
		{
			muestreado=0;

			//Paso los valores obtenidos del ADC almacenados en en DMA, a un arreglo de tension y otro de corriente
			for(i=0;i<N;i++)
			{
				V[i] = DMA[2*i]*VREF/MAX_ADC;		//las celdas pares corresponden a la tension
				I[i] = DMA[2*i+1]*VREF/MAX_ADC;		// las celdas impares corriente del toroide
			}
			//Cálculo de los valores medio de tensión y corriente
			arm_mean_f32(V,N,&Vmed);			//se utiliza para hallar el valor medio y luego con éste quitar el offset
			arm_mean_f32(I,N,&Imed);

			//Se elimina el offset de los vectores de tensión y corriente
			arm_offset_f32(V,-Vmed,VAC,N);			//utilizando el valor medio hallado anteriormente, le quito el offset a cada valor que se encuentra
			arm_offset_f32(I,-Imed,IAC,N);			//en el arreglo

			//Se escalan los vectores a los valores reales
			arm_scale_f32(VAC, V_SCALE, Vinst, N);		//para realizar los correctos calculos, es necesario escalarlos
			arm_scale_f32(IAC, I_SCALE, Iinst, N);		// ya que en VAC y en IAC hay valores entre -1.5 a 1.5 aprox


			//Potencia Activa
			arm_dot_prod_f32(Vinst,Iinst,N,&P);
			P=P/N;
			//Tensión y corriente eficaz
			arm_rms_f32(Vinst,N,&Vef);
			arm_rms_f32(Iinst,N,&Ief);


			//Potenica aparente
			Ps = Vef*Ief;
			//Potencia reactiva
			Pq = sqrt(Ps*Ps-P*P);
			//Factor de potencia
			FP = P/Ps;




			//Mostrar resultados en el display
			Menu();


			//una vez realizado todos los calculos, se vuelve a activar el timer para la adquisicion de datos
			TIM_Cmd(TIM3, ENABLE);


			if (!GPIO_ReadInputDataBit(GPIOD, PULSADOR)) // boton utilizado para ir variando el menu
			{
				Delay (200000);
				if(!GPIO_ReadInputDataBit(GPIOD, PULSADOR))
				{
					tecla++;
					if(tecla==4)
						tecla=1;
					while (!GPIO_ReadInputDataBit(GPIOD, PULSADOR));
				}
			}
		}

		//se hacen cero para no afectar los calculos
		P=0;
		Ps=0;
		Pq=0;
		FP=0;

		//		sprintf(Vchar, "%1.3lf",Vmed);
		//		UB_LCD_2x16_String(0,0,"Vmedio:            ");
		//		UB_LCD_2x16_String(8,0,Vchar);
		//
		//		sprintf(Ichar, "%1.3lf",Imed);
		//		UB_LCD_2x16_String(0,1,"Imedio:            ");
		//		UB_LCD_2x16_String(8,1,Ichar);
		//		Delay(1000000);
		//		UB_LCD_2x16_Clear();

	}
}

void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t PrescalerValue = 0;
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//*******************************************************************************
	// Resumen de formulas a utilizar:

	// TIM2 counter clock (TIM2CK_CNT)= TIM2CLK/(PrescalerValue+1)

	// PrescalerValue = ((TIM2CLK) / TIM2CK_CNT) - 1;
	// PrescalerValue = ((SystemCoreClock / 2) / TIM2CK_CNT) - 1;

	// Tiempo entre interrupciones (Tei) = TIM_Period * PrescalerValue + 1
	//												    ------------------
	//										                 TIM2CLK
	//*******************************************************************************

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 200000) - 1; 	// TIM3=SystemCoreClock/2=84MHz -> Prescaler=419 -> 200kHz

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (200000/(50*MUESTRAS))-1;		// y TIM_Period 15 --> 12,5kHz para la salida de este timer
	// FRECUENCIA DE MUESTREO 12.5kHz ---> 250 muestras * 50Hz
	//Receta
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	//Se elige como salida el timer que activa el ADC1
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

/* Configuración de las interrupciones */
void NVIC_Config()
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configuración interrupción cuando se llena el buffer del DMA*/
	/* Enable and set DMA2 Stream0 Interrupt to the lower priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC , ENABLE);	//TC: Transfer Complete
}

void ADC_Config(void)
{
	//Configuracion de PINES
	GPIO_InitTypeDef      GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0 | GPIO_Pin_1; //PC0 y PC1
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Conficuracion ADC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 							//Resolucion 12 bits
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;									//Scan Convertion Mode habilitado
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//Conversión continua deshabilitada
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;	//Flanco de disparo ascendente
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;			//TIM 3 TRGO como disparo de conversión
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//Código de medición alineado a la derecha
	ADC_InitStructure.ADC_NbrOfConversion = 2;										//Dos conversiones por disparo: canal 10 y 11
	ADC_Init(ADC1, &ADC_InitStructure);

	// Configuración común del ADC

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//Modo independiente
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//Prescaler en 2
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;				//DMA AccessMode deshabilitado
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//Delay entre 2 muestreos de 5 ciclos
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 regular channels 10, 11 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_15Cycles); //PC1 (Tensión)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_15Cycles); //PC0 (Corriente)

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);	//habilitar requerimiento de DMA luego de cada conversiÃ³n -> Luego de convertir pide un requerimiento de DMA
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}


void DMA_Config (void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;									//Se elige el canal 0
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DMA;							//Los datos se guardarán en DMA
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;					//Los datos se toman del ADC
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;							//Los datos van del periférico (ADC) a la memoria
	DMA_InitStructure.DMA_BufferSize = 2*N;											//Tamaño del buffer es de 2N: 2000 muestras
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//Deshabilitado
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//Incrementa el puntero de la memoria luego de escribir un dato
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		//Tamaño de los datos de 32 bits
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									//Modo circular
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//Prioridad alta
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;							//FIFO habilitado
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	/* Enable Transfer Complete interrupt */
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	/* DMA2_Stream0 enable */
	DMA_Cmd(DMA2_Stream0, ENABLE);
}

/* Handler de interrupción por DMA2 Stream 0 */
void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		//Cuando se llena el bufferDMA se para la adquisición de muestras y se procede a calcular
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
		TIM_Cmd(TIM3, DISABLE);
		muestreado = 1;
	}

}

void PULS_Config(void){

	/* Configuramos las entradas a interrupciones */
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Habilita el reloj GPIOC */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	/* Habilita el reloj SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configura PC6 como entrada */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}



// Selector de Menu
void Menu (void)
{
	//UB_LCD_2x16_Clear();
	if(tecla==1)
	{
		sprintf(Vchar, "%.0lf V",Vef);
		UB_LCD_2x16_String(0,0,"Vef:            ");
		UB_LCD_2x16_String(5,0,Vchar);

		UB_LCD_2x16_String(0,1,"Ief:            ");
		if(Ief>=1)
		{
			sprintf(Ichar, "%.2lf A",Ief);
			UB_LCD_2x16_String(5,1,Ichar);
		}
		else
		{
			Ief=Ief*1000;
			sprintf(Ichar, "%.0lf mA",Ief);
			UB_LCD_2x16_String(5,1,Ichar);
		}
	}
	if(tecla==2)
	{

		UB_LCD_2x16_String(0,0,"Ps:        ");
		sprintf(Schar, "%.1lf VA",Ps);

		UB_LCD_2x16_String(5,0,Schar);

		UB_LCD_2x16_String(0,1,"Pq:      ");
		sprintf(Qchar, "%.1lf VAr",Pq);

		UB_LCD_2x16_String(5,1,Qchar);
	}
	if(tecla==3)
	{
		logo_rayo_lleno (0,0, 0);
		UB_LCD_2x16_String(1,0," P :              ");
		sprintf(Pchar, "%.1lf W",P);
		UB_LCD_2x16_String(7,0,Pchar);

		UB_LCD_2x16_String(0,1,"Cos :           ");
		logo_coseno_phi (3,1,1);
		sprintf(FPchar, "%1.2lf",FP);
		UB_LCD_2x16_String(7,1,FPchar);
	}

}

void Delay(int time)
{
	while(time)
		time--;
}
