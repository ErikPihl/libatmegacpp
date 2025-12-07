/**
 * @brief Timer stub.
 */
#pragma once

#include <stdint.h>

#include "driver/timer/interface.h"

namespace driver 
{
namespace timer
{
/**
 * @brief Timer stub.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     *
     * @param[in] timeout_ms The timeout in milliseconds (default = 100 ms).
     * @param[in] startTimer Start the timer immediately (default = false).
     */
    Stub(const uint32_t timeout_ms = 100U, const bool startTimer = false) noexcept
        : myTimeout_ms{timeout_ms}
        , myEnabled{startTimer}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the timer is initialized.
     * 
     *        An uninitialized timer indicates that no timer circuit was available when the 
     *        timer was created.
     * 
     * @return True if the timer is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Check whether the timer is enabled.
     *
     * @return True if the timer is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Check whether the timer has timed out.
     * 
     *        The timer will restart automatically on timeout.
     *
     * @return True if the timer has timed out, false otherwise.
     */
    bool hasTimedOut() const noexcept override { return myEnabled && myTimedOut; }

    /**
     * @brief Get the timeout of the timer.
     * 
     * @return The timeout in milliseconds.
     */
    uint32_t timeout_ms() const noexcept override { return myTimeout_ms; }

    /**
     * @brief Set timeout of the timer.
     * 
     * @param[in] timeout_ms The new timeout in milliseconds.
     */
    void setTimeout_ms(const uint32_t timeout_ms) noexcept override { myTimeout_ms = timeout_ms; }

    /**
     * @brief Start the timer.
     */
    void start() noexcept override 
    { 
        myEnabled  = true; 
        myTimedOut = false;
    }

    /**
     * @brief Stop the timer.
     */
    void stop() noexcept override 
    { 
        myEnabled  = false;
        myTimedOut = false; 
    }

    /**
     * @brief Toggle the timer.
     */
    void toggle() noexcept override 
    { 
        myEnabled  = !myEnabled; 
        myTimedOut = false;
    }

    /**
     * @brief Restart the timer.
     */
    void restart() noexcept override { start(); }

    /**
     * @brief Set whether the timer has timed out.
     * 
     * @param[in] timedOut True to indicate that the timer has timed out, false otherwise.
     */
    void setTimedOut(const bool timedOut) { myTimedOut = timedOut; }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Timeout in ms. */
    uint32_t myTimeout_ms;

    /** Indicate whether the timer is enabled. */
    bool myEnabled;

    /** Indicate whether the timer has timed out. */
    bool myTimedOut;
};
} // namespace timer
} // namespace driver
