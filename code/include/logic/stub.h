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
     * @brief Get the EEPROM address used to store the LED state.
     *
     * @return The EEPROM address for the LED state.
     */
    uint16_t ledMemAddr() const noexcept { return LedState::Address; }

    /**
     * @brief Get the EEPROM data representing the LED enabled state.
     *
     * @return The EEPROM value for the enabled LED state.
     */
    uint8_t ledMemData() const noexcept { return LedState::Enabled; }
};
} // namespace logic