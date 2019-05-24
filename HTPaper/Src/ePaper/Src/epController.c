#include <stdlib.h>
#include "main.h"
#include "../Inc/epController.h"
#include "../Inc/epaper.h"
#include "../Src/GUIPaint/GUI_Paint.h"
#include "../Src/Images/ImageData.h"
#include "../Src/Fonts/fonts.h"
void epControllerInit()
{
	epInit(&hspi1, lut_full_update);
	epClear();
}

uint8_t epControllerSplash()
{
  uint8_t *BlackImage;
  uint32_t Imagesize = ((EP_WIDTH % 8 == 0)? (EP_WIDTH / 8 ): (EP_WIDTH / 8 + 1)) * EP_HEIGHT;
  if((BlackImage = (uint8_t *)malloc(Imagesize)) == NULL) {
	 // printf("Failed to apply for black memory...\r\n");
	  return -1;
  }
  Paint_NewImage(BlackImage, EP_WIDTH, EP_HEIGHT, 270, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
  Paint_DrawBitMap(gImage_2in9);

  Paint_DrawString_EN(12, 15, "12", &Font16, WHITE, BLACK);
  Paint_DrawCircle(40, 15, 2, BLACK, WHITE, DOT_PIXEL_1X1);


  epDisplayImage(BlackImage);
 // epTurnOnDisplay();
  HAL_Delay(2000);
  return 0;
}
