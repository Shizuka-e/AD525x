/* 
AD525x.cpp - Class file for AD5253/AD5254 digital potentiometer Arduino library.
*/

#include <AD525x.h>
#include <Arduino.h>

AD525x::AD525x(uint8_t AD_addr) {
    /* 
    Constructor - pass (AD1<<1 | AD0) to AD_addr to select the chosen device address.
    */
    if(AD_addr > 3) {
        AD_addr = 0x00;         // Not sure how to throw an error here.
    }
    dev_addr = base_I2C_addr | AD_addr;
    max_val = 0;
    err_code = 0;

    Wire.begin();               // Start I2C communications.
}

uint8_t AD525x::write_RDAC(uint8_t RDAC, uint8_t value) {
    /*
    Write value to RDAC register specified by RDAC. RDAC registers are zero-based index.

    Error handling:
    0: No error
    1-4: I2C errors.
    5-6: Value errors.
    */
    if(RDAC > 3) {
        err_code = EC_BAD_REGISTER;
        return err_code;
    }

    if(value > max_val) {
        err_code = EC_BAD_WIPER_SETTING;
        return err_code;
    }

    uint8_t instr_addr = RDAC_register | RDAC;
    err_code = write_data(instr_addr, value);
    return err_code;
}

uint8_t AD525x::read_RDAC(uint8_t RDAC) {
    /*
    Read RDAC value from register.

    On error, sets error code and returns 0. Check error code if 0 is returned (0 is also a valid 
    return value from the register).
    */
    if(RDAC > 3) {
        err_code = EC_BAD_REGISTER;
        return err_code;
    }

    uint8_t instr_addr = RDAC_register | RDAC;

    uint8_t *rv = read_data(instr_addr, 1);
    if(rv == NULL) {
        return 0;       // Err code set in read_data already.
    }

    return rv[0];
}

uint8_t AD525x::write_EEMEM(uint8_t reg, uint8_t value) {
    /*
    Write to the EEMEM non-volatile memory register. The first 4 registers (0, 1, 2, 3) are the 
    RDAC storage registers. The remainder can store user data. There are 16 total registers.
    */

    if(reg < 4 && value < max_val) {
        // Fairly sure the max value only applies to the RDAC registers, not the EEMEM.
        err_code = EC_BAD_WIPER_SETTING;
        return err_code;
    }

    if(reg > 15) {
        err_code = EC_BAD_REGISTER;
        return err_code;
    }

    uint8_t instr_addr = EEMEM_register | reg;

    err_code = write_data(instr_addr, value);
    return err_code;
}

uint8_t AD525x::read_EEMEM(uint8_t reg) {
    /*
    Read RDAC value from register.

    On error, sets error code and returns 0. Check error code if 0 is returned (0 is also a valid 
    return value from the register).
    */
    if(reg > 15) {
        err_code = EC_BAD_REGISTER;
        return err_code;
    }

    uint8_t instr_addr = EEMEM_register | reg;

    uint8_t *rv = read_data(instr_addr, 1);
    if(rv == NULL) {
        return 0;       // Err code set in read_data already.
    }

    return rv[0];
}

float AD525x::read_tolerance(uint8_t RDAC) {
    /*
    Reads the RAB tolerance, written at the factory, in percentage (signed float).

    Returns 0 on error. Because 0 is a valid output, check get_err_code() to determine if an
    error has occurred.
    */
    if (RDAC > 3) {
        err_code = EC_BAD_REGISTER;
        return 0;
    }

    uint8_t sign_mask = 0x80;

    // Shift RDAC by 1, low bit is integer / decimal.
    uint8_t instr_addr = Tolerance_register | (RDAC << 1);
    uint8_t instr_addr_int = instr_addr | Tol_int;
    uint8_t instr_addr_dec = instr_addr | Tol_dec;

    // 8-bit signed integer
    uint8_t tol_int_data = read_data_byte(instr_addr_int);
    if(err_code) {
        return 0;
    }

    // Fractional portion - 8 bits
    uint8_t tol_dec_data = read_data_byte(instr_addr_dec);
    if(err_code) {
        return 0;
    }

    // Convert to signed double
    float output;
    output = float((~sign_mask) & tol_int_data);
    if(sign_mask & tol_int_data) {
        output *= -1.0;
    }

    // This is almost certainly the wrong way to do this.
    for (int i = 0; i < 8; i++) {
        if ((sign_mask >> i) & tol_dec_data) {
            output += 1.0/((float)(2<<i));
        }
    }

    return output;
}

uint8_t AD525x::get_err_code() {
    /*
    Retrieve the error code stored in a private variable.
    */
    return err_code;
}

char * AD525x::get_error_text() {
    /*
    Retrieve the error string associated with the stored error code.
    */
    return get_error_string(err_code);
}

//
// Private functions: General I2C communications.
//
uint8_t AD525x::write_cmd(uint8_t cmd_register) {
    /*
    Trigger a command in the command register (no data)
    */
    Wire.beginTransmission(dev_addr);
    Wire.write(cmd_register);

    err_code = Wire.endTransmission();
    return err_code;

}

uint8_t AD525x::write_data(uint8_t register_addr, uint8_t data) {
    /*
    Write data to the specified register address.

    Error handling:
    0: No error
    1-4: I2C errors.
    */
    Wire.beginTransmission(dev_addr);
    Wire.write(register_addr);
    Wire.write(data);
    err_code = Wire.endTransmission();
    return err_code;
}

uint8_t * AD525x::read_data(uint8_t register_addr, uint8_t length) {
    /*
    Read data of length `length` from register register_addr

    Returns NULL on error, err_code will be set.
    */
    
    uint8_t error = 0;

    Wire.beginTransmission(dev_addr);
    Wire.write(register_addr);
    err_code = Wire.endTransmission();
    if(err_code > 0) {
        return NULL;
    }

    Wire.beginTransmission(dev_addr);
    uint8_t n_bytes = Wire.requestFrom(dev_addr, length);

    if(n_bytes != length) {
        err_code = EC_BAD_READ_SIZE;
        return NULL;
    }

    uint8_t buff[length];
    if(Wire.available() == length) {
        for(int i = 0; i < length; i++) {
            buff[i] = Wire.read();
        }
    }

    err_code = Wire.endTransmission();
    if(err_code > 0) {
        return NULL;
    }

    return buff;
}

uint8_t AD525x::read_data_byte(uint8_t register_addr) {
    /*
    Reads a single byte from the specified register. Convenience wrapper for read_data.

    Returns 0 on error - because this is a valid error, check get_err_code() if value is 0.
    */

    uint8_t *rv = read_data(register_addr, 1);
    if(rv == NULL) {
        return 0;       // Err code set in read_data already.
    }

    return rv[0];
}

char * AD525x::get_error_string(uint8_t error_code) {
    /*
    Get the error string from an error code. (private function)
    */

    switch(error_code) {
        case EC_NO_ERR:
            return EC_NO_ERR_str;
        case EC_DATA_LONG:
            return EC_DATA_LONG_str;
        case EC_NACK_ADDR:
            return EC_NACK_ADDR_str;
        case EC_NACK_DATA:
            return EC_NACK_DATA_str;
        case EC_I2C_OTHER:
            return EC_I2C_OTHER_str;
        case EC_BAD_REGISTER:
            return EC_BAD_REGISTER_str;
        case EC_BAD_WIPER_SETTING:
            return EC_BAD_WIPER_SETTING_str;
        case EC_BAD_READ_SIZE:
            return EC_BAD_READ_SIZE_str;
        default:
            return EC_UNKNOWN_ERR_str;
    }
}