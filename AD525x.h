/** @file 
Header file for AD5253/AD5254 digital potentiometer Arduino library.
*/

#ifndef AD525X_h
#define AD525X_h

#include <Arduino.h>
#include <Wire.h>

#define base_I2C_addr 0x2C              /*!< Base address of these devices. Full address is 
                                           `base_I2C_addr | (AD1 << 1) | AD0`            */

/** @{ */
// AD5253 and AD5254 differ based on what the maximum wiper setting is.
#define AD5253_max 63                  /*!< Maximum wiper value for AD5253 */
#define AD5254_max 255                 /*!< Maximum wiper value for AD5254 */
/** @} */

/** @{ */
// Instruction registers: These are the top 3 bits of the instruction registers.
#define RDAC_register 0x00             /*!< Read/write RDAC, bottom 2 bits are the 4 wipers */
#define EEMEM_register 0x20            /*!< Read/write EEMEM, bottom 4 bits are the 16 regs. */

#define Tolerance_register 0x38        /*!< Read factory tolerances (Read-only) [RDAC is A2, A1] */
#define Tol_int 0x00                   /*!< Low bit of the tolerance register sets int or decimal */
#define Tol_dec 0x01
/**@}*/


/** 
 \defgroup ErrorCodes Error codes
 @{ 
 */
#define EC_NO_ERR 0
#define EC_NO_ERR_str "No error."

#define EC_DATA_LONG 1
#define EC_DATA_LONG_str "Data too long to fit in transmit buffer."

#define EC_NACK_ADDR 2
#define EC_NACK_ADDR_str "Received NACK on transmit of address."

#define EC_NACK_DATA 3
#define EC_NACK_DATA_str "Received NACK on transmit of data."

#define EC_I2C_OTHER 4
#define EC_I2C_OTHER_str "Other I2C error."

#define EC_BAD_REGISTER 5
#define EC_BAD_REGISTER_str "Invalid register."

#define EC_BAD_WIPER_SETTING 6
#define EC_BAD_WIPER_SETTING_str "Invalid wiper setting."

#define EC_BAD_READ_SIZE 7
#define EC_BAD_READ_SIZE_str "Invalid number of bytes read from register."

#define EC_BAD_DEVICE_ADDR 8
#define EC_BAD_DEVICE_ADDR_str "Bad device address - device address must be in [0, 3]."

#define EC_NOT_IMPLEMENTED 9
#define EC_NOT_IMPLEMENTED_str "Function not implemented on interface."

#define EC_UNKNOWN_ERR_str "Unknown error."
/**@}*/

class AD525x {
// This is a parent class - use AD5253 or AD5254 as necessary.
public:
    AD525x(uint8_t AD_addr);

    uint8_t write_RDAC(uint8_t RDAC, uint8_t value);
    uint8_t read_RDAC(uint8_t RDAC);

    uint8_t write_EEMEM(uint8_t reg, uint8_t value);
    uint8_t read_EEMEM(uint8_t reg);

    float read_tolerance(uint8_t RDAC);

    // Device commands
    uint8_t reset_device(void);

    uint8_t restore_RDAC(uint8_t RDAC);
    uint8_t restore_all_RDAC(void);
    uint8_t store_RDAC(uint8_t RDAC);

    uint8_t decrement_RDAC(uint8_t RDAC);
    uint8_t increment_RDAC(uint8_t RDAC);
    uint8_t decrement_RDAC_6dB(uint8_t RDAC);
    uint8_t increment_RDAC_6dB(uint8_t RDAC);

    uint8_t decrement_all_RDAC(void);
    uint8_t increment_all_RDAC(void);
    uint8_t decrement_all_RDAC_6dB(void);
    uint8_t increment_all_RDAC_6dB(void);

    // For class inheritance
    virtual uint8_t get_max_val(void);          /*!< Not implemented for top-level AD525x. */

    // Error handling
    uint8_t get_err_code(void);
    char *get_error_text(void);

protected:
    uint8_t max_val;        

private:
    uint8_t write_cmd(uint8_t cmd_register);

    uint8_t write_data(uint8_t register_addr, uint8_t data);
    uint8_t *read_data(uint8_t register_addr, uint8_t length);
    uint8_t read_data_byte(uint8_t register_addr);

    uint8_t dev_addr;       /*!< The full 7-bit address of the specified device. */
    uint8_t err_code;       /*!< Used for error detection. Access via get_err_code() and 
                                 get_error_text() */

    static const uint8_t CMD_NOP = 0x80;            /*!< Return device to idle state */
    static const uint8_t CMD_Restore_RDAC = 0x88;
    static const uint8_t CMD_Store_RDAC = 0x90;
    static const uint8_t CMD_Dec_RDAC_6dB = 0x98;
    static const uint8_t CMD_Dec_All_RDAC_6dB = 0xa0;            /*!< Return device to idle state */
    static const uint8_t CMD_Dec_RDAC_step = 0xa8;
    static const uint8_t CMD_Dec_All_RDAC_step = 0xb0;
    static const uint8_t CMD_Restore_All_RDAC = 0xb8;
    static const uint8_t CMD_Inc_RDAC_6dB = 0xc0;
    static const uint8_t CMD_Inc_All_RDAC_6dB = 0xc8;
    static const uint8_t CMD_Inc_RDAC_step = 0xd0;
    static const uint8_t CMD_Inc_All_RDAC_step = 0xd8;
};

class AD5253 : public AD525x {
public:
    AD5253(uint8_t AD_addr) : AD525x(AD_addr) {};

    uint8_t get_max_val(void);
private:
    static const uint8_t max_val = 63;          /*!< Maximum wiper value. 63 for AD5253 */
};

class AD5254 : public AD525x {
public:
    AD5254(uint8_t AD_addr) : AD525x(AD_addr) {};

    uint8_t get_max_val(void);
private:
    static const uint8_t max_val = 255;         /*!< Maximum wiper value. 255 for AD5254 */
};


#endif