/*!
 * @file Geegrow_3641AS.cpp
 *
 * This is a library for the Geegrow 7-segment 4-digit I2C display.
 * https://www.geegrow.ru
 *
 * @section author Author
 * Written by Anton Pomazanov
 *
 * @section license License
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Geegrow_3641AS.h"

/******************************************************************************/
/*!
    @brief    Constructor for I2C mode
    @param    addr    I2C address of device
 */
/******************************************************************************/
Geegrow_3641AS::Geegrow_3641AS(uint8_t addr)
{
    this->dev_addr = addr;
    this->I2C_mode = true;
}

/******************************************************************************/
/*!
    @brief    Constructor for SDP mode
    @param    addr    I2C address of device
    @param    pin     Pin for SDP mode
 */
/******************************************************************************/
Geegrow_3641AS::Geegrow_3641AS(uint8_t addr, uint8_t pin)
{
    this->dev_addr = addr;
    this->I2C_mode = false;
    this->SDP_pin = pin;
    pinMode(this->SDP_pin, OUTPUT);
    digitalWrite(this->SDP_pin, HIGH);
}

/******************************************************************************/
/*!
    @brief    Set certain digit to selected position
    @param    pos       Position of digit on display
    @param    value     Which digit to show
    @param    dot       Set if dot needed
 */
/******************************************************************************/
void Geegrow_3641AS::setDigit(uint8_t pos, uint8_t value, bool dot)
{
    if (pos > 3)
        return;
    if (value >= SYMBOLS_NUM)
        value = DIGIT_MINUS;

    if (dot) {
        this->currentValues[pos].dot = 1;
    }
    this->currentValues[pos].valueID = value;
    this->update();
}

/******************************************************************************/
/*!
    @brief    Set integer value on display
    @param    value    Integer to show
 */
/******************************************************************************/
int Geegrow_3641AS::setInt(int value) 
{
    if ( (value > 9999) || (value < -999) ) {
        return -1;
    }
    
    for (uint8_t i = 0; i < 4; ++i) {
        this->currentValues[i].valueID = DIGIT_EMPTY;
        this->currentValues[i].dot = 0;
    }

    bool isNeg = false;
    if (value < 0) {
        isNeg = true;
        value *= -1;
    }

    uint8_t digits[4];
    uint8_t digitsNum = 0;
    do {
        digits[digitsNum] = value % 10;
        digitsNum++;
        value /= 10;
    } while (value > 0);

    for (uint8_t i = 0; i < digitsNum; ++i) {
        this->currentValues[3 - i].valueID = digits[i];
    }

    if (isNeg) {
        this->currentValues[3 - digitsNum].valueID = DIGIT_MINUS;
    }

    this->update();
}

/******************************************************************************/
/*!
    @brief    Set float value on display
    @param    value       Float to show
    @param    fractNum    Desired number of digits from fractional part. If not
                          enough space, less quantity will be shown
 */
/******************************************************************************/
int Geegrow_3641AS::setFloat(float value, uint8_t fractNum)
{
    if ( (value >= 10000.0) || (value <= -1000.0) ) {
        return -1;
    }
    
    for (uint8_t i = 0; i < 4; ++i) {
        this->currentValues[i].valueID = DIGIT_EMPTY;
        this->currentValues[i].dot = 0;
    }

    /* Need to know if we can display required number of digits after dot */
    uint8_t freePos = 4;

    /* If value is neg, one pos is reserved for '-' */
    bool isNeg = false;
    if (value < 0) {
        isNeg = true;
        value *= -1;
        freePos--;
    }

    /* Integral part */

    int temp_int = (int)value;
    /* Get number of int digits */
    uint8_t int_digits[4];
    uint8_t int_digitsNum = 0;
    do {
        int_digits[int_digitsNum] = temp_int % 10;
        int_digitsNum++;
        temp_int /= 10;
    } while (temp_int > 0);

    /* Check free positions */
    freePos -= int_digitsNum;

    /* Fractional part */

    if (freePos == 0) {
        /* Have space only for integral part. Show it and set dot */
        for (uint8_t i = 0; i < int_digitsNum; ++i) {
            this->currentValues[3 - i].valueID = int_digits[i];
        }
        if (isNeg) {
            this->currentValues[3 - int_digitsNum].valueID = DIGIT_MINUS;
        }
        this->currentValues[3].dot = 1;
        this->update();
        return 1;
    }

    /* Remove integral part */
    value -= (int)value;

    uint8_t fract_final = 0;

    if (freePos >= fractNum) {
        /* Enough free space, will show fractNum fractional digits */
        fract_final = fractNum;
    } else {
        /* Not enough space, show only head digits */
        fract_final = freePos;
    }

    /* Make new integral part */
    value *= pow(10, fract_final);
    int temp_frac = (int)value;
    uint8_t frac_digits[4];
    uint8_t frac_digitsNum = 0;
    do {
        frac_digits[frac_digitsNum] = temp_frac % 10;
        frac_digitsNum++;
        temp_frac /= 10;
    } while (temp_frac > 0);

    /* Fill digits */
    uint8_t pos = 3;
    /* Fract part */
    for (uint8_t i = 0; i < fract_final; ++i) {
        this->currentValues[pos--].valueID = frac_digits[i];
    }
    /* Dot */
    this->currentValues[pos].dot = 1;
    /* Integral part */
    for (uint8_t i = 0; i < int_digitsNum; ++i) {
        this->currentValues[pos--].valueID = int_digits[i];
    }
    if (isNeg) {
        this->currentValues[pos].valueID = DIGIT_MINUS;
    }

    this->update();
}

/******************************************************************************/
/*!
    @brief    Set dot to certain position on display
    @param    pos    Position for dot
 */
/******************************************************************************/
void Geegrow_3641AS::setDot(uint8_t pos)
{
    this->currentValues[pos].dot = 1;
    this->update();
}

/******************************************************************************/
/*!
    @brief    Remove dot from certain position on display
    @param    pos    Position where dot is not needed
 */
/******************************************************************************/
void Geegrow_3641AS::resetDot(uint8_t pos)
{
    this->currentValues[pos].dot = 0;
    this->update();
}

/******************************************************************************/
/*!
    @brief    Set 4 digits by one command. Digits are set by separate arguments
    @param    value0       Digit for position 0 on display
    @param    value1       Digit for position 1 on display
    @param    value2       Digit for position 2 on display
    @param    value3       Digit for position 3 on display
    @note     Dots are not handled by this function.
 */
/******************************************************************************/
void Geegrow_3641AS::setAllDigits(uint8_t value0, uint8_t value1, uint8_t value2, uint8_t value3)
{
    if (value0 >= SYMBOLS_NUM)
        value0 = DIGIT_MINUS;
    if (value1 >= SYMBOLS_NUM)
        value1 = DIGIT_MINUS;
    if (value2 >= SYMBOLS_NUM)
        value2 = DIGIT_MINUS;
    if (value3 >= SYMBOLS_NUM)
        value3 = DIGIT_MINUS;

    this->currentValues[0].valueID = value0;
    this->currentValues[1].valueID = value1;
    this->currentValues[2].valueID = value2;
    this->currentValues[3].valueID = value3;
    this->update();
}

/******************************************************************************/
/*!
    @brief    Set 4 digits by one command. Digits are set by buffer[4] of values
    @param    values       Digits for showing on display. Buffer must contain at 
                           least 4 elements to avoid errors
    @note     Dots are not handled by this function.
 */
/******************************************************************************/
void Geegrow_3641AS::setAllDigits(uint8_t *values)
{
    for (uint8_t i = 0; i < 4; ++i) {
        if (*(values + i) >= SYMBOLS_NUM) {
            *(values + i) = DIGIT_MINUS;
        }

        this->currentValues[i].valueID = *(values + i);
    }
    this->update();
}

/******************************************************************************/
/*!
    @brief    Send data to display using selected transmission mode
 */
/******************************************************************************/
void Geegrow_3641AS::update()
{
    uint8_t data[4];
    for (uint8_t i = 0; i < 4; ++i) {
        data[i] = this->currentValues[i].valueID;
        if (this->currentValues[i].dot) {
            data[i] |= 0b10000000;
        }
    }

    if (this->I2C_mode) {
        Wire.beginTransmission(this->dev_addr);
        Wire.write(data, 4);
        Wire.endTransmission();
    } else {
        /* Transmit 5 bytes: addr + 4 data */
        this->writeSDPbyte(this->dev_addr);
        for (uint8_t i = 0; i < 4; ++i) {
            this->writeSDPbyte(data[i]);
        }
    }
}

/******************************************************************************/
/*!
    @brief    Send byte using SDP mode
    @param    byte       Byte to write
 */
/******************************************************************************/
void Geegrow_3641AS::writeSDPbyte(uint8_t byte)
{
    /* Start bit */
    digitalWrite(this->SDP_pin, LOW);
    delay(3);
    /* 8 data bits */
    for (int8_t i = 7; i >= 0; i--) {
        if (byte & (1 << i)) {
            digitalWrite(this->SDP_pin, HIGH);
            delay(2);
        } else {
            digitalWrite(this->SDP_pin, LOW);
            delay(2);
        }
    }
    /* Stop bit */
    digitalWrite(this->SDP_pin, HIGH);
    delay(2);
}

/******************************************************************************/
/*!
    @brief    Clear display
 */
/******************************************************************************/
void Geegrow_3641AS::clear()
{
    for (uint8_t i = 0; i < 4; ++i) {
        this->currentValues[i].valueID = DIGIT_EMPTY;
        this->currentValues[i].dot = 0;
    }
    this->update();
}

/******************************************************************************/
/*!
    @brief    Change device address ( for both I2C and SDP modes)
    @param    addr       New address
 */
/******************************************************************************/
void Geegrow_3641AS::setDevAddr(uint8_t addr)
{
    this->currentValues[0].valueID = 65;
    this->currentValues[1].valueID = 68;
    this->currentValues[2].valueID = 82;
    this->currentValues[3].valueID = addr;
    this->update();

    this->dev_addr = addr;
}
