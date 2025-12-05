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
 * @brief GPIO stub.
 */
struct Stub : public Interface
{
    /**
     * @brief Create a new GPIO stub instance.
     */
    Stub() noexcept
        : myVal{false}
    {}

    /**
     * @brief Check whether the GPIO is initialized.
     * 
     * @return True if the GPIO is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

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
    void enableInterrupt(const bool enable) noexcept override { (void) (enable); }

    /**
     * @brief Enable pin change interrupt for I/O port associated with the GPIO.
     * 
     * @param[in] enable True to enable pin change interrupt for the I/O port, false otherwise.
     */
    void enableInterruptOnPort(const bool enable) noexcept override { (void) (enable); }

private:
    /** Value of the GPIO pin (true = high, false = low). */
    bool myVal;
};
} // namespace gpio
} // namespace driver
