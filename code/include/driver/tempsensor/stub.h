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
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     */
    Stub() noexcept
        : myTemp{}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

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

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Temperature sensor value. */
    int16_t myTemp;
};
} // namespace tempsensor
} // namespace driver
