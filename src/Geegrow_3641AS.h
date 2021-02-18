/*!
 * @file Geegrow_3641AS.h
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

#ifndef GEEGROW_3641AS_H
#define GEEGROW_3641AS_H

#include <Arduino.h>
#include <Wire.h>

/******************************************************************************/
/*!
    @brief    Default I2C address of Geegrow_3641AS device. Actual addr may be
              got through using I2C_scanner sketch.
 */
/******************************************************************************/
#define DEV_I2C_ADDRESS      0x16

/******************************************************************************/
/*!
    @brief    Geegrow_3641AS symbols' IDs. Use this as user is not able to
              control separate segments of display.
 */
/******************************************************************************/
enum {
    DIGIT_0,
    DIGIT_1,
    DIGIT_2,
    DIGIT_3,
    DIGIT_4,
    DIGIT_5,
    DIGIT_6,
    DIGIT_7,
    DIGIT_8,
    DIGIT_9,
    DIGIT_MINUS,
    DIGIT_EMPTY,
    DIGIT_A,
    DIGIT_C,
    DIGIT_E,
    DIGIT_F,
    DIGIT_H,
    DIGIT_L,
    DIGIT_P,
    DIGIT_U
};
/******************************************************************************/
/*!
    @brief    Number of predefined symbols whih display can show.
 */
/******************************************************************************/
#define SYMBOLS_NUM     20

/******************************************************************************/
/*!
    @brief    Type to store data of digits
 */
/******************************************************************************/
typedef struct {
    uint8_t valueID;
    uint8_t dot;
} digitValue_t;

/******************************************************************************/
/*!
    @brief    Class that stores state and functions for interacting with display
 */
/******************************************************************************/
class Geegrow_3641AS 
{
    public:
        Geegrow_3641AS(uint8_t addr);
        Geegrow_3641AS(uint8_t addr, uint8_t pin);
        ~Geegrow_3641AS(){}
        void setDigit(uint8_t pos, uint8_t value, bool dot);
        int setInt(int value);
        int setFloat(float value, uint8_t fractNum);
        void setDot(uint8_t pos);
        void resetDot(uint8_t pos);
        void setAllDigits(uint8_t value0, uint8_t value1, uint8_t value2, uint8_t value3);
        void setAllDigits(uint8_t *values);
        void clear();
        void setDevAddr(uint8_t addr);

    private:
        void update();
        void writeSDPbyte(uint8_t byte);
        uint8_t dev_addr = 0;
        uint8_t SDP_pin = 0;
        bool I2C_mode = true;
        digitValue_t currentValues[4] = {
            {DIGIT_MINUS, false},
            {DIGIT_MINUS, false},
            {DIGIT_MINUS, false},
            {DIGIT_MINUS, false}
        };

};

#endif /* GEEGROW_3641AS_H */