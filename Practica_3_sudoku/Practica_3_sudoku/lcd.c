/*********************************************************************************************
* Fichero:	lcd.c
* Autor:	
* Descrip:	funciones de visualizacion y control LCD
* Version:	<P6-ARM>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "Bmp.h"
#include "sudoku_2021.h"
#include "timer1.h"

/*--- definicion de macros ---*/
#define DMA_Byte  (0)
#define DMA_HW    (1)
#define DMA_Word  (2)
#define DW 		  DMA_Byte		//configura  ZDMA0 como media palabras
	
/*--- variables externas ---*/
extern INT8U g_auc_Ascii8x16[];
extern INT8U g_auc_Ascii6x8[];
extern STRU_BITMAP Stru_Bitmap_gbMouse;

/*  CONSTANTES  */
const static int dimension_cuadricula = 218;
const static int separacion = 22;
const static int inicioTablero_izquierda = 20;
const static int inicioTablero_arriba = 20;
const static int marIzq_num = 28;
const static int marSup_num = 24;
static int tiempo_partida;
const INT8U *numeros[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };


/*--- codigo de la funcion ---*/
void Lcd_Init(void)
{       
	rDITHMODE=0x1223a;
	rDP1_2 =0x5a5a;      
	rDP4_7 =0x366cd9b;
	rDP3_5 =0xda5a7;
	rDP2_3 =0xad7;
	rDP5_7 =0xfeda5b7;
	rDP3_4 =0xebd7;
	rDP4_5 =0xebfd7;
	rDP6_7 =0x7efdfbf;

	rLCDCON1=(0)|(1<<5)|(MVAL_USED<<7)|(0x0<<8)|(0x0<<10)|(CLKVAL_GREY16<<12);
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21); 
	rLCDSADDR1= (0x2<<27) | ( ((LCD_ACTIVE_BUFFER>>22)<<21 ) | M5D(LCD_ACTIVE_BUFFER>>1));
 	rLCDSADDR2= M5D(((LCD_ACTIVE_BUFFER+(SCR_XSIZE*LCD_YSIZE/2))>>1)) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/4) | ( ((SCR_XSIZE-LCD_XSIZE)/4)<<9 );
	// enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_GREY16<<12);
	rBLUELUT=0xfa40;
	//Enable LCD Logic and EL back-light.
	rPDATE=rPDATE&0x0e;
	
	//DMA ISR
	rINTMSK &= ~(BIT_GLOBAL|BIT_ZDMA0);
    pISR_ZDMA0=(int)Zdma0Done;
}

/*********************************************************************************************
* name:		Lcd_Active_Clr()
* func:		clear LCD screen
* para:		none 
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Active_Clr(void)
{
	INT32U i;
	INT32U *pDisp = (INT32U *)LCD_ACTIVE_BUFFER;
	
	for( i = 0; i < (SCR_XSIZE*SCR_YSIZE/2/4); i++ )
	{
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
* name:		Lcd_GetPixel()
* func:		Get appointed point's color value
* para:		usX,usY -- pot's X-Y coordinate 
* ret:		pot's color value
* modify:
* comment:		
*********************************************************************************************/
INT8U LCD_GetPixel(INT16U usX, INT16U usY)
{
	INT8U ucColor;

	ucColor = *((INT8U*)(LCD_VIRTUAL_BUFFER + usY*SCR_XSIZE/2 + usX/8*4 + 3 - (usX%8)/2));
	ucColor = (ucColor >> ((1-(usX%2))*4)) & 0x0f;
	return ucColor;
}

/*********************************************************************************************
* name:		Lcd_Active_Clr()
* func:		clear virtual screen
* para:		none 
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Clr(void)
{
	INT32U i;
	INT32U *pDisp = (INT32U *)LCD_VIRTUAL_BUFFER;
	
	for( i = 0; i < (SCR_XSIZE*SCR_YSIZE/2/4); i++ )
	{
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
* name:		LcdClrRect()
* func:		fill appointed area with appointed color
* para:		usLeft,usTop,usRight,usBottom -- area's rectangle acme coordinate
*			ucColor -- appointed color value
* ret:		none
* modify:
* comment:	also as clear screen function 
*********************************************************************************************/
void LcdClrRect(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom, INT8U ucColor)
{
	INT16 i,k,l,m;
	
	INT32U ulColor = (ucColor << 28) | (ucColor << 24) | (ucColor << 20) | (ucColor << 16) | 
				     (ucColor << 12) | (ucColor << 8) | (ucColor << 4) | ucColor;

	i = k = l = m = 0;	
	if( (usRight-usLeft) <= 8 )
	{
		for( i=usTop; i<=usBottom; i++)
		{
			for( m=usLeft; m<=usRight; m++)
			{
				(LCD_PutPixel(m, i, ucColor));
			}
		}	
		return;
	}

	/* check borderline */
	if( 0 == (usLeft%8) )
		k=usLeft;
	else
	{
		k=(usLeft/8)*8+8;
	}
	if( 0 == (usRight%8) )
		l= usRight;
	else
	{
		l=(usRight/8)*8;
	}

	for( i=usTop; i<=usBottom; i++ )
	{
		for( m=usLeft; m<=(k-1); m++ )
		{
			(LCD_PutPixel(m, i, ucColor));
		}
		for( m=k; m<l; m+=8 )
		{
			(*(INT32U*)(LCD_VIRTUAL_BUFFER + i * SCR_XSIZE / 2 + m / 2)) = ulColor;
		}
		for( m=l; m<=usRight; m++ )
		{
			(LCD_PutPixel(m, i, ucColor));
		}
	}
}

/*********************************************************************************************
* name:		Lcd_Draw_Box()
* func:		Draw rectangle with appointed color
* para:		usLeft,usTop,usRight,usBottom -- rectangle's acme coordinate
*			ucColor -- appointed color value
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_Box(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom, INT8U ucColor)
{
	Lcd_Draw_HLine(usLeft, usRight,  usTop,    ucColor, 1);
	Lcd_Draw_HLine(usLeft, usRight,  usBottom, ucColor, 1);
	Lcd_Draw_VLine(usTop,  usBottom, usLeft,   ucColor, 1);
	Lcd_Draw_VLine(usTop,  usBottom, usRight,  ucColor, 1);
}

/*********************************************************************************************
* name:		Lcd_Draw_Line()
* func:		Draw line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usX1,usY1 -- line's end point coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_Line(INT16 usX0, INT16 usY0, INT16 usX1, INT16 usY1, INT8U ucColor, INT16U usWidth)
{
	INT16 usDx;
	INT16 usDy;
	INT16 y_sign;
	INT16 x_sign;
	INT16 decision;
	INT16 wCurx, wCury, wNextx, wNexty, wpy, wpx;

	if( usY0 == usY1 )
	{
		Lcd_Draw_HLine (usX0, usX1, usY0, ucColor, usWidth);
		return;
	}
	if( usX0 == usX1 )
	{
		Lcd_Draw_VLine (usY0, usY1, usX0, ucColor, usWidth);
		return;
	}
	usDx = abs(usX0 - usX1);
	usDy = abs(usY0 - usY1);
	if( ((usDx >= usDy && (usX0 > usX1)) ||
        ((usDy > usDx) && (usY0 > usY1))) )
    {
        GUISWAP(usX1, usX0);
        GUISWAP(usY1, usY0);
    }
    y_sign = (usY1 - usY0) / usDy;
    x_sign = (usX1 - usX0) / usDx;

    if( usDx >= usDy )
    {
        for( wCurx = usX0, wCury = usY0, wNextx = usX1,
             wNexty = usY1, decision = (usDx >> 1);
             wCurx <= wNextx; wCurx++, wNextx--, decision += usDy )
        {
            if( decision >= usDx )
            {
                decision -= usDx;
                wCury += y_sign;
                wNexty -= y_sign;
            }
            for( wpy = wCury - usWidth / 2;
                 wpy <= wCury + usWidth / 2; wpy++ )
            {
                (LCD_PutPixel(wCurx, wpy, ucColor));
            }

            for( wpy = wNexty - usWidth / 2;
                 wpy <= wNexty + usWidth / 2; wpy++ )
            {
                (LCD_PutPixel(wNextx, wpy, ucColor));
            }
        }
    }
    else
    {
        for( wCurx = usX0, wCury = usY0, wNextx = usX1,
             wNexty = usY1, decision = (usDy >> 1);
             wCury <= wNexty; wCury++, wNexty--, decision += usDx )
        {
            if( decision >= usDy )
            {
                decision -= usDy;
                wCurx += x_sign;
                wNextx -= x_sign;
            }
            for( wpx = wCurx - usWidth / 2;
                 wpx <= wCurx + usWidth / 2; wpx++ )
            {
                (LCD_PutPixel(wpx, wCury, ucColor));
            }

            for( wpx = wNextx - usWidth / 2;
                 wpx <= wNextx + usWidth / 2; wpx++ )
            {
                (LCD_PutPixel(wpx, wNexty, ucColor));
            }
        }
    }
}

/*********************************************************************************************
* name:		Lcd_Draw_HLine()
* func:		Draw horizontal line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usX1 -- line's end point X-coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_HLine(INT16 usX0, INT16 usX1, INT16 usY0, INT8U ucColor, INT16U usWidth)
{
	INT16 usLen;

    if( usX1 < usX0 )
    {
        GUISWAP (usX1, usX0);
    }

    while( (usWidth--) > 0 )
    {
        usLen = usX1 - usX0 + 1;
        while( (usLen--) > 0 )
        {
        	(LCD_PutPixel(usX0 + usLen, usY0, ucColor));
        }
        usY0++;
    }
}

/*********************************************************************************************
* name:		Lcd_Draw_VLine()
* func:		Draw vertical line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usY1 -- line's end point Y-coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_VLine (INT16 usY0, INT16 usY1, INT16 usX0, INT8U ucColor, INT16U usWidth)
{
	INT16 usLen;

    if( usY1 < usY0 )
    {
        GUISWAP (usY1, usY0);
    }

    while( (usWidth--) > 0 )
    {
        usLen = usY1 - usY0 + 1;
        while( (usLen--) > 0 )
        {
        	(LCD_PutPixel(usX0, usY0 + usLen, ucColor));
        }
        usX0++;
    }
}

void Lcd_DisplayString(INT16U usX0, INT16U usY0, INT8U *pucStr){

}

/*********************************************************************************************
* name:		Lcd_DspAscII8x16()
* func:		display 8x16 ASCII character string 
* para:		usX0,usY0 -- ASCII character string's start point coordinate
*			ForeColor -- appointed color value
*			pucChar   -- ASCII character string
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_DspAscII8x16(INT16U x0, INT16U y0, INT8U ForeColor, INT8U * s)
{
	INT16 i,j,k,x,y,xx;
	INT8U qm;
	INT32U ulOffset;
	INT8 ywbuf[16],temp[2];
    
	for( i = 0; i < strlen((const char*)s); i++ )
	{
		if( (INT8U)*(s+i) >= 161 )
		{
			temp[0] = *(s + i);
			temp[1] = '\0';
			return;
		}
		else
		{
			qm = *(s+i);
			ulOffset = (INT32U)(qm) * 16;		//Here to be changed tomorrow
			for( j = 0; j < 16; j ++ )
			{
				ywbuf[j] = g_auc_Ascii8x16[ulOffset + j];
            }

            for( y = 0; y < 16; y++ )
            {
            	for( x = 0; x < 8; x++ ) 
               	{
                	k = x % 8;
			    	if( ywbuf[y]  & (0x80 >> k) )
			       	{
			       		xx = x0 + x + i*8;
			       		(LCD_PutPixel(xx, y + y0, (INT8U)ForeColor));
			       	}
			   	}
            }
		}
	}
}

/*********************************************************************************************
* name:		Lcd_DspAscII6x8()
* func:		display 6x8 ASCII character string
* para:		usX0,usY0 -- ASCII character string's start point coordinate
*			ForeColor -- appointed color value
*			pucChar   -- ASCII character string
* ret:		none
* modify:
* comment:
*********************************************************************************************/
void Lcd_DspAscII6x8(INT16U usX0, INT16U usY0,INT8U ForeColor, INT8U* pucChar)
{
	INT32U i,j;
	INT8U  ucTemp;

	while( *pucChar != 0 )
	{
		for( i=0; i < 8; i++ )
		{
  			ucTemp = g_auc_Ascii6x8[(*pucChar) * 8 + i];
  			for( j = 0; j < 8; j++ )
  			{
  				if( (ucTemp & (0x80 >> j)) != 0 )
  				{
  					LCD_PutPixel(usX0 + i, usY0 + 8 - j, (INT8U)ForeColor);
  				}
  			}
		}
		usX0 += XWIDTH;
		pucChar++;
	}
}


/*********************************************************************************************
* name:		ReverseLine()
* func:		Reverse display some lines 
* para:		ulHeight -- line's height
*			ulY -- line's Y-coordinate
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void ReverseLine(INT32U ulHeight, INT32U ulY)
{
	INT32U i, j, temp;
	
	for( i = 0; i < ulHeight; i++ )
	{
		for( j = 0; j < (SCR_XSIZE/4/2) ; j++ )
		{
			temp = *(INT32U*)(LCD_VIRTUAL_BUFFER + (ulY+i)*SCR_XSIZE/2 + j*4);
			temp ^= 0xFFFFFFFF;
			*(INT32U*)(LCD_VIRTUAL_BUFFER + (ulY+i)*SCR_XSIZE/2 + j*4) = temp;
		}
	}
}

/*********************************************************************************************
* name:		Zdma0Done()
* func:		LCD dma interrupt handle function
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
volatile static INT8U ucZdma0Done=1;	//When DMA is finish,ucZdma0Done is cleared to Zero
void Zdma0Done(void)
{
	rI_ISPC=BIT_ZDMA0;	    //clear pending
	ucZdma0Done=0;
}

/*********************************************************************************************
* name:		Lcd_Dma_Trans()
* func:		dma transport virtual LCD screen to LCD actual screen
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Dma_Trans(void)
{
	INT8U err;
	
	ucZdma0Done=1;
	//#define LCD_VIRTUAL_BUFFER	(0xc400000)
	//#define LCD_ACTIVE_BUFFER	(LCD_VIRTUAL_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))	//DMA ON
	//#define LCD_ACTIVE_BUFFER	LCD_VIRTUAL_BUFFER								//DMA OFF
	//#define LCD_BUF_SIZE		(SCR_XSIZE*SCR_YSIZE/2)
	//So the Lcd Buffer Low area is from LCD_VIRTUAL_BUFFER to (LCD_ACTIVE_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))
	rNCACHBE1=(((unsigned)(LCD_ACTIVE_BUFFER)>>12) <<16 )|((unsigned)(LCD_VIRTUAL_BUFFER)>>12);
  	rZDISRC0=(DW<<30)|(1<<28)     |LCD_VIRTUAL_BUFFER; // inc
  	rZDIDES0=( 2<<30)|(1<<28)     |LCD_ACTIVE_BUFFER; // inc
    rZDICNT0=( 2<<28)|(1<<26)     |(3<<22)     |(0<<20)      |(LCD_BUF_SIZE);
        //           |            |            |             |            |---->0 = Disable DMA
        //           |            |            |             |------------>Int. whenever transferred
        //           |            |            |-------------------->Write time on the fly
        //           |            |---------------------------->Block(4-word) transfer mode
        //           |------------------------------------>whole service
	    //reEnable ZDMA transfer
  	rZDICNT0 |= (1<<20);		//after ES3
    rZDCON0=0x1; // start!!!  

	//Delay(500);
// HAY QUE MODIFICARLO	while(ucZdma0Done);		//wait for DMA finish
}

/*********************************************************************************************
* name:		Lcd_Test()
* func:		LCD test function
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Test(void)
{
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	/* draw rectangle pattern */ 
    #ifdef Eng_v // english version
	Lcd_DspAscII8x16(10,0,DARKGRAY,"Embest S3CEV40 ");
	#else
//	Lcd_DspHz16(10,0,DARKGRAY,"英蓓特三星实验评估板");
	#endif
	Lcd_DspAscII8x16(10,20,BLACK,"Codigo del puesto: ");
	Lcd_Draw_Box(10,40,310,230,14);
	Lcd_Draw_Box(20,45,300,225,13);
	Lcd_Draw_Box(30,50,290,220,12);
	Lcd_Draw_Box(40,55,280,215,11);
	Lcd_Draw_Box(50,60,270,210,10);
	Lcd_Draw_Box(60,65,260,205,9);
	Lcd_Draw_Box(70,70,250,200,8);
	Lcd_Draw_Box(80,75,240,195,7);
	Lcd_Draw_Box(90,80,230,190,6);
	Lcd_Draw_Box(100,85,220,185,5);
	Lcd_Draw_Box(110,90,210,180,4);
	Lcd_Draw_Box(120,95,200,175,3);
	Lcd_Draw_Box(130,100,190,170,2);
	BitmapView(125,135,Stru_Bitmap_gbMouse);
	Lcd_Dma_Trans();

}
//************************NUESTRO C覦IGO ***********************************

void pintar_pantalla_inicio() {
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	Lcd_DspAscII8x16(0, 10, BLACK, " ******** JUEGO DEL SUDOKU 2021 ******** ");

	Lcd_DspAscII8x16(0, 30, BLACK, "Instrucciones:");

	Lcd_DspAscII8x16(10, 50, BLACK, "Primero se seleccionara la fila.");
	Lcd_DspAscII8x16(10, 70, BLACK, "Segundo se seleccionara la colummna.");
	Lcd_DspAscII8x16(10, 90, BLACK, "Tercero se seleccionara el valor.");

	Lcd_DspAscII8x16(10, 120, BLACK, "Se confirma con el boton izquierdo.");

	Lcd_DspAscII8x16(10, 140, BLACK, "Se aumenta el valor con el boton");
	Lcd_DspAscII8x16(10, 160, BLACK, "derecho.");

	Lcd_DspAscII8x16(40, 180, BLACK, "Pulse un boton para empezar.");
	Lcd_Dma_Trans(); //NECESARIO PARA QUE SE ESCRIBA EN EL LCD
}

void pintar_tablero() {
	int i = 0;
	Lcd_Draw_Box(inicioTablero_izquierda, inicioTablero_arriba, dimension_cuadricula, dimension_cuadricula, BLACK);
	for (i = 0; i < 9; i++) {
		//Pintamos los indices del tablero
		Lcd_DspAscII8x16(27 + separacion * i, 0, BLACK, numeros[i + 1]);
		Lcd_DspAscII8x16(4, 25 + separacion * i, BLACK, numeros[i + 1]);

		if (i % 3 == 0) { //Pintamos lineas gruesas de la cuadricula
			Lcd_Draw_VLine(inicioTablero_arriba, dimension_cuadricula, inicioTablero_izquierda + i * separacion, 15, 1);
			Lcd_Draw_HLine(inicioTablero_izquierda, dimension_cuadricula, inicioTablero_arriba + i * separacion, 15, 1);
		} else { //Pintamos lineas finaes normales
			Lcd_Draw_VLine(inicioTablero_arriba, dimension_cuadricula, inicioTablero_izquierda + i * separacion, 10, 1);
			Lcd_Draw_HLine(inicioTablero_izquierda, dimension_cuadricula, inicioTablero_arriba + i * separacion, 10, 1);
		}
	}
	Lcd_Dma_Trans();
}
void pintar_tiempo_partida(int tiempo) {
	/*int tiempo_partida;
	tiempo_partida = timer1_count();
	Lcd_DspAscII8x16(225, 10, BLACK, "Tiempo ");
	Lcd_DspAscII8x16(225, 20, BLACK,  "partida");
	Lcd_DspAscII8x16(225, 30, BLACK, tiempo_partida);*/

	int i;
			INT8U t_micro[8] = { 0, 0, 0, 0, 0, 0, 0, '\0' };

			Lcd_DspAscII8x16(225, 10, BLACK, "Tiempo ");
			Lcd_DspAscII8x16(225, 20, BLACK,  "partida");

			for (i = 6; i >= 0; i--) {
				t_micro[i] = 0;
				t_micro[i] = (tiempo % 10) + 48;
				tiempo /= 10;
			}
			Lcd_DspAscII8x16(225, 30, BLACK, t_micro);





}
void pintar_tiempo_calculos(unsigned int tiempo) {
	/*int tiempo_calculos;
	tiempo_calculos = timer1_count();
	Lcd_DspAscII8x16(225, 70, BLACK, "Tiempo");
	Lcd_DspAscII8x16(225, 80, BLACK, "calculos");
	Lcd_DspAscII8x16(225, 90, BLACK, tiempo_calculos);*/
		int i;
		INT8U t_micro[8] = { 0, 0, 0, 0, 0, 0, 0, '\0' };

		Lcd_DspAscII8x16(225, 70, BLACK, "Tiempo");
		Lcd_DspAscII8x16(225, 80, BLACK, "calculos");
		//Lcd_Draw_HLine(225, 300, 85, WHITE, 20);
		for (i = 6; i >= 0; i--) {
			t_micro[i] = 0;
			t_micro[i] = (tiempo % 10) + 48;
			tiempo /= 10;
		}
		Lcd_DspAscII8x16(225, 90, BLACK, t_micro);
}

void actualizar_tiempo_partida(int tiempo_partida) {
	pintar_tiempo_partida(tiempo_partida);
	Lcd_Dma_Trans();
}
void actualizar_tiempo_calculos(unsigned int tiempo) {
	pintar_tiempo_calculos(tiempo);
	Lcd_Dma_Trans();
}

void pintar_informacion_tablero() {
	Lcd_DspAscII8x16(225, 120, BLACK, "Introduzca");
	Lcd_DspAscII8x16(225, 135, BLACK, "fila 0 para");
	Lcd_DspAscII8x16(225, 150, BLACK, "finalizar");

}
void pintar_numeros_tablero() {
	//uint8_t numeroPintar = 0;
	int candidatos = 0;
	int aux = 0x000000001;
	int i, j, k;
	int izq, arriba;
	for (i = 0; i < 9; i++) { //Recorremos las filas
		for (j = 0; j < 9; j++) { //Recorremos las columnas
			uint8_t valor = pasarela_celda_leer_valor(i, j);
			int a = 0;
			if ( valor != 0x0000) { //Vemos si tiene valor cada celda
				//if (pasarela_celda_es_pista(i, j) == 1) {
					//Lcd_DspAscII8x16(marIzq_num + separacion * j, marSup_num + separacion * i, TRANSPARENCY, numeros[numeroPintar]);
				//} else {
						Lcd_DspAscII8x16(marIzq_num + separacion * j,marSup_num + separacion * i, BLACK, numeros[valor]);
			//	}
			} else {
				k = 1;
				candidatos = pasarela_obtener_candidatos(i, j);
				candidatos = candidatos >> 7;
				while (k <= 9) {
					if ((candidatos &  aux) == 0) {
						izq = inicioTablero_izquierda + separacion * j;
						arriba = inicioTablero_arriba + separacion * i;
						pintar_candidatos(izq, arriba, k);
					}
					candidatos = candidatos >> 1;
					//aux = aux <<1;
					k++;

				/*	if (celda_error(i, j) == TRUE) {
						pintar_error_celda(i, j, numeroPintar);
					}*/
				}
			}
		}
	}
}
void pintar_candidatos(int izq, int arriba, int k) {
	/* Dependiendo del numero del candidato a pintar pintamos en una zona u otra de la cuadricula*/
	int lado = 3;
	int x, y;
	switch (k) {
	case 1:
		x = izq + 3;
		y = arriba + 3;
		break;
	case 2:
		x = izq + 9;
		y = arriba + 3;
		break;
	case 3:
		x = izq + 15;
		y = arriba + 3;
		break;
	case 4:
		x = izq + 3;
		y = arriba + 9;
		break;
	case 5:
		x = izq + 9;
		y = arriba + 9;
		break;
	case 6:
		x = izq + 15;
		y = arriba + 9;
		break;
	case 7:
		x = izq + 3;
		y = arriba + 15;
		break;
	case 8:
		x = izq + 9;
		y = arriba + 15;
		break;
	case 9:
		x = izq + 15;
		y = arriba + 15;
		break;
	default:
		break;
	}
	Lcd_Draw_HLine(x, x + lado, y, BLACK, lado + 2);

}
void pintar_pantalla_tablero(){
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	pintar_tablero();
	pintar_numeros_tablero();
	pintar_informacion_tablero();
	pintar_tiempo_partida(timer1_count());
	pintar_tiempo_calculos(timer1_count());
	Lcd_Dma_Trans();
}
void pintar_pantalla_despedida(){
	/* clear screen */
		Lcd_Clr();
		Lcd_Active_Clr();
		Lcd_DspAscII8x16(0, 20, BLACK, "Gracias por haber jugado al sudoku 2021");
		Lcd_DspAscII8x16(0, 50, BLACK, "Creadores: Alvaro Fraidias y David Ros");
		Lcd_DspAscII8x16(20, 170, BLACK, "Pulsa un botono para volver a jugar");
		Lcd_Dma_Trans(); //NECESARIO PARA QUE SE ESCRIBA EN EL LCD
}
void pintar_error_pista_celda_tablero() {

	Lcd_DspAscII8x16(15, 220, BLACK, "Error: La celda seleccionada es pista");
	Lcd_Dma_Trans();

}

void pintar_error_candidato_celda_tablero() {

	Lcd_DspAscII8x16(15, 220, BLACK, "El numero no es candidato");
	Lcd_Dma_Trans();

}
