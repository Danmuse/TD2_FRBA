/*!
 * @file comm.h
 * @par Author & Doxygen Editor
 *  Daniel Di Módica ~ <a href = "mailto: danifabriziodmodica@gmail.com">danifabriziodmodica@@gmail.com</a>
 * @date 20/04/2023 13:36:24
 * @version 1.0
 * @brief comm API
 * @details @par Description
 * 	The <b>comm</b> library provides an easy and reliable interface for handling error messages, peripheral initialization, and I2C communication status. @n 
 *  Functions in this library are especially useful in applications where data communication is critical and it is necessary to guarantee the integrity of the transmitted data.
 */

#ifndef _COMM_H_
#define _COMM_H_

#include "utils.h"
#include <SPI.h> //!< Serial Peripheral Interface (<tt>SPI</tt>) communication methods.
#include <Wire.h> //!< Inter-IC (<tt>I2C</tt>) communication methods.

#define SPI_DUMMY_QUERY 0xFF

//! @hide initializer Defines the speed of Serial Peripheral Interface (<tt>SPI</tt>) communication for the <a target = "__blank" href = "https://www.arduino.cc/en/Reference/SPI"><tt><b><SPI.h></b></tt></a> library. The value is defined in bits per second (<tt>bps</tt>) and determines the maximum data transfer rate.
#define SPI_PERIPHERAL_DEFAULT_SPEED (4000000UL) //!< <pre><strong>Value:</strong> 4000000
#define SPI_PERIPHERAL_SPEED (36000000UL) //!< <pre><strong>Value:</strong> 36000000

//! @hideinitializer Defines the speed of Inter-IC (<tt>I2C</tt>) communication for the <a target = "__blank" href = "https://reference.arduino.cc/reference/en/language/functions/communication/wire/"><tt><b><Wire.h></b></tt></a> library. The value is defined in bits per second (<tt>bps</tt>) and determines the maximum data transfer rate.
#define WIRE_SPEED (400000UL) //!< <pre><strong>Value:</strong> 400000

//! @brief <b>statusI2C_t</b> enumeration indicates the status and faults of Inter-IC (<tt>I2C</tt>) communication.
typedef enum {
	I2C_SUCCESS     = 0, //!< Successful operation.
	I2C_BUFFER      = 1, //!< Indicates that the data to be transmitted was too long to fit in the transmit buffer.
	I2C_ADDR_NACK   = 2, //!< Indicates that the slave device did not acknowledge the transmit of the address byte.
	I2C_DATA_NACK   = 3, //!< Indicates that the slave device did not acknowledge the transmit of a data byte.
	I2C_OTHER_ERROR = 4  //!< Other error has occurred.
} statusI2C_t;

//! @hideinitializer Defines a string literal that describes a message indicating that the MicroSD card was not found.
#define ERR_SD_NOT_FOUND "MICRO SD CARD\\nNOT FOUND"
//! @hideinitializer Defines a string literal that describes a message indicating that the current section was not implmented.
#define ERR_SECTION_NOT_AVAILABLE "NOT AVAILABLE"

static void spiTransaction(SPISettings settings, void (*operation)());

static void spiTransaction(SPISettings settings, void (*operation)()) {
    // SPI.endTransaction();
    SPI.end(); SPI.begin();
    SPI.beginTransaction(settings);
    operation();
}

#endif // _COMM_H_
