/**
 * @brief Test cases for the Atmega328p GPIO driver.
 */
#include <cstdint>

#include <gtest/gtest.h>
#include <iostream>

#include "arch/avr/hw_platform.h"
#include "driver/gpio/atmega328p.h"
#include "utils/utils.h"

#ifdef TESTSUITE

namespace driver
{
namespace
{
/** Number of available pins. */
constexpr std::uint8_t PinCount{20U};

// -----------------------------------------------------------------------------
constexpr bool isPinValid(const std::uint8_t pin) noexcept { return PinCount > pin; }

/**
 * @brief GPIO initialization test.
 * 
 *        Verify that only one instance per valid pin can be used at once.
 */
TEST(Gpio_Atmega328p, Initialization)
{
    constexpr std::uint8_t pinMax{50U};

    // Systematically test GPIO initilization across a range of pin numbers.
    for (std::uint8_t pin{}; pin < pinMax; ++pin)
    {
        // Create a new GPIO instance.
        // Expect the instance to be initialized correctly if the pin is valid.
        gpio::Atmega328p gpio{pin, gpio::Direction::Input};
        EXPECT_EQ(gpio.isInitialized(), isPinValid(pin));

        // Create another GPIO instance on the same pin.
        // Expect the instance to not be initialized, since the pin is already reserved.
        gpio::Atmega328p other{pin, gpio::Direction::Input};
        EXPECT_FALSE(other.isInitialized());
    }
}

/**
 * @brief GPIO output test.
 * 
 *        Verify that GPIO outputs can be used for reading and writing.
 */
TEST(Gpio_Atmega328p, Output)
{
    constexpr std::uint8_t portd0{0};
    constexpr std::uint8_t portb0{8U};
    //constexpr std::uint8_t portc0{13U};

    // Systematically test I/O port D.
    for (std::uint8_t pin{portd0}; pin < portb0; ++pin)
    {
        // Create a new GPIO output.
        // Expect the instance to be initialized correctly if the pin is valid.
        gpio::Atmega328p gpio{pin, gpio::Direction::Output};
        EXPECT_EQ(gpio.isInitialized(), isPinValid(pin));
        
        // Expect the GPIO to be set as output, i.e. the corresponding bit in DDRD is set.
        EXPECT_TRUE(utils::read(DDRD, pin));

        // Set the output high, expect the corresponding bit in PORTD to be set.
        gpio.write(true);
        EXPECT_TRUE(utils::read(PORTD, pin));

        // Toggle the output, expect the corresponding bit in PORTD to be cleared.
        gpio.toggle();
        EXPECT_TRUE(utils::read(PIND, pin));

    }
}
} // namespace
} // namespace driver

#endif /** IFDEF TESTSUITE */
