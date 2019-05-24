/*
 * epaper.h
 *
 *  Created on: 22 May 2019
 *      Author: Graeme
 */

#ifndef EPAPER_H_
#define EPAPER_H_

#include "spi.h"

// Display resolution
#define EPD_WIDTH       = 128
#define EPD_HEIGHT      = 296

// EPD2IN9 commands
#define DRIVER_OUTPUT_CONTROL                        0x01
#define BOOSTER_SOFT_START_CONTROL                   0x0C
#define GATE_SCAN_START_POSITION                     0x0F
#define DEEP_SLEEP_MODE                              0x10
#define DATA_ENTRY_MODE_SETTING                      0x11
#define SW_RESET                                     0x12
#define TEMPERATURE_SENSOR_CONTROL                   0x1A
#define MASTER_ACTIVATION                            0x20
#define DISPLAY_UPDATE_CONTROL_1                     0x21
#define DISPLAY_UPDATE_CONTROL_2                     0x22
#define WRITE_RAM                                    0x24
#define WRITE_VCOM_REGISTER                          0x2C
#define WRITE_LUT_REGISTER                           0x32
#define SET_DUMMY_LINE_PERIOD                        0x3A
#define SET_GATE_TIME                                0x3B
#define BORDER_WAVEFORM_CONTROL                      0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION         0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION         0x45
#define SET_RAM_X_ADDRESS_COUNTER                    0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                    0x4F
#define TERMINATE_FRAME_READ_WRITE                   0xFF

#define MODE_CMD									0
#define MODE_DATA									1
#define SELECT										0
#define SELECT_NOT									1

#define EP_WIDTH       128
#define EP_HEIGHT      296

extern const unsigned char lut_full_update[];
extern const unsigned char lut_partial_update[];
// Prototypes

// Sets the CS line high for active
void epInit(SPI_HandleTypeDef *spi, const unsigned char *lut);
void epSendCmd(uint8_t cmd);
void epSendData(uint8_t data);
void epSetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void epClear(void);
void epSetCursor(uint16_t Xstart, uint16_t Ystart);
void epWaitUntilIdle(void);
void epTurnOnDisplay(void);
void epDisplayImage(uint8_t *Image);

#endif /* EPAPER_H_ */
