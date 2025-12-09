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
/**
 * @brief GPIO register structure.
 */
struct GpioRegs
{
    /** Data direction register. */
    volatile std::uint8_t& ddrx;

    /** Port register. */
    volatile std::uint8_t& portx;

    /** Pin register. */
    volatile std::uint8_t& pinx;
};

/**
 * @brief Pin offset structure.
 */
struct PinOffset
{
    /** Pin offset for I/O port D. */
    static constexpr std::uint8_t D{0U};

    /** Pin offset for I/O port B. */
    static constexpr std::uint8_t B{8U};

    /** Pin offset for I/O port C. */
    static constexpr std::uint8_t C{14U};
};

/** Number of available pins. */
constexpr std::uint8_t PinCount{20U};

// -----------------------------------------------------------------------------
constexpr bool isPinValid(const std::uint8_t pin) noexcept { return PinCount > pin; }

// -----------------------------------------------------------------------------
constexpr std::uint8_t getPhysicalPin(const std::uint8_t pin) noexcept
{
    if (!isPinValid(pin))        { return static_cast<std::uint8_t>(-1); }
    if (PinOffset::B > pin)      { return pin; } 
    else if (PinOffset::C > pin) { return pin - PinOffset::B; }
    return pin - PinOffset::C;
}

// -----------------------------------------------------------------------------
constexpr void simulateToggle(GpioRegs& regs) noexcept
{
    constexpr std::uint8_t bitCount{8U};

    for (std::uint8_t pin{}; pin < bitCount; ++pin)
    {
        // Toggle the output of a given pin if configured as output and the pin bit has been set.
        if (utils::read(regs.ddrx, pin) && utils::read(regs.pinx, pin))
        {
            utils::toggle(regs.portx, pin);
            utils::clear(regs.pinx, pin);
        }
    }
}

// -----------------------------------------------------------------------------
void runOutputTest(const std::uint8_t pin, GpioRegs& regs) noexcept
{
    // Get the physical pin on the given port.
    const std::uint8_t physPin{getPhysicalPin(pin)};

    // Limit the scope for the GPIO instance.
    {
        // Create a new GPIO output.
        // Expect the instance to be initialized correctly if the pin is valid.
        gpio::Atmega328p gpio{pin, gpio::Direction::Output};
        EXPECT_EQ(gpio.isInitialized(), isPinValid(pin));
        
        // Expect the GPIO to be set as output, i.e., the corresponding bit in DDRx should be set.
        EXPECT_TRUE(utils::read(regs.ddrx, physPin));

        // Set the output high, expect the corresponding bit in PORTx to be set.
        gpio.write(true);
        EXPECT_TRUE(utils::read(regs.portx, physPin));

        // Set the output low, expect the corresponding bit in PORTx to be cleared.
        gpio.write(false);
        EXPECT_FALSE(utils::read(regs.portx, physPin));

        // Toggle the output, expect the corresponding bit in PORTx to be set.
        gpio.toggle();
        simulateToggle(regs);
        EXPECT_TRUE(utils::read(regs.portx, physPin));

        // Toggle the output again, expect the corresponding bit in PORTx to be cleared.
        gpio.toggle();
        simulateToggle(regs);
        EXPECT_FALSE(utils::read(regs.portx, physPin));

        // Toggle the output once more, expect the corresponding bit in PORTx to be set.
        gpio.toggle();
        simulateToggle(regs);
        EXPECT_TRUE(utils::read(regs.portx, physPin));
    }

    // Expect DDRx and PORTx to be cleared after the instance has been deleted.
    EXPECT_FALSE(utils::read(regs.ddrx, physPin));
    EXPECT_FALSE(utils::read(regs.portx, physPin));
}

// -----------------------------------------------------------------------------
void runInputTest(const std::uint8_t pin, GpioRegs& regs) noexcept
{
    // Get the physical pin on the given port.
    const std::uint8_t physPin{getPhysicalPin(pin)};

    // Limit the scope for the GPIO instance.
    {
        // Create a new GPIO input with internal pull-up resistor enabled.
        // Expect the instance to be initialized correctly if the pin is valid.
        gpio::Atmega328p gpio{pin, gpio::Direction::InputPullup};
        EXPECT_EQ(gpio.isInitialized(), isPinValid(pin));
        
        // Expect the GPIO to be set as input, i.e., the corresponding bit in DDRx should be cleared.
        EXPECT_FALSE(utils::read(regs.ddrx, physPin));

        // Expect the internal pull-up resistor to be enabled, i.e., the corresponding bit in PORTx
        // should be set.
        EXPECT_TRUE(utils::read(regs.portx, physPin));

        // Set the input high, expect the corresponding bit in PINx to be set.
        utils::set(regs.pinx, physPin);
        EXPECT_TRUE(gpio.read());

        // Set the input low, expect the corresponding bit in PINx to be cleared.
        utils::clear(regs.pinx, physPin);
        EXPECT_FALSE(gpio.read());
    }

    // Expect DDRx and PORTx to be cleared after the instance has been deleted.
    EXPECT_FALSE(utils::read(regs.ddrx, physPin));
    EXPECT_FALSE(utils::read(regs.portx, physPin));
}

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
    // Systematically test I/O port D.
    for (std::uint8_t pin{}; pin < PinOffset::B; ++pin)
    {
        GpioRegs regs{DDRD, PORTD, PIND};
        runOutputTest(pin, regs);
    }

    // Systematically test I/O port B.
    for (std::uint8_t pin{PinOffset::B}; pin < PinOffset::C; ++pin)
    {
        GpioRegs regs{DDRB, PORTB, PINB};
        runOutputTest(pin, regs);
    }

    // Systematically test I/O port C.
    for (std::uint8_t pin{PinOffset::C}; pin < PinCount; ++pin)
    {
        GpioRegs regs{DDRC, PORTC, PINC};
        runOutputTest(pin, regs);
    }
}

/**
 * @brief GPIO output test.
 * 
 *        Verify that GPIO outputs can be used for reading and writing.
 */
TEST(Gpio_Atmega328p, Input)
{
    // Systematically test I/O port D.
    for (std::uint8_t pin{}; pin < PinOffset::B; ++pin)
    {
        GpioRegs regs{DDRD, PORTD, PIND};
        runInputTest(pin, regs);
    }

    // Systematically test I/O port B.
    for (std::uint8_t pin{PinOffset::B}; pin < PinOffset::C; ++pin)
    {
        GpioRegs regs{DDRB, PORTB, PINB};
        runInputTest(pin, regs);
    }

    // Systematically test I/O port C.
    for (std::uint8_t pin{PinOffset::C}; pin < PinCount; ++pin)
    {
        GpioRegs regs{DDRC, PORTC, PINC};
        runInputTest(pin, regs);
    }
}
} // namespace
} // namespace driver

#endif /** IFDEF TESTSUITE */
