/*
 * epaper.c
 *
 *  Created on: 22 May 2019
 *      Author: Graeme
 */

#include "main.h"
#include "../Inc/epaper.h"

SPI_HandleTypeDef *hspi;

const unsigned char lut_full_update[] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const unsigned char lut_partial_update[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void epClear(void)
{
    uint16_t Width, Height;
    Width = (EP_WIDTH % 8 == 0)? (EP_WIDTH / 8 ): (EP_WIDTH / 8 + 1);
    Height = EP_HEIGHT;
    epSetWindows(0, 0, EP_WIDTH, EP_HEIGHT);
    for (uint16_t j = 0; j < Height; j++) {
        epSetCursor(0, j);
        epSendCmd(WRITE_RAM);
        for (uint16_t i = 0; i < Width; i++) {
            epSendData(0XFF);
        }
    }
    epTurnOnDisplay();
}
/******************************************************************************
function :	Setting the display window
parameter:
******************************************************************************/
 void epSetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
	 epSendCmd(SET_RAM_X_ADDRESS_START_END_POSITION);
    epSendData((Xstart >> 3) & 0xFF);
    epSendData((Xend >> 3) & 0xFF);

    epSendCmd(SET_RAM_Y_ADDRESS_START_END_POSITION);
    epSendData(Ystart & 0xFF);
    epSendData((Ystart >> 8) & 0xFF);
    epSendData(Yend & 0xFF);
    epSendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
******************************************************************************/
 void epSetCursor(uint16_t Xstart, uint16_t Ystart)
{
	epSendCmd(SET_RAM_X_ADDRESS_COUNTER);
    epSendData((Xstart >> 3) & 0xFF);

    epSendCmd(SET_RAM_Y_ADDRESS_COUNTER);
    epSendData(Ystart & 0xFF);
    epSendData((Ystart >> 8) & 0xFF);

}

 void epWaitUntilIdle(void)
 {
     while (HAL_GPIO_ReadPin(SPI_DIS_BUSY_GPIO_Port, SPI_DIS_BUSY_Pin)==GPIO_PIN_SET){
     //while(DEV_Digital_Read(EPD_BUSY_PIN) == 1) {      //LOW: idle, HIGH: busy
         HAL_Delay(100);
     }
 }

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
 void epTurnOnDisplay(void)
{
	 epSendCmd(DISPLAY_UPDATE_CONTROL_2);
    epSendData(0xC4);
    epSendCmd(MASTER_ACTIVATION);
    epSendCmd(TERMINATE_FRAME_READ_WRITE);

    epWaitUntilIdle();
}

void epInit(SPI_HandleTypeDef *spi, const unsigned char *lut)
{
	HAL_GPIO_WritePin(SPI_DIS_RESET_GPIO_Port, SPI_DIS_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(SPI_DIS_RESET_GPIO_Port, SPI_DIS_RESET_Pin, GPIO_PIN_SET);
	hspi = spi;
	//HAL_GPIO_WritePin(SPI_DIS_CS_GPIO_Port, SPI_DIS_CS_Pin, SELECT_NOT);
	HAL_Delay(10);
	epSendCmd(DRIVER_OUTPUT_CONTROL);
	epSendData((EP_HEIGHT-1) & 0xff);
	epSendData(((EP_HEIGHT-1) >> 8) & 0xff);
	epSendData(0x00);
	epSendCmd(BOOSTER_SOFT_START_CONTROL);
	epSendData(0xD7);
	epSendData(0xD6);
	epSendData(0x9D);
	epSendCmd(WRITE_VCOM_REGISTER);
	epSendData(0xA8);                     // VCOM 7C
	epSendCmd(SET_DUMMY_LINE_PERIOD);
	epSendData(0x1A);                     // 4 dummy lines per gate
	epSendCmd(SET_GATE_TIME);
	epSendData(0x08);                     // 2us per line
	epSendCmd(BORDER_WAVEFORM_CONTROL);
	epSendData(0x03);
	epSendCmd(DATA_ENTRY_MODE_SETTING);
	epSendData(0x03);

	//set the look-up table register
	epSendCmd(WRITE_LUT_REGISTER);
	for (uint16_t i = 0; i < 30; i++) {
	     epSendData(lut[i]);
	}
}



void epSendCmd(uint8_t cmd)
{
	// DC Low for Command Mode
	HAL_GPIO_WritePin(SPI_DIS_DC_GPIO_Port, SPI_DIS_DC_Pin, MODE_CMD);
	// Set SC for this device
	HAL_GPIO_WritePin(SPI_DIS_CS_GPIO_Port, SPI_DIS_CS_Pin, SELECT);
	HAL_SPI_Transmit(hspi, &cmd, 1, 0xff);
	// De-Select device
	HAL_GPIO_WritePin(SPI_DIS_CS_GPIO_Port, SPI_DIS_CS_Pin, SELECT_NOT);
}



void epSendData(uint8_t data)
{
	// DC Low for Data Mode
	HAL_GPIO_WritePin(SPI_DIS_DC_GPIO_Port, SPI_DIS_DC_Pin, MODE_DATA);
	// Set SC for this device
	HAL_GPIO_WritePin(SPI_DIS_CS_GPIO_Port, SPI_DIS_CS_Pin, SELECT);
	HAL_SPI_Transmit(hspi, &data, 1, 0xff);
	// De-Select device
	HAL_GPIO_WritePin(SPI_DIS_CS_GPIO_Port, SPI_DIS_CS_Pin, SELECT_NOT);
}



/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void epDisplayImage(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (EP_WIDTH % 8 == 0)? (EP_WIDTH / 8 ): (EP_WIDTH / 8 + 1);
    Height = EP_HEIGHT;

    uint16_t Addr = 0;
    // UDOUBLE Offset = ImageName;
    epSetWindows(0, 0, EP_WIDTH, EP_HEIGHT);
    for (uint16_t j = 0; j < Height; j++) {
        epSetCursor(0, j);
        epSendCmd(WRITE_RAM);
        for (uint16_t i = 0; i < Width; i++) {
            Addr = i + j * Width;
            epSendData(Image[Addr]);
        }
    }
    epTurnOnDisplay();
}
