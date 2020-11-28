/* ************************************************************************** */
/** Descriptive File Name

  @Company
    YAMAHA CORPORATION

  @File Name
    eepromEmu.h

  @Summary
    EEPROM emulator.

  @Description

 */
/* ************************************************************************** */

#ifndef _EEPROM_EMU_H    /* Guard against multiple inclusion */
#define _EEPROM_EMU_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */


#define EEPROM_PAGE_SIZE_BYTES                  256
#define EEPROM_PAGE_SIZE_MASK                   0xFF
#define EEPROM_SIZE_BYTES                       256

typedef enum
{
    EEPROM_CMD_WRITE,
    EEPROM_CMD_IDLE,
} EEPROM_CMD;

typedef struct
{
    /* currentAddrPtr - to allow for sequential read (from the current address) */
    uint8_t                     currentAddrPtr;
    /* addrIndex - used to copy 2 bytes of EEPROM memory address */
    uint8_t                     addrIndex;
    /* wrBuffer - holds the incoming data from the I2C master */
    uint8_t                     wrBuffer[EEPROM_PAGE_SIZE_BYTES];
    /* wrBufferIndex - Index into the wrBuffer[] */
    uint16_t                    wrBufferIndex;
    /* wrAddr - indicates the starting address of the EEPROM emulation memory to write to */
    volatile uint16_t           wrAddr;
    /* nWrBytes - indicates the number of bytes to write to EEPROM emulation buffer */
    volatile uint8_t            nWrBytes;
    /* internalWriteInProgress - indicates that EEPROM is busy with internal writes */
    bool                        internalWriteInProgress;
    /* eepromCommand - used to trigger write to the EEPROM emulation buffer */
    EEPROM_CMD                  eepromCommand;
} EEPROM_DATA;

extern uint8_t EEPROM_EmulationBuffer[EEPROM_SIZE_BYTES];
extern EEPROM_DATA eepromData;

bool APP_SERCOM_I2C_Callback ( SERCOM_I2C_SLAVE_TRANSFER_EVENT event, uintptr_t contextHandle );
void EEPROM_StateMachine(void);

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
