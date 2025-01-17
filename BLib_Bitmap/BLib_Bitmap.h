#ifndef __BITMAP_H__
#define __BITMAP_H__

typedef struct
{
	char* Bitmap;
	int16_t Width;
	int16_t Height;
}Bitmap_t;

extern Bitmap_t BLib_Bitmap_Light;
extern Bitmap_t Blib_Bitmap_Plant;
extern Bitmap_t Blib_Bitmap_Plant_2;
extern Bitmap_t BLib_Bitmap_Temp;
extern Bitmap_t BLib_Bitmap_Time_1;
extern Bitmap_t BLib_Bitmap_Time_2;
extern Bitmap_t BLib_Bitmap_Humidity;
extern Bitmap_t BLib_Bitmap_Moisture;

#endif
