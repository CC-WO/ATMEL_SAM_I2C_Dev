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
#include "definitions.h"                // SYS function prototypes
#include "i2cApp.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint8_t address = 0x00;
    uint8_t exist[255] = {0};
    // uint8_t txData = 0;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    // SYSTICK Settings
    SYSTICK_TimerStart();

    printf("ATMEL_SAM_D21_I2C_Master\r\n");

    for (address = 0x00; address < 0x78; address++) {
        if (I2C_APP_DEVICE_VERIFY(address, 5) == 1) {
            exist[address] = address;
        } else {
            exist[address] = 0x00;
        }
    }

    printf("    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\r\n");
    printf("00: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x00], exist[0x01], exist[0x02], exist[0x03], exist[0x04], exist[0x05], exist[0x06], exist[0x07],
            exist[0x08], exist[0x09], exist[0x0a], exist[0x0b], exist[0x0c], exist[0x0d], exist[0x0e], exist[0x0f]);
    printf("10: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x10], exist[0x11], exist[0x12], exist[0x13], exist[0x14], exist[0x15], exist[0x16], exist[0x17],
            exist[0x18], exist[0x19], exist[0x1a], exist[0x1b], exist[0x1c], exist[0x1d], exist[0x1e], exist[0x1f]);
    printf("20: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x20], exist[0x21], exist[0x22], exist[0x23], exist[0x24], exist[0x25], exist[0x26], exist[0x27],
            exist[0x28], exist[0x09], exist[0x2a], exist[0x2b], exist[0x2c], exist[0x2d], exist[0x2e], exist[0x2f]);
    printf("30: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x30], exist[0x31], exist[0x32], exist[0x33], exist[0x34], exist[0x35], exist[0x36], exist[0x37],
            exist[0x38], exist[0x39], exist[0x3a], exist[0x3b], exist[0x3c], exist[0x3d], exist[0x3e], exist[0x3f]);
    printf("40: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x40], exist[0x41], exist[0x42], exist[0x43], exist[0x44], exist[0x45], exist[0x46], exist[0x47],
            exist[0x48], exist[0x49], exist[0x4a], exist[0x4b], exist[0x4c], exist[0x4d], exist[0x4e], exist[0x4f]);
    printf("50: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x50], exist[0x51], exist[0x52], exist[0x53], exist[0x54], exist[0x55], exist[0x56], exist[0x57],
            exist[0x58], exist[0x59], exist[0x5a], exist[0x5b], exist[0x5c], exist[0x5d], exist[0x5e], exist[0x5f]);
    printf("60: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x60], exist[0x61], exist[0x62], exist[0x63], exist[0x64], exist[0x65], exist[0x66], exist[0x67],
            exist[0x68], exist[0x69], exist[0x6a], exist[0x6b], exist[0x6c], exist[0x6d], exist[0x6e], exist[0x6f]);
    printf("70: %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
            exist[0x70], exist[0x71], exist[0x72], exist[0x73], exist[0x74], exist[0x75], exist[0x76], exist[0x77],
            exist[0x78], exist[0x79], exist[0x7a], exist[0x7b], exist[0x7c], exist[0x7d], exist[0x7e], exist[0x7f]);

    // // txData = 0xC1; // 48k
    // txData = 0xC2; // 96k
    // I2C_APP_TX(0x4C, 0x40, &txData, 1, 1);
    // // txData = 0x07; // 48k
    // txData = 0x09; // 96k
    // I2C_APP_TX(0x4C, 0x41, &txData, 1, 1);
    // txData = 0x00;
    // I2C_APP_TX(0x4C, 0x42, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x43, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x44, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x45, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x46, &txData, 1, 1);
    // txData = 0xFF;
    // I2C_APP_TX(0x4C, 0x47, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x48, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x49, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4A, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4B, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4C, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4D, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4E, &txData, 1, 1);
    // I2C_APP_TX(0x4C, 0x4F, &txData, 1, 1);

    while ( true )
    {
        SYSTICK_DelayMs(100);

        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

