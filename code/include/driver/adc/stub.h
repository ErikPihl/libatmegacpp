/**
 * @brief ADC driver stub.
 */
#pragma once

#include <math.h>
#include <stdint.h>

#include "driver/adc/interface.h"

namespace driver 
{
namespace adc
{
/**
 * @brief ADC driver stub.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Create a new ADC stub.
     * 
     * @param[in] resolution ADC resolution (default = 10 bits).
     * @param[in] supplyVoltage Supply voltage (default = 5 V).
     */
    explicit Stub(const uint8_t resolution = 10U, const double supplyVoltage = 5.0) noexcept
        : mySupplyVoltage{supplyVoltage}
        , myMaxVal{static_cast<uint16_t>(pow(2U, resolution) - 1U)}
        , myAdcVal{}
        , myResolution{resolution}
        , myEnabled{true}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Get the resolution of the ADC.
     * 
     * @return The resolution of the ADC in bits.
     */
    uint8_t resolution() const noexcept override { return myResolution; }

    /**
     * @brief Get the maximal input value of the ADC.
     * 
     * @return The maximum digital value of the ADC.
     */
    uint16_t maxValue() const noexcept override { return myMaxVal; }

    /**
     * @brief Get the supply voltage of the ADC.
     * 
     * @return The supply voltage of the ADC in Volts.
     */
    double supplyVoltage() const noexcept override { return mySupplyVoltage; }

    /**
     * @brief Read input from given channel.
     * 
     * @param[in] channel Channel from which to read.
     * 
     * @return The digital value corresponding to the input of the specified channel.
     */
    uint16_t read(const uint8_t channel) const noexcept override 
    { 
        (void) (channel);
        return myEnabled ? myAdcVal : 0U; 
    }

    /**
     * @brief Calculate duty cycle out of input from given channel.
     * 
     * @param[in] channel Channel from which to read.
     * 
     * @return The duty cycle as a floating point value between 0.0 - 1.0.
     */
    double dutyCycle(const uint8_t channel) const noexcept override 
    { 
        return read(channel) / myMaxVal;
    }

    /**
     * @brief Read input voltage from given channel.
     * 
     * @param[in] channel Channel from which to read.
     * 
     * @return The input voltage in Volts.
     */
    double inputVoltage(const uint8_t channel) const noexcept override
    {
        return dutyCycle(channel) * mySupplyVoltage;
    }

    /**
     * @brief Check whether the ADC is initialized.
     * 
     * @return True if the ADC is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Indicate whether the ADC is enabled.
     * 
     * @return True if the ADC is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Set enablement of ADC.
     * 
     * @param[in] enable Indicate whether to enable the ADC.
     */
    void setEnabled(const bool enable) noexcept override { myEnabled = enable; }

    /**
     * @brief Check whether the given channel is valid.
     * 
     * @param[in] channel The channel to check.
     * 
     * @return True if the channel is valid, false otherwise.
     */
    bool isChannelValid(const uint8_t channel) const noexcept override { return true; }

    Stub()                       = delete; // No default constructor.
    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Supply voltage. */
    const double mySupplyVoltage;

    /** ADC max value. */
    const uint16_t myMaxVal;

    /** ADC value (virtual input). */
    uint16_t myAdcVal;

    /** ADC resolution. */
    const uint8_t resolution;

    /** Indicate whether the ADC is enabled. */
    bool myEnabled;
};
} // namespace adc
} // namespace driver
