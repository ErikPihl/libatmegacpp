/**
 * @brief Watchdog timer stub.
 */
#pragma once

#include <stdint.h>

#include "driver/watchdog/interface.h"

namespace driver 
{
namespace watchdog
{
/**
 * @brief Watchdog timer stub.
 */
struct Stub : public Interface
{
    /**
     * @brief Constructor.
     * 
     * @param[in] timeout_ms Watchdog timeout in ms (default = 1024 ms).
     */
    Stub(const uint16_t timeout_ms = 1024U) noexcept
        : myTimeout_ms{timeout_ms}
    {}

    /**
     * @brief Check whether the watchdog timer is initialized.
     * 
     * @return True if the watchdog timer is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Check whether the watchdog timer is enabled.
     * 
     * @return True if the watchdog timer is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Set enablement of the watchdog timer.
     * 
     * @param[in] enable True to enable the watchdog timer, false otherwise.
     */
    void setEnabled(const bool enable) noexcept override { myEnabled = enable; }

    /**
     * @brief Get the timeout of the watchdog timer.
     * 
     * @return The timeout of the watchdog timer in milliseconds.
     */
    uint16_t timeout_ms() const noexcept override { return myTimeout_ms; }

    /**
     * @brief Reset the watchdog timer.
     */
    void reset() noexcept override {}

private:
    /** Watchdog timeout in ms. */
    const uint16_t myTimeout_ms;

    /** Indicate whether the watchdog is enabled. */
    bool myEnabled;
};
} // namespace watchdog
} // namespace driver
