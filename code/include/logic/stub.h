/**
 * @brief Logic stub for exposing protected members and constants for unit tests.
 */
#pragma once

#include "logic/logic.h"

namespace logic
{
/**
 * @brief Logic stub for exposing protected members and constants for unit tests.
 */
struct Stub : public Logic
{
    /** Inherit constructors from Logic. */
    using Logic::Logic;

    /**
     * @brief Write LED state to EEPROM.
     * 
     * @param[in] enable True to mark the LED as enabled, false otherwise.
     */
    void writeLedStateToEeprom(const bool enable) noexcept override
    {
        myEeprom.write(LedStateAddr, static_cast<uint8_t>(enable));
    }

    /**
     * @brief Read LED state from EEPROM.
     * 
     * @return True if the LED is enabled, false otherwise.
     */
    bool readLedStateFromEeprom() const noexcept override 
    { 
        uint8_t state{};
        return myEeprom.read(LedStateAddr, state) ? static_cast<bool>(state) : false;
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
    /** LED address in EEPROM. */
    static constexpr uint16_t LedStateAddr{0U};

    /** The number of temperature printouts. */
    uint16_t myTempPrintouts{};
};
} // namespace logic