/* ************************************************************************** */
/** Descriptive File Name

  @Company
     CCWO

  @File Name
    i2cApp.c

  @Summary
    IIC App functions.

  @Description

 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "i2cApp.h"

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */


void APP_I2CCallback(uintptr_t context )
{
    APP_TRANSFER_STATUS* transferStatus = (APP_TRANSFER_STATUS*)context;

    if(SERCOM2_I2C_ErrorGet() == SERCOM_I2C_ERROR_NONE)
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_ERROR;
        }
    }
}

bool I2C_APP_DEVICE_VERIFY(uint8_t DEVICE_ADDR, uint8_t DEVICE_CHECK_REPEAT) {
    APP_STATES state = APP_STATE_EEPROM_STATUS_VERIFY;
    volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;
    uint8_t ackData = 0;
    uint8_t repeat = 0;
    bool success = 0;

    while(1)
    {
        if (state == APP_STATE_IDLE) {
            break;
        }
        switch (state)
        {
            case APP_STATE_IDLE:
            {
                state = APP_STATE_IDLE;
                break;
            }
            case APP_STATE_EEPROM_STATUS_VERIFY:
            {
                /* Register the TWIHS Callback with transfer status as context */
                SERCOM2_I2C_CallbackRegister( APP_I2CCallback, (uintptr_t)&transferStatus );

               /* Verify if EEPROM is ready to accept new requests */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);

                state = APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS;
                break;
            }
            case APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS:
            {
                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_XFER_SUCCESSFUL;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                    if (repeat == DEVICE_CHECK_REPEAT) {
                        state = APP_STATE_XFER_ERROR;
                    }
                    repeat++;
                }
                break;
            }
            case APP_STATE_XFER_SUCCESSFUL:
            {
                success = 1;
                state = APP_STATE_IDLE;
                break;
            }
            case APP_STATE_XFER_ERROR:
            {
                success = 0;
                state = APP_STATE_IDLE;
                break;
            }
            default: break;
        }
    }

    return success;
}

void I2C_APP_TX(uint8_t DEVICE_ADDR, uint8_t MEMORY_ADDR, uint8_t *TRANSMIT_DATA, uint8_t TRANSMIT_DATA_LENGTH, uint8_t VERIFY)
{
    // printf("IIC_APP_TX\r\n");

    // int i = 0;
    APP_STATES state = APP_STATE_EEPROM_STATUS_VERIFY;
    volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;
    uint8_t ackData = 0;
    uint8_t txDataLength = APP_RECEIVE_DUMMY_WRITE_LENGTH + TRANSMIT_DATA_LENGTH;;
    uint8_t rxDataLength = TRANSMIT_DATA_LENGTH;
    uint8_t txData[APP_RECEIVE_DUMMY_WRITE_LENGTH+MAX_TX_SIZE] = {0};
    uint8_t rxData[MAX_TX_SIZE] = {0};

    txData[0] = MEMORY_ADDR;
    memcpy(&txData[APP_RECEIVE_DUMMY_WRITE_LENGTH], TRANSMIT_DATA, TRANSMIT_DATA_LENGTH);

    // for (i = 0; i < APP_RECEIVE_DUMMY_WRITE_LENGTH+MAX_TX_SIZE; i++) {
    //     printf("txData[%d]: 0x%X\r\n", i, txData[i]);
    // }

    while(1)
    {
        if (state == APP_STATE_IDLE) {
            break;
        }
        /* Check the application's current state. */
        switch (state)
        {
            case APP_STATE_IDLE:
                state = APP_STATE_IDLE;
                break;
            case APP_STATE_EEPROM_STATUS_VERIFY:

                /* Register the TWIHS Callback with transfer status as context */
                SERCOM2_I2C_CallbackRegister( APP_I2CCallback, (uintptr_t)&transferStatus );

               /* Verify if EEPROM is ready to accept new requests */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);

                state = APP_STATE_EEPROM_WRITE;
                break;

            case APP_STATE_EEPROM_WRITE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write data to EEPROM */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(DEVICE_ADDR, &txData[0], txDataLength);
                    state = APP_STATE_EEPROM_WAIT_WRITE_COMPLETE;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM is not ready to accept new requests. 
                     * Keep checking until the EEPROM becomes ready. */
                    state = APP_STATE_EEPROM_STATUS_VERIFY;
                }
                break;

            case APP_STATE_EEPROM_WAIT_WRITE_COMPLETE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Read the status of internal write cycle */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                    state = APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    if (VERIFY == VERIFY_ENABLE) {
                        // verify enable
                        state = APP_STATE_EEPROM_READ;
                    } else {
                        // verify disable
                        state = APP_STATE_XFER_SUCCESSFUL;
                    }
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                }
                break;

            case APP_STATE_EEPROM_READ:

                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                /* Read the data from the page written earlier */
                SERCOM2_I2C_WriteRead(DEVICE_ADDR, &txData[0], APP_RECEIVE_DUMMY_WRITE_LENGTH, &rxData[0], rxDataLength);

                state = APP_STATE_EEPROM_WAIT_READ_COMPLETE;

                break;

            case APP_STATE_EEPROM_WAIT_READ_COMPLETE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_VERIFY;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_VERIFY:
                if (memcmp(&txData[APP_RECEIVE_DUMMY_WRITE_LENGTH], &rxData[0], rxDataLength) == 0) {
                    /* It means received data is not same as transmitted data */
                    state = APP_STATE_XFER_SUCCESSFUL;
                } else {
                    /* It means received data is same as transmitted data */
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_XFER_SUCCESSFUL:
            {
                LED_ON();
                // for (i = 0; i < APP_RECEIVE_DUMMY_WRITE_LENGTH+MAX_TX_SIZE; i++) {
                //     printf("txData[%d]: 0x%X\r\n", i, txData[i]);
                // }
                printf("XFER SUCCESS\r\n");
                printf("DEVICE: 0x%02X, ADR: 0x%02X, TX: 0x%02X, RX: 0x%02X\r\n", DEVICE_ADDR, MEMORY_ADDR, txData[APP_RECEIVE_DUMMY_WRITE_LENGTH+0], rxData[0]);
                state = APP_STATE_IDLE;
                break;
            }
            case APP_STATE_XFER_ERROR:
            {
                LED_OFF();
                // for (i = 0; i < APP_RECEIVE_DUMMY_WRITE_LENGTH+MAX_TX_SIZE; i++) {
                //     printf("txData[%d]: 0x%X\r\n", i, txData[i]);
                // }
                printf("XFER ERROR\r\n");
                printf("DEVICE: 0x%02X, ADR: 0x%02X, TX: 0x%02X, RX: 0x%02X\r\n", DEVICE_ADDR, MEMORY_ADDR, txData[APP_RECEIVE_DUMMY_WRITE_LENGTH+0], rxData[0]);
                state = APP_STATE_IDLE;
                break;
            }
            default:
                break;
        }
    }
}


void I2C_APP_RX(uint8_t DEVICE_ADDR, uint8_t MEMORY_ADDR, uint8_t *RECIEVE_DATA, uint8_t RECEIVE_DATA_LENGTH)
{
    // printf("IIC_APP_RX\r\n");

    APP_STATES state = APP_STATE_EEPROM_STATUS_VERIFY;
    volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;
    uint8_t ackData = 0;

    uint8_t rxDataLength = RECEIVE_DATA_LENGTH;
    uint8_t txData[APP_RECEIVE_DUMMY_WRITE_LENGTH+MAX_TX_SIZE] = {0};
    uint8_t rxData[MAX_TX_SIZE] = {0};

    txData[0] = MEMORY_ADDR;

    while(1)
    {
        if (state == APP_STATE_IDLE) {
            break;
        }
        /* Check the application's current state. */
        switch (state)
        {
            case APP_STATE_IDLE:
                state = APP_STATE_IDLE;
                break;
            case APP_STATE_EEPROM_STATUS_VERIFY:

                /* Register the TWIHS Callback with transfer status as context */
                SERCOM2_I2C_CallbackRegister( APP_I2CCallback, (uintptr_t)&transferStatus );

               /* Verify if EEPROM is ready to accept new requests */
                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);

                state = APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS;
                break;

            case APP_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_EEPROM_READ;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                }
                break;

            case APP_STATE_EEPROM_READ:

                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                /* Read the data from the page written earlier */
                SERCOM2_I2C_WriteRead(DEVICE_ADDR, &txData[0], APP_RECEIVE_DUMMY_WRITE_LENGTH,  &rxData[0], rxDataLength);

                state = APP_STATE_EEPROM_WAIT_READ_COMPLETE;

                break;

            case APP_STATE_EEPROM_WAIT_READ_COMPLETE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    state = APP_STATE_VERIFY;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    state = APP_STATE_XFER_ERROR;
                }
                break;

            case APP_STATE_VERIFY:

                /* It means received data is same as transmitted data */
                state = APP_STATE_XFER_SUCCESSFUL;

                break;

            case APP_STATE_XFER_SUCCESSFUL:
            {
                LED_ON();
                printf("XFER SUCCESS\r\n");
                printf("DEVICE: 0x%02X, ADR: 0x%02X, DATA: 0x%02X\r\n", DEVICE_ADDR, MEMORY_ADDR, rxData[0]);
                memcpy(RECIEVE_DATA, rxData, rxDataLength);
                state = APP_STATE_IDLE;
                break;
            }
            case APP_STATE_XFER_ERROR:
            {
                LED_OFF();
                printf("XFER ERROR\r\n");
                state = APP_STATE_IDLE;
                break;
            }
            default:
                break;
        }
    }
}

/* *****************************************************************************
 End of File
 */
