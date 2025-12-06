/**
 * @brief Logic stub for exposing protected members and constants for unit tests.
 */
#pragma once

#include "logic/logic.h"

namespace logic
{
/**
 * @brief Logic stub for testing.
 */
class Stub final : public Logic
{
public:
    /** Inherit constructors from Logic. */
    using Logic::Logic;

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Get the toggle state address in EEPROM.
     * 
     * @return The toggle state address in EEPROM.
     */
    static uint16_t toggleStateAddr() noexcept { return ToggleStateAddr; }

    /**
     * @brief Write toggle state to EEPROM.
     * 
     * @param[in] enable Toggle state (true = enabled).
     */
    void writeToggleStateToEeprom(const bool enable) noexcept override
    {
        myEeprom.write(ToggleStateAddr, static_cast<uint8_t>(enable));
    }

    /**
     * @brief Read LED state from EEPROM.
     * 
     * @return The toggle state (true = enabled).
     */
    bool readToggleStateFromEeprom() const noexcept override 
    { 
        uint8_t state{};
        return myEeprom.read(ToggleStateAddr, state) ? static_cast<bool>(state) : false;
    }

    /**
     * @brief Print the temperature in the terminal.
     */
    void printTemperature() noexcept override
    {
        // Read and print the temperature.
        mySerial.printf("Simulated temperature: %d °C\n", myTempSensor.read());
        myTempPrintouts++;
    }

    /**
     * @brief Get the number of temperature printouts.
     * 
     * @return The number of temperature printouts.
     */
    uint16_t tempPrintoutCount() const noexcept { return myTempPrintouts; }

private:
    /** The number of temperature printouts. */
    uint16_t myTempPrintouts{};
};
} // namespace logic