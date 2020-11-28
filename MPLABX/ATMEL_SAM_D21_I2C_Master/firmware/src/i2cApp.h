/* ************************************************************************** */
/** Descriptive File Name

  @Company
    CCWO

  @File Name
    i2cApp.h

  @Summary
    IIC App functions.

  @Description

 */
/* ************************************************************************** */

#ifndef _I2CAPP_H    /* Guard against multiple inclusion */
#define _I2CAPP_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */

#define APP_AT24MAC_DEVICE_ADDR             0x54
#define MAX_TX_SIZE                         8
#define APP_ACK_DATA_LENGTH                 1
#define APP_RECEIVE_DUMMY_WRITE_LENGTH      1

#define VERIFY_ENABLE                       1
#define VERIFY_DISABLE                      0

#define LED_ON()                       LED_Clear()
#define LED_OFF()                      LED_Set()

typedef enum
{
    APP_STATE_EEPROM_STATUS_VERIFY,
    APP_STATE_EEPROM_WRITE,
    APP_STATE_EEPROM_WAIT_WRITE_COMPLETE,
    APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS,
    APP_STATE_EEPROM_READ,
    APP_STATE_EEPROM_WAIT_READ_COMPLETE,
    APP_STATE_VERIFY,
    APP_STATE_IDLE,
    APP_STATE_XFER_SUCCESSFUL,
    APP_STATE_XFER_ERROR

} APP_STATES;

typedef enum
{
    APP_TRANSFER_STATUS_IN_PROGRESS,
    APP_TRANSFER_STATUS_SUCCESS,
    APP_TRANSFER_STATUS_ERROR,
    APP_TRANSFER_STATUS_IDLE,

} APP_TRANSFER_STATUS;

void APP_I2CCallback(uintptr_t context );
void I2C_APP_TX(uint8_t DEVICE_ADDR, uint8_t MEMORY_ADDR, uint8_t *TRANSMIT_DATA, uint8_t TRANSMIT_DATA_LENGTH, uint8_t VERIFY);
void I2C_APP_RX(uint8_t DEVICE_ADDR, uint8_t MEMORY_ADDR, uint8_t *RECIEVE_DATA, uint8_t RECEIVE_DATA_LENGTH);

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
