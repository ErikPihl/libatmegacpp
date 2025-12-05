/**
 * @brief Temperature sensor stub.
 */
#pragma once

#include <stdint.h>

#include "driver/tempsensor/interface.h"

namespace driver
{
namespace tempsensor
{
/**
 * @brief Temperature sensor interface.
 */
struct Stub : public Interface
{
    /**
     * @brief Constructor.
     */
    Stub() noexcept
        : myTemp{}
    {}

    /**
     * @brief Check if the temperature sensor is initialized.
     * 
     * @return True if the temperature sensor is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Read the temperature sensor.
     *
     * @return The temperature in degrees Celsius.
     */
    int16_t read() const noexcept override { return myTemp; }

    /**
     * @brief Set temperature.
     * 
     * @param[in] temp Temperature.
     */
    void setTemp(const int16_t temp) noexcept { myTemp = temp; }

private:
    /** Temperature sensor value. */
    int16_t myTemp;
};
} // namespace tempsensor
} // namespace driver
