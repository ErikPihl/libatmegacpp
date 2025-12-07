/**
 * @brief Test cases for the Atmega328p ADC.
 */
#include <cstdint>

#include <gtest/gtest.h>

#include "arch/avr/hw_platform.h"
#include "driver/adc/atmega328p.h"
#include "utils/utils.h"

#ifdef TESTSUITE

namespace driver
{
namespace
{
// -----------------------------------------------------------------------------
constexpr double computeDutyCycle(const std::uint16_t adcVal) noexcept
{
    // Convert the ADC value to a duty cycle.
    constexpr std::uint16_t adcMax{1023U};
    return static_cast<double>(adcVal) / adcMax;
}

// -----------------------------------------------------------------------------
constexpr double computeInputVoltage(const std::uint16_t adcVal) noexcept
{
    // Convert the ADC value to a voltage.
    constexpr double supplyVoltage{5.0};
    return computeDutyCycle(adcVal) * supplyVoltage;
}

/**
 * @brief ADC read test.
 * 
 *        Verify that the ADC returns the expected values when an invalid pin number is used,
 *        otherwise 0.
 */
TEST(Adc_Atmega328p, Read)
{
    // Set up the ADC.
    utils::set(ADCSRA, ADIF);
    adc::Interface& adc{adc::Atmega328p::getInstance()};

     // Expect that the ADC was initialized successfully.
    EXPECT_TRUE(adc.isInitialized());

    // Try 20 different pin numbers.
    constexpr std::uint8_t pinMax{20U};

    for (std::uint8_t pin{}; pin <= pinMax; ++pin)
    {
        // Check if the pin is within the valid range for the ADC.
        const bool isValid{adc.isChannelValid(pin)};

        // Sample ten ADC values between 0 - 1000.
        constexpr std::uint16_t lastVal{1000U};
        constexpr std::uint16_t stepVal{100U};

        for (std::uint16_t adcVal{}; adcVal <= lastVal; adcVal += stepVal)
        {
            // For valid pins, the ADC should return the expected ADC value, as well as the 
            // corresponding duty cycle and input voltage.
            // For invalid pins, the ADC should always return 0.
            if (isValid) 
            { 
                ADC = adcVal;
                EXPECT_EQ(adc.read(pin), adcVal); 
                EXPECT_EQ(adc.dutyCycle(pin), computeDutyCycle(adcVal));
                EXPECT_EQ(adc.inputVoltage(pin), computeInputVoltage(adcVal));
            }
            else 
            { 
                constexpr std::int16_t defaultAdcVal{0U};
                ADC = adcVal;
                EXPECT_EQ(adc.read(pin), defaultAdcVal); 
                EXPECT_EQ(adc.dutyCycle(pin), defaultAdcVal);
                EXPECT_EQ(adc.inputVoltage(pin), defaultAdcVal);
            }
        }
    }
}
} // namespace
} // namespace driver

#endif /** IFDEF TESTSUITE */
