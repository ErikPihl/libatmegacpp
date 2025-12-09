/**
 * @brief GPIO stub.
 */
#pragma once

#include <stdint.h>

#include "driver/gpio/interface.h"

namespace driver 
{
namespace gpio
{
/**
 * @brief GPIO driver stub.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Create a new GPIO stub instance.
     */
    Stub() noexcept
        : myVal{false}
        , myInterruptEnabled{false}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check whether the GPIO is initialized.
     * 
     * @return True if the GPIO is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Get the data direction of the GPIO.
     * 
     * @return The data direction of the GPIO.
     */
    Direction direction() const noexcept override { return Direction::Input; }

    /**
     * @brief Read input of the GPIO.
     * 
     * @return True if the input is high, false otherwise.
     */
    bool read() const noexcept override { return myVal; }

    /**
     * @brief Write output to the GPIO.
     * 
     * @param[in] output The output value to write (true = high, false = low).
     */
    void write(const bool output) noexcept override { myVal = output; }

    /**
     * @brief Toggle the output of the GPIO.
     *        
     * @note This operation is only supported for pins set to output.
     */
    void toggle() noexcept override { myVal = !myVal; }

    /**
     * @brief Enable/disable pin change interrupt for the GPIO.
     * 
     * @param[in] enable True to enable pin change interrupt for the GPIO, false otherwise.
     */
    void enableInterrupt(const bool enable) noexcept override { myInterruptEnabled = enable; }

    /**
     * @brief Enable pin change interrupt for I/O port associated with the GPIO.
     * 
     * @param[in] enable True to enable pin change interrupt for the I/O port, false otherwise.
     */
    void enableInterruptOnPort(const bool enable) noexcept override 
    { 
        myInterruptEnabled = enable; 
    }

    /**
     * @brief Check whether interrupt is enabled for the GPIO.
     * 
     * @return True if interrupt is enabled for the GPIO, false otherwise.
     */
    bool isInterruptEnabled() const noexcept { return myInterruptEnabled; }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Value of the GPIO pin (true = high, false = low). */
    bool myVal;

    /** Indicate whether interrupts are enabled for the GPIO. */
    bool myInterruptEnabled;
};
} // namespace gpio
} // namespace driver
