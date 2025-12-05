/**
 * @brief Watchdog timer driver implementation details for ATmega328P.
 */
#include "arch/avr/hw_platform.h"
#include "utils/utils.h"
#include "driver/watchdog/atmega328p.h"

namespace driver 
{
namespace watchdog
{
namespace 
{
/**
 * @brief Structure of ATmega328P watchdog timer parameters.
 */
struct WatchdogParam
{
    /** Default watchdog timeout (1024 ms). */
    static constexpr auto DefaultTimeout{Atmega328p::Timeout::Duration1024ms};
};

// -----------------------------------------------------------------------------
constexpr bool isTimeoutValid(const Atmega328p::Timeout timeout) noexcept
{
    return static_cast<uint16_t>(Atmega328p::Timeout::Invalid) > static_cast<uint16_t>(timeout);
}
    
// -----------------------------------------------------------------------------
uint8_t timeoutValue(const Atmega328p::Timeout timeout) noexcept
{
    switch (timeout)
    {
        case Atmega328p::Timeout::Duration16ms:
            return 0U;
        case Atmega328p::Timeout::Duration32ms:
            return (1U << WDP0);
        case Atmega328p::Timeout::Duration64ms:
            return (1U << WDP1);
        case Atmega328p::Timeout::Duration128ms:
            return (1U << WDP1) | (1U << WDP0);
        case Atmega328p::Timeout::Duration256ms:
            return (1U << WDP2);
        case Atmega328p::Timeout::Duration512ms:
            return (1U << WDP2) | (1U << WDP0);
        case Atmega328p::Timeout::Duration1024ms:
            return (1U << WDP2) | (1U << WDP1);
        case Atmega328p::Timeout::Duration2048ms:
            return (1U << WDP2) | (1U << WDP1) | (1U << WDP0);
        case Atmega328p::Timeout::Duration4096ms:
            return (1U << WDP3);
        case Atmega328p::Timeout::Duration8192ms:
            return (1U << WDP3) | (1U << WDP0);
        default:
            return 0xFFU;
    }
}
} // namespace

// -----------------------------------------------------------------------------
Interface& Atmega328p::getInstance() noexcept
{
    // Create and initialize the singleton watchdog timer instance (once only).
    static Atmega328p myInstance{};

    // Return a reference to the singleton watchdog instance, cast to the corresponding interface.
    return myInstance; 
}

// -----------------------------------------------------------------------------
bool Atmega328p::isInitialized() const noexcept { return true; }

// -----------------------------------------------------------------------------
bool Atmega328p::isEnabled() const noexcept { return myEnabled; }

// -----------------------------------------------------------------------------
void Atmega328p::setEnabled(const bool enable) noexcept
{
    // Reset the watchdog to prevent a timeout during the enablement update.
    reset();

    // Update the enablement status, disable interrupts during the write sequence.
    utils::globalInterruptDisable();
    utils::set(WDTCSR, WDCE, WDE);
    if (enable) { utils::set(WDTCSR, WDE); }
    else { utils::clear(WDTCSR, WDE); }

    // Re-enable interrupts once the write sequence is complete.
    utils::globalInterruptEnable();
}

// -----------------------------------------------------------------------------
uint16_t Atmega328p::timeout_ms() const noexcept { return static_cast<uint16_t>(myTimeout); }

// -----------------------------------------------------------------------------
void Atmega328p::reset() noexcept 
{ 
    // Disable interrupts during the reset process.
    utils::globalInterruptDisable();

    // Reset the watchdog and clear the corresponding reset flag.
    asm("WDR");
    utils::clear(MCUSR, WDRF);

    // Re-enable interrupts once the reset process is complete.
    utils::globalInterruptEnable();
}

// -----------------------------------------------------------------------------
bool Atmega328p::setTimeout(const Timeout timeout) noexcept
{
    // Return false if the timeout is invalid.
    if (!isTimeoutValid(timeout)) { return false; }

    // Calculate the timeout value before the timed write sequence.
    const auto value{timeoutValue(timeout)};

    // Update the watchdog timeout, disable interrupts during the write sequence.
    utils::globalInterruptDisable();
    WDTCSR |= (1UL << WDCE) | (1UL << WDE);
    WDTCSR = value;
    utils::globalInterruptEnable();

    // Re-enable interrupts once the write sequence is complete.
    utils::globalInterruptEnable();

    // Store the new timeout and return true to indicate success.
    myTimeout = timeout;
    return true;
} 

// -----------------------------------------------------------------------------
Atmega328p::Atmega328p() noexcept
    : myTimeout{}
    , myEnabled{false}
{
    // Set the default timeout.
    setTimeout(WatchdogParam::DefaultTimeout);
}
} // namespace watchdog
} // namespace driver
