// This file has been prepared for Doxygen automatic documentation generation.
/*! \file ********************************************************************
*
* Atmel Corporation
*
* \li File:               eeprom.c
* \li Compiler:           IAR EWAAVR 3.10c
* \li Support mail:       avr@atmel.com
*
* \li Supported devices:  All devices with split EEPROM erase/write
*                         capabilities can be used.
*                         The example is written for ATmega48.
*
* \li AppNote:            AVR103 - Using the EEPROM Programming Modes.
*
* \li Description:        Example on how to use the split EEPROM erase/write
*                         capabilities in e.g. ATmega48. All EEPROM
*                         programming modes are tested, i.e. Erase+Write,
*                         Erase-only and Write-only.
*
*                         $Revision: 1.6 $
*                         $Date: Friday, February 11, 2005 07:16:44 UTC $
****************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef USER_FLASH
#define FLASH_30K_READ_ADDR 0xB600  // 30k位置用来存放数据 在存储空间的映射地址
#define FLASH_30k_WRITE_ADDR 0x7800 // 30k在flash中的物理地址
#endif

/* These EEPROM bits have different names on different devices. */
#ifndef EEPE
		#define EEPE  EEWE  //!< EEPROM program/write enable.
		#define EEMPE EEMWE //!< EEPROM master program/write enable.
#endif

/* These two are unfortunately not defined in the device include files. */
#define EEPM1 5 //!< EEPROM Programming Mode Bit 1.
#define EEPM0 4 //!< EEPROM Programming Mode Bit 0.

/* Define to reduce code size. */
#define EEPROM_IGNORE_SELFPROG //!< Remove SPM flag polling.

/*! \brief  Read byte from EEPROM.
 *
 *  This function reads one byte from a given EEPROM address.
 *
 *  \note  The CPU is halted for 4 clock cycles during EEPROM read.
 *
 *  \param  addr  EEPROM address to read from.
 *  \return  The byte read from the EEPROM address.
 */
unsigned char eeprom_get_char( unsigned int addr )
{
    EEARH = addr >> 8; // Set EEPROM address register.
    EEARL = addr & 0xff;
    EECR |= (1 << EERE); // Start EEPROM read operation.

    asm __volatile__ ("nop");
    asm __volatile__ ("nop");
    return EEDR; // Return the byte read from EEPROM.
}

/*! \brief  Write byte to EEPROM.
 *
 *  This function writes one byte to a given EEPROM address.
 *  The differences between the existing byte and the new value is used
 *  to select the most efficient EEPROM programming mode.
 *
 *  \note  The CPU is halted for 2 clock cycles during EEPROM programming.
 *
 *  \note  When this function returns, the new EEPROM value is not available
 *         until the EEPROM programming time has passed. The EEPE bit in EECR
 *         should be polled to check whether the programming is finished.
 *
 *  \note  The EEPROM_GetChar() function checks the EEPE bit automatically.
 *
 *  \param  addr  EEPROM address to write to.
 *  \param  new_value  New EEPROM value.
 */
void eeprom_put_char( unsigned int addr, unsigned char new_value )
{
    uint8_t	__bk_sreg = SREG;

    EEARH = addr >> 8;
    EEARL = addr & 0xff;

    EEDR = new_value;

    cli(); // Ensure atomic operation for the write operation.
    // program mode
    EECR = 0x04;
    EECR = 0x02;

    asm __volatile__ ("nop");
    asm __volatile__ ("nop");
    SREG = __bk_sreg;
    sei(); // Restore interrupt flag state.
}

#ifdef USER_FLASH

unsigned char flash_get_char( unsigned int addr )
{
	uint8_t *p ;
	p = FLASH_30K_BASE_ADDR + addr;
	return (uint8_t *)(p);
}

void flash_erase(void)
{
    EEAR = FLASH_30k_WRITE_ADDR;  // 写入最后1页/1K地址
	EECR = 0x94;
	EECR = 0x92;
	__asm__ __volatile__ ("nop" ::);
	__asm__ __volatile__ ("nop" ::);

}

void flash_put_char(unsigned int addr, unsigned charvalue)
{
    EEAR = FLASH_30k_WRITE_ADDR + addr&0x3;
	switch (addr%4) {
		case 0：
			E2PD0 = new_value;
			E2PD1 = 0;
			E2PD2 = 0;
			E2PD3 = 0;
			break;

		case 1:
			E2PD0 = 0;
			E2PD1 = new_value;
			E2PD2 = 0;
			E2PD3 = 0;
			break;
		case 2:
			E2PD0 = 0;
			E2PD1 = 0;
			E2PD2 = new_value;
			E2PD3 = 0;
			break;
			break;
		case 3:
			E2PD0 = 0;
			E2PD1 = 0;
			E2PD2 = 0;
			E2PD3 = new_value;
			break;
	}
    EECR = 0xA4;   // 编程使能
    EECR = 0xA2;   // 启动编程
	__asm__ __volatile__ ("nop" ::);
	__asm__ __volatile__ ("nop" ::);
}

void flash_write_byte(unsigned int addr, uint8_t *pData, uint16_t length)
{
	uint16_t i;
	flash_erase();
    for (i = 0; i < length/4; i += 4) 
    {
        EEAR = FLASH_30k_WRITE_ADDR + addr&0x3 + i;
		E2PD0 = *pData;
		E2PD1 = *(pData+1);
		E2PD2 = *(pData+2);
		E2PD3 = *(pData+3);
        cli();
    	EECR = 0xA0;    // 写FLASH模式
    	EECR |= 0x04;   // 编程使能
   	 	EECR |= 0x02;   // 启动编程
		__asm__ __volatile__ ("nop" ::);
		__asm__ __volatile__ ("nop" ::);
    }
	if (length%4)
	{
		EEAR = FLASH_30k_WRITE_ADDR + addr&0x3 + i;
		E2PD0 = *pData;
		E2PD1 = (length%4 == 2)? *(pData+1):0;
		E2PD2 = (length%4 == 3)? *(pData+2):0;
		E2PD3 = 0;
        cli();
    	EECR = 0xA0;    // 写FLASH模式
    	EECR |= 0x04;   // 编程使能
   	 	EECR |= 0x02;   // 启动编程
		__asm__ __volatile__ ("nop" ::);
		__asm__ __volatile__ ("nop" ::);
	}
}
#endif


// Extensions added as part of Grbl 
void memcpy_to_eeprom_with_checksum(unsigned int destination, char *source, unsigned int size) {
  unsigned char checksum = 0;
#ifdef USER_FLASH
  flash_write_4byte(unsigned int destination, char *source, unsigned int size);
#endif
  for(; size > 0; size--) { 
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += *source;

#ifndef USER_FLASH
   eeprom_put_char(destination, *(source++));
#endif
	destination++;
  }
#ifdef USER_FLASH
  flash_put_char(destination, checksum);
#else
  eeprom_put_char(destination, checksum);
#endif
  

}

int memcpy_from_eeprom_with_checksum(char *destination, unsigned int source, unsigned int size) {
  unsigned char data, checksum = 0;
  for(; size > 0; size--) {
#ifdef USER_FLASH
    data = flash_get_char();
#else
	eeprom_get_char(source++);
#endif 

    checksum = (checksum << 1) || (checksum >> 7);
    checksum += data;    
    *(destination++) = data; 
  }
#ifdef USER_FLASH
    return(checksum == flash_get_char(source));
#else
	return(checksum == eeprom_get_char(source));
#endif 

}

// end of file
