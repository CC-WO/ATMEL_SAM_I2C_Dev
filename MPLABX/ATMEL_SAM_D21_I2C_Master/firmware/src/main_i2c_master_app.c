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

#define SLAVE_ADDRESS 0x54

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    uint8_t txData = 0x01;
    uint8_t rxData = 0x00;

    /* Initialize all modules */
    SYS_Initialize ( NULL );

    printf("ATMEL_SAM_D21_I2C_Master\r\n");

    I2C_APP_TX(SLAVE_ADDRESS, 0x3C, &txData, 1, VERIFY_ENABLE);
    I2C_APP_RX(SLAVE_ADDRESS, 0x3C, &rxData, 1);

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

