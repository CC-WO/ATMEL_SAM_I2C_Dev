/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes

#define LED_ON()                       LED_Clear()
#define LED_OFF()                      LED_Set()

#define APP_AT24MAC_DEVICE_ADDR             0x0054
#define APP_AT24MAC_MEMORY_ADDR             0x00
#define APP_TRANSMIT_DATA_LENGTH            5
#define APP_ACK_DATA_LENGTH                 1
#define APP_RECEIVE_DUMMY_WRITE_LENGTH      1
#define APP_RECEIVE_DATA_LENGTH             4

static uint8_t testTxData[APP_TRANSMIT_DATA_LENGTH] =
{
    APP_AT24MAC_MEMORY_ADDR,'M','C','H','P',
};

static uint8_t  testRxData[APP_RECEIVE_DATA_LENGTH];

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

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    APP_STATES state = APP_STATE_EEPROM_STATUS_VERIFY;
    volatile APP_TRANSFER_STATUS transferStatus = APP_TRANSFER_STATUS_ERROR;
    uint8_t ackData = 0;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    printf("ATMEL SAM D21 I2C Master\r\n");

    while(1)
    {
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
                SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);

                state = APP_STATE_EEPROM_WRITE;
                break;

            case APP_STATE_EEPROM_WRITE:

                if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                {
                    /* Write data to EEPROM */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &testTxData[0], APP_TRANSMIT_DATA_LENGTH);
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
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
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
                    state = APP_STATE_EEPROM_READ;
                }
                else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                {
                    /* EEPROM's internal write cycle is not complete. Keep checking. */
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM2_I2C_Write(APP_AT24MAC_DEVICE_ADDR, &ackData, APP_ACK_DATA_LENGTH);
                }
                break;

            case APP_STATE_EEPROM_READ:

                transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                /* Read the data from the page written earlier */
                SERCOM2_I2C_WriteRead(APP_AT24MAC_DEVICE_ADDR, &testTxData[0], APP_RECEIVE_DUMMY_WRITE_LENGTH,  &testRxData[0], APP_RECEIVE_DATA_LENGTH);

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

                if (memcmp(&testTxData[APP_RECEIVE_DUMMY_WRITE_LENGTH], &testRxData[0], APP_RECEIVE_DATA_LENGTH ) != 0)
                {
                    /* It means received data is not same as transmitted data */
                    state = APP_STATE_XFER_ERROR;
                }
                else
                {
                    /* It means received data is same as transmitted data */
                    state = APP_STATE_XFER_SUCCESSFUL;
                }
                break;

            case APP_STATE_XFER_SUCCESSFUL:
            {
                LED_ON();
                printf("XFER SUCCESS\r\n");
                printf("TX[0], RX[0]: %c, %c\r\n", (char)testTxData[APP_RECEIVE_DUMMY_WRITE_LENGTH+0], (char)testRxData[0]);
                printf("TX[1], RX[1]: %c, %c\r\n", (char)testTxData[APP_RECEIVE_DUMMY_WRITE_LENGTH+1], (char)testRxData[1]);
                printf("TX[2], RX[2]: %c, %c\r\n", (char)testTxData[APP_RECEIVE_DUMMY_WRITE_LENGTH+2], (char)testRxData[2]);
                printf("TX[3], RX[3]: %c, %c\r\n", (char)testTxData[APP_RECEIVE_DUMMY_WRITE_LENGTH+3], (char)testRxData[3]);
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
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }
}


/*******************************************************************************
 End of File
*/

