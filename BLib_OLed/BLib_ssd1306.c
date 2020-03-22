/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>

   ----------------------------------------------------------------------
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */
#define stm32f4
#include "BLib_ssd1306.h"
#include "math.h"
extern I2C_HandleTypeDef hi2c1;
/* Write command */
//#define BLib_SSD1306_WRITECOMMAND(command)      BLib_SSD1306_I2C_Write(BLib_SSD1306_I2C_ADDR, 0x00, (command))
/* Write data */
//#define BLib_SSD1306_WRITEDATA(data)            BLib_SSD1306_I2C_Write(BLib_SSD1306_I2C_ADDR, 0x40, (data))
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SSD1306 data buffer */
static uint8_t BLib_SSD1306_Buffer[BLib_SSD1306_WIDTH * BLib_SSD1306_HEIGHT / 8];

/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} BLib_SSD1306_t;

/* Private variable */
static BLib_SSD1306_t SSD1306;


#define BLib_SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define BLib_SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define BLib_SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define BLib_SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define BLib_SSD1306_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define BLib_SSD1306_ACTIVATE_SCROLL                      0x2F // Start scroll
#define BLib_SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define BLib_SSD1306_NORMALDISPLAY       0xA6
#define BLib_SSD1306_INVERTDISPLAY       0xA7

void BLib_SSD1306_WRITECOMMAND(uint8_t command) {

//	HAL_I2C_Master_Transmit(&hi2c1, address<<1, buffer_loc, 2, 10);
	HAL_I2C_Mem_Write(&hi2c1,BLib_SSD1306_I2C_ADDR<<1,0x00,I2C_MEMADD_SIZE_8BIT,&command,1,HAL_MAX_DELAY);
}

void BLib_SSD1306_I2C_WriteMulti(uint8_t* buffer , size_t buffer_size) {
	HAL_I2C_Mem_Write(&hi2c1,BLib_SSD1306_I2C_ADDR<<1,0x40,I2C_MEMADD_SIZE_8BIT,buffer,buffer_size,HAL_MAX_DELAY);
}

void BLib_SSD1306_Data_Display_int(uint32_t data , FontDef_t* Font , uint8_t BLib_SSD1306_Data_Unit)
{
		char x_char[10];
		uint8_t i, j, numLength , unitLength;
		uint8_t x_char_loc[10];

		for(i=1 ; i < 10 ; i++)
		{
			if(data < pow(10,i))
			{
				numLength = i;
				break;
			}
		}
		
		switch(BLib_SSD1306_Data_Unit)
		{
			case BLib_SSD1306_Data_Unit_Deg:
				x_char[numLength] = ' ';
				x_char[numLength+1] = 'D';
				x_char[numLength+2] = 'e';
				x_char[numLength+3] = 'g';
				unitLength = 3;
				break;
			case BLib_SSD1306_Data_Unit_Lux:
				x_char[numLength] = ' ';
				x_char[numLength+1] = 'L';
				x_char[numLength+2] = 'u';
				x_char[numLength+3] = 'x';				
				unitLength = 3;
				break;
			case BLib_SSD1306_Data_Unit_Percent:
				x_char[numLength] = ' ';
				x_char[numLength+1] = '%';			
				unitLength = 1;
				break;			
			case BLib_SSD1306_Data_Unit_Sec:
				x_char[numLength] = 's';
				unitLength = 0;
				break;
			case BLib_SSD1306_Data_Unit_Day:
				switch(data)
				{
					case 1:
						BLib_SSD1306_Puts("Mon",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 2:
						BLib_SSD1306_Puts("Tue",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 3:
						BLib_SSD1306_Puts("Wed",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 4:
						BLib_SSD1306_Puts("Thu",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 5:
						BLib_SSD1306_Puts("Fri",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 6:
						BLib_SSD1306_Puts("Sat",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					case 7:
						BLib_SSD1306_Puts("Sun",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
					default:
						BLib_SSD1306_Puts("Error",&Font_11x18,BLib_SSD1306_COLOR_WHITE);
						break;
				}
			default:
				x_char[numLength] = ' ';
				unitLength = 0;
				break;
		}
		for(j=1 ; j < numLength ; j++)
		{		    
			x_char_loc[j-1] = data / pow(10,numLength-j);
			data = data - x_char_loc[j-1] * pow(10,numLength-j);			
		}
		x_char_loc[numLength-1] = (data % 10);
		if(BLib_SSD1306_Data_Unit != BLib_SSD1306_Data_Unit_Day)
		{
			for(i=0 ;i < numLength ; i++)
				x_char[i] = x_char_loc[i] + '0';	
			for(i=0 ;i <= numLength + unitLength ; i++)
				BLib_SSD1306_Putc (x_char[i], Font ,BLib_SSD1306_COLOR_WHITE);	
		}
		else
		{
			;
		}
}

void BLib_SSD1306_Data_Display_float(float data , FontDef_t* Font, uint8_t unit)
{
		char x_char[10];
		uint8_t i, j, numLength ,unitLength;
		uint8_t x_char_loc[10];
		for(i=1 ; i < 10 ; i++)
		{
			if(data < pow(10,i))
			{
				numLength = i;
				break;
			}
		}
		for(j=1 ; j < numLength ; j++)
		{
		    
			x_char_loc[j-1] = data / pow(10,numLength-j);
			data=data - x_char_loc[j-1] * pow(10,numLength-j);			
		}
		x_char_loc[numLength-1] = ((uint16_t)data % 10);
		data = (data - ((uint16_t)data % 10)) * 1000;
		if((((uint16_t)data%100)%10) < 5)
		{
		    data/=10;
    		x_char_loc[numLength+1] = (uint16_t)data / 10;
    		x_char_loc[numLength+2] = (uint16_t)data % 10;
		}
		else
		{
		    data/=10;
    		x_char_loc[numLength+1] = (uint16_t)data / 10;
    		x_char_loc[numLength+2] = ((uint16_t)data % 10)+1;
    		if(x_char_loc[numLength+2] == 10)
    		{
    		    x_char_loc[numLength+1] =x_char_loc[numLength+1] + 1;
    		    x_char_loc[numLength+2] = 0;
    		    if(x_char_loc[numLength+1] == 10)
    		    {
    		        x_char_loc[numLength-1] = x_char_loc[numLength-1] + 1;
    		        x_char_loc[numLength+1] = 0;
    		    }
    		}
		}
		switch(unit)
		{
			case BLib_SSD1306_Data_Unit_Deg:
				x_char[numLength+3] = ' ';
				x_char[numLength+4] = 'D';
				x_char[numLength+5] = 'e';
				x_char[numLength+6] = 'g';
				unitLength = 4;
				break;
			case BLib_SSD1306_Data_Unit_Lux:
				x_char[numLength+3] = ' ';
				x_char[numLength+4] = 'L';
				x_char[numLength+5] = 'u';
				x_char[numLength+6] = 'x';				
				unitLength = 4;
				break;
			case BLib_SSD1306_Data_Unit_Percent:
				x_char[numLength+3] = ' ';
				x_char[numLength+4] = '%';			
				unitLength = 2;
				break;			
			default:
				x_char[numLength] = ' ';
				unitLength = 0;
				break;
		}		
		for(i=0;i<numLength;i++)
		    x_char[i] = x_char_loc[i] + '0';
		x_char_loc[numLength-1] = ((uint16_t)data % 10);		
		x_char[numLength] = '.';
		x_char[numLength+1] = x_char_loc[numLength+1] + '0';
		x_char[numLength+2] = x_char_loc[numLength+2] + '0';
		for(i=0 ;i <= numLength+2 + unitLength ; i++)
			BLib_SSD1306_Putc (x_char[i], Font ,BLib_SSD1306_COLOR_WHITE);		
}

void BLib_SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row)
{
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_RIGHT_HORIZONTAL_SCROLL);  // send 0x26
  BLib_SSD1306_WRITECOMMAND (0x00);  // send dummy
  BLib_SSD1306_WRITECOMMAND(start_row);  // start page address
  BLib_SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
  BLib_SSD1306_WRITECOMMAND(end_row);  // end page address
  BLib_SSD1306_WRITECOMMAND(0X00);
  BLib_SSD1306_WRITECOMMAND(0XFF);
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_ACTIVATE_SCROLL); // start scroll
}


void BLib_SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row)
{
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_LEFT_HORIZONTAL_SCROLL);  // send 0x26
  BLib_SSD1306_WRITECOMMAND (0x00);  // send dummy
  BLib_SSD1306_WRITECOMMAND(start_row);  // start page address
  BLib_SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
  BLib_SSD1306_WRITECOMMAND(end_row);  // end page address
  BLib_SSD1306_WRITECOMMAND(0X00);
  BLib_SSD1306_WRITECOMMAND(0XFF);
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_ACTIVATE_SCROLL); // start scroll
}


void BLib_SSD1306_Scrolldiagright(uint8_t start_row, uint8_t end_row)
{
  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
  BLib_SSD1306_WRITECOMMAND (0x00);   // write dummy
  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_HEIGHT);

  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  BLib_SSD1306_WRITECOMMAND (0x00);
  BLib_SSD1306_WRITECOMMAND(start_row);
  BLib_SSD1306_WRITECOMMAND(0X00);
  BLib_SSD1306_WRITECOMMAND(end_row);
  BLib_SSD1306_WRITECOMMAND (0x01);
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_ACTIVATE_SCROLL);
}


void BLib_SSD1306_Scrolldiagleft(uint8_t start_row, uint8_t end_row)
{
  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
  BLib_SSD1306_WRITECOMMAND (0x00);   // write dummy
  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_HEIGHT);

  BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  BLib_SSD1306_WRITECOMMAND (0x00);
  BLib_SSD1306_WRITECOMMAND(start_row);
  BLib_SSD1306_WRITECOMMAND(0X00);
  BLib_SSD1306_WRITECOMMAND(end_row);
  BLib_SSD1306_WRITECOMMAND (0x01);
  BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_ACTIVATE_SCROLL);
}


void BLib_SSD1306_Stopscroll(void)
{
	BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_DEACTIVATE_SCROLL);
}



void BLib_SSD1306_InvertDisplay (int i)
{
  if (i) BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_INVERTDISPLAY);

  else BLib_SSD1306_WRITECOMMAND (BLib_SSD1306_NORMALDISPLAY);

}


void BLib_SSD1306_DrawBitmap(int16_t x, int16_t y, Bitmap_t* BLib_Bitmap, BLib_SSD1306_COLOR_t color)
{
    int16_t byteWidth = (BLib_Bitmap->Width + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<BLib_Bitmap->Height; j++, y++)
    {
        for(int16_t i=0; i<BLib_Bitmap->Width; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&BLib_Bitmap->Bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) 
						{	
							BLib_SSD1306_DrawPixel(x+i, y, color);
						}
        }
    }
}








void BLib_SSD1306_Init(void) {

	/* Init I2C */
	/* Check if LCD connected to I2C */
	
	/* A little delay */
	
	/* Init LCD */
	HAL_Delay(100);
	BLib_SSD1306_WRITECOMMAND(0xAE); //display off
	BLib_SSD1306_WRITECOMMAND(0x20); //Set Memory Addressing Mode   
	BLib_SSD1306_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	BLib_SSD1306_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	BLib_SSD1306_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
	BLib_SSD1306_WRITECOMMAND(0x00); //---set low column address
	BLib_SSD1306_WRITECOMMAND(0x10); //---set high column address
	BLib_SSD1306_WRITECOMMAND(0x40); //--set start line address
	BLib_SSD1306_WRITECOMMAND(0x81); //--set contrast control register
	BLib_SSD1306_WRITECOMMAND(0xFF);
	BLib_SSD1306_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
	BLib_SSD1306_WRITECOMMAND(0xA6); //--set normal display
	BLib_SSD1306_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
	BLib_SSD1306_WRITECOMMAND(0x3F); //
	BLib_SSD1306_WRITECOMMAND(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	BLib_SSD1306_WRITECOMMAND(0xD3); //-set display offset
	BLib_SSD1306_WRITECOMMAND(0x00); //-not offset
	BLib_SSD1306_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
	BLib_SSD1306_WRITECOMMAND(0xF0); //--set divide ratio
	BLib_SSD1306_WRITECOMMAND(0xD9); //--set pre-charge period
	BLib_SSD1306_WRITECOMMAND(0x22); //
	BLib_SSD1306_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	BLib_SSD1306_WRITECOMMAND(0x12);
	BLib_SSD1306_WRITECOMMAND(0xDB); //--set vcomh
	BLib_SSD1306_WRITECOMMAND(0x20); //0x20,0.77xVcc
	BLib_SSD1306_WRITECOMMAND(0x8D); //--set DC-DC enable
	BLib_SSD1306_WRITECOMMAND(0x14); //
	BLib_SSD1306_WRITECOMMAND(0xAF); //--turn on SSD1306 panel
	

	

//	BLib_SSD1306_WRITECOMMAND(BLib_SSD1306_DEACTIVATE_SCROLL);

	/* Clear screen */
	BLib_SSD1306_Fill(BLib_SSD1306_COLOR_BLACK);
	
	/* Update screen */
	BLib_SSD1306_UpdateScreen();
	
	/* Set default values */
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;
//	
//	/* Initialized OK */
	SSD1306.Initialized = 1;
	
	/* Return OK */
}

void BLib_SSD1306_UpdateScreen(void) {
	uint8_t m;
	
	for (m = 0; m < 8; m++) {
		BLib_SSD1306_WRITECOMMAND(0xB0 + m);
		BLib_SSD1306_WRITECOMMAND(0x00);
		BLib_SSD1306_WRITECOMMAND(0x10);
		
		/* Write multi data */
		BLib_SSD1306_I2C_WriteMulti(&BLib_SSD1306_Buffer[BLib_SSD1306_WIDTH * m] , BLib_SSD1306_WIDTH);
	}
}

void BLib_SSD1306_ToggleInvert(void) {
	uint16_t i;
	
	/* Toggle invert */
	SSD1306.Inverted = !SSD1306.Inverted;
	
	/* Do memory toggle */
	for (i = 0; i < sizeof(BLib_SSD1306_Buffer); i++) {
		BLib_SSD1306_Buffer[i] = ~BLib_SSD1306_Buffer[i];
	}
}

void BLib_SSD1306_Fill(BLib_SSD1306_COLOR_t color) {
	/* Set memory */
	memset(BLib_SSD1306_Buffer, (color == BLib_SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(BLib_SSD1306_Buffer));
}

void BLib_SSD1306_DrawPixel(uint16_t x, uint16_t y, BLib_SSD1306_COLOR_t color) {
	if (
		x >= BLib_SSD1306_WIDTH ||
		y >= BLib_SSD1306_HEIGHT
	) {
		/* Error */
		return;
	}
	
	/* Check if pixels are inverted */
	if (SSD1306.Inverted) {
		color = (BLib_SSD1306_COLOR_t)!color;
	}
	
	/* Set color */
	if (color == BLib_SSD1306_COLOR_WHITE) {
		BLib_SSD1306_Buffer[x + (y / 8) * BLib_SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		BLib_SSD1306_Buffer[x + (y / 8) * BLib_SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void BLib_SSD1306_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

char BLib_SSD1306_Putc(char ch, FontDef_t* Font, BLib_SSD1306_COLOR_t color) {
	uint32_t i, b, j;
	
	/* Check available space in LCD */
	if (
		BLib_SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
		BLib_SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}
	
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				BLib_SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (BLib_SSD1306_COLOR_t) color);
			} else {
				BLib_SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (BLib_SSD1306_COLOR_t)!color);
			}
		}
	}
	
	/* Increase pointer */
	SSD1306.CurrentX += Font->FontWidth;
	
	/* Return character written */
	return ch;
}

char BLib_SSD1306_Puts(char* str, FontDef_t* Font, BLib_SSD1306_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (BLib_SSD1306_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}
		
		/* Increase string pointer */
		str++;
	}
	
	/* Everything OK, zero should be returned */
	return *str;
}
 

void BLib_SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, BLib_SSD1306_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp; 
	
	/* Check for overflow */
	if (x0 >= BLib_SSD1306_WIDTH) {
		x0 = BLib_SSD1306_WIDTH - 1;
	}
	if (x1 >= BLib_SSD1306_WIDTH) {
		x1 = BLib_SSD1306_WIDTH - 1;
	}
	if (y0 >= BLib_SSD1306_HEIGHT) {
		y0 = BLib_SSD1306_HEIGHT - 1;
	}
	if (y1 >= BLib_SSD1306_HEIGHT) {
		y1 = BLib_SSD1306_HEIGHT - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			BLib_SSD1306_DrawPixel(x0, i, c);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			BLib_SSD1306_DrawPixel(i, y0, c);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		BLib_SSD1306_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

void BLib_SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, BLib_SSD1306_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= BLib_SSD1306_WIDTH ||
		y >= BLib_SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= BLib_SSD1306_WIDTH) {
		w = BLib_SSD1306_WIDTH - x;
	}
	if ((y + h) >= BLib_SSD1306_HEIGHT) {
		h = BLib_SSD1306_HEIGHT - y;
	}
	
	/* Draw 4 lines */
	BLib_SSD1306_DrawLine(x, y, x + w, y, c);         /* Top line */
	BLib_SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	BLib_SSD1306_DrawLine(x, y, x, y + h, c);         /* Left line */
	BLib_SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void BLib_SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, BLib_SSD1306_COLOR_t c) {
	uint8_t i;
	
	/* Check input parameters */
	if (
		x >= BLib_SSD1306_WIDTH ||
		y >= BLib_SSD1306_HEIGHT
	) {
		/* Return error */
		return;
	}
	
	/* Check width and height */
	if ((x + w) >= BLib_SSD1306_WIDTH) {
		w = BLib_SSD1306_WIDTH - x;
	}
	if ((y + h) >= BLib_SSD1306_HEIGHT) {
		h = BLib_SSD1306_HEIGHT - y;
	}
	
	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		BLib_SSD1306_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void BLib_SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, BLib_SSD1306_COLOR_t color) {
	/* Draw lines */
	BLib_SSD1306_DrawLine(x1, y1, x2, y2, color);
	BLib_SSD1306_DrawLine(x2, y2, x3, y3, color);
	BLib_SSD1306_DrawLine(x3, y3, x1, y1, color);
}


void BLib_SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, BLib_SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		BLib_SSD1306_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void BLib_SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, BLib_SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    BLib_SSD1306_DrawPixel(x0, y0 + r, c);
    BLib_SSD1306_DrawPixel(x0, y0 - r, c);
    BLib_SSD1306_DrawPixel(x0 + r, y0, c);
    BLib_SSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        BLib_SSD1306_DrawPixel(x0 + x, y0 + y, c);
        BLib_SSD1306_DrawPixel(x0 - x, y0 + y, c);
        BLib_SSD1306_DrawPixel(x0 + x, y0 - y, c);
        BLib_SSD1306_DrawPixel(x0 - x, y0 - y, c);

        BLib_SSD1306_DrawPixel(x0 + y, y0 + x, c);
        BLib_SSD1306_DrawPixel(x0 - y, y0 + x, c);
        BLib_SSD1306_DrawPixel(x0 + y, y0 - x, c);
        BLib_SSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}

void BLib_SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, BLib_SSD1306_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    BLib_SSD1306_DrawPixel(x0, y0 + r, c);
    BLib_SSD1306_DrawPixel(x0, y0 - r, c);
    BLib_SSD1306_DrawPixel(x0 + r, y0, c);
    BLib_SSD1306_DrawPixel(x0 - r, y0, c);
    BLib_SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        BLib_SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        BLib_SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        BLib_SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        BLib_SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}
 


void BLib_SSD1306_Clear (void)
{
		BLib_SSD1306_Fill (BLib_SSD1306_COLOR_BLACK);
}
void BLib_SSD1306_ON(void) {
	BLib_SSD1306_WRITECOMMAND(0x8D);  
	BLib_SSD1306_WRITECOMMAND(0x14);  
	BLib_SSD1306_WRITECOMMAND(0xAF);  
}






