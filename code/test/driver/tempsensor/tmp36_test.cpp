/**
 * @brief Test cases for the TMP36 temperature sensor.
 */
#include <cstdint>
#include <limits>
#include <memory>

#include <gtest/gtest.h>

#include "arch/avr/hw_platform.h"
#include "driver/adc/atmega328p.h"
#include "driver/tempsensor/tmp36.h"
#include "utils/utils.h"

namespace driver
{
namespace
{
// -----------------------------------------------------------------------------
constexpr double computeInputVoltage(const std::uint16_t adcVal) noexcept
{
    constexpr double supplyVoltage{5.0};
    constexpr std::uint16_t adcMax{1023U};

    // Convert the ADC value to a voltage.
    return static_cast<double>(adcVal) / adcMax * supplyVoltage;
}

// -----------------------------------------------------------------------------
constexpr std::int16_t convertToTemp(const double inputVoltage) noexcept
{
    // Convert voltage to temperature: T(°C) = 100 * V - 50.
    return utils::round<std::int16_t>(100.0 * inputVoltage - 50.0); 
}

// -----------------------------------------------------------------------------
constexpr std::int16_t convertToTemp(const std::uint16_t adcVal) noexcept
{
    // Convert ADC value to voltage, then to temperature.
    return convertToTemp(computeInputVoltage(adcVal));
}

/**
 * @brief Temp sensor initilaization test.
 * 
 *        Verify that invalid pin numbers are not accepted and the sensor is not initialized 
 *        for out-of-range values.
 */
TEST(TempSensor_Tmp36, Initialization)
{
    constexpr std::int16_t defaultTemp{0};
    constexpr std::uint16_t adcVal{100U};

    // Set up the ADC.
    utils::set(ADCSRA, ADIF);
    ADC = adcVal;
    adc::Interface& adc{adc::Atmega328p::getInstance()};

     // Expect that the ADC was initialized successfully.
    EXPECT_TRUE(adc.isInitialized());

    // Try all pin numbers 0 - 255.
    for (std::size_t i{}; i <= std::numeric_limits<std::uint8_t>::max(); ++i)
    {
        // Create a TMP36 sensor instance for this pin.
        const std::uint8_t pin{static_cast<std::uint8_t>(i)};
        tempsensor::Tmp36 tempSensor{pin, adc};

        // Check if the pin is within the valid range for the ADC.
        const bool isValid{adc.isChannelValid(pin)};
        EXPECT_EQ(tempSensor.isInitialized(), isValid);

        if (isValid)
        {
            // For valid pins, the sensor should be initialized and return the expected temperature.
            const std::int16_t expectedTemp{convertToTemp(adcVal)};
            EXPECT_TRUE(tempSensor.isInitialized());
            EXPECT_EQ(tempSensor.read(), expectedTemp);
        }
        else
        {
            // For invalid pins, the sensor should not be initialized and should return the 
            // default temperature.
            EXPECT_FALSE(tempSensor.isInitialized());
            EXPECT_EQ(tempSensor.read(), defaultTemp);
        }
    }
}

/**
 * @brief Temp sensor accuray test.
 * 
 *        Verify that the temp sensor predicts accurately.
 */
TEST(TempSensor_Tmp36, Accuracy)
{
    constexpr std::uint8_t tempSensorPin{0U};
    constexpr std::size_t stepVal{10U};

    // Set up the ADC.
    utils::set(ADCSRA, ADIF);
    adc::Interface& adc{adc::Atmega328p::getInstance()};

    // Expect that the ADC was initialized successfully and that the sensor pin is within the 
    // valid range for the ADC.
    EXPECT_TRUE(adc.isInitialized());
    EXPECT_TRUE(adc.isChannelValid(tempSensorPin));
    
    // Set up the temperature sensor, use the interface this time.
    std::unique_ptr<tempsensor::Interface> tempSensor{
        std::make_unique<tempsensor::Tmp36>(tempSensorPin, adc)};

    // Expect the temperature sensor to be initialized successfully.
    EXPECT_TRUE(tempSensor->isInitialized());

    // Try different ADC values to simulate different input voltages.
    for (std::size_t i{}; i <= std::numeric_limits<std::uint16_t>::max(); i += stepVal)
    {
        // Calculate the expected temperature for this ADC value.
        const std::uint16_t adcVal{static_cast<std::uint16_t>(i)};
        const std::int16_t expectedTemp{convertToTemp(adcVal)};

        // Set the ADC register to simulate the sensor reading.
        ADC = adcVal;

        // The sensor should return the expected temperature for this ADC value.
        EXPECT_EQ(tempSensor->read(), expectedTemp);
    }
}
} // namespace
} // namespace driver