/**
 * @brief Component tests for the logic implementation.
 */
#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>

#include <gtest/gtest.h>

#include "driver/eeprom/stub.h"
#include "driver/gpio/stub.h"
#include "driver/serial/stub.h"
#include "driver/tempsensor/stub.h"
#include "driver/timer/stub.h"
#include "driver/watchdog/stub.h"
#include "logic/stub.h"

namespace logic
{
namespace
{
/**
 * @brief Structure of mock instances.
 */
struct Mock
{
    /** EEPROM size in bytes. */
    static constexpr uint16_t EepromSize{1024U};

    /** LED stub. */
    driver::gpio::Stub led;

    /** Toggle button stub. */
    driver::gpio::Stub toggleButton;

    /** Temperature button stub. */
    driver::gpio::Stub tempButton;

    /** Debounce timer stub. */
    driver::timer::Stub debounceTimer;

    /** Toggle timer stub. */
    driver::timer::Stub toggleTimer;

    /** Temperature timer stub. */
    driver::timer::Stub tempTimer;

    /** Serial driver stub. */
    driver::serial::Stub serial;

    /** Watchdog timer stub. */
    driver::watchdog::Stub watchdog;

    /** EEPROM stream stub. */
    driver::eeprom::Stub<EepromSize> eeprom;

    /** Temperature sensor stub. */
    driver::tempsensor::Stub tempSensor;

    /** Logic implementation stub. */
    std::unique_ptr<logic::Stub> logic;

    /** 
     * @brief Create a new mock instance.
     */
    Mock() noexcept
        : led{}
        , toggleButton{}
        , tempButton{}
        , debounceTimer{}
        , toggleTimer{}
        , tempTimer{}
        , serial{}
        , watchdog{}
        , eeprom{}
        , tempSensor{}
        , logic{createLogic()}
    {}

    /**
     * @brief Write LED state to memory.
     * 
     * @param[in] enable True to mark the LED as enabled, false otherwise.
     */
    void writeLedMemState(const bool enable) noexcept
    {
        const std::uint8_t byte{enable ? logic->ledMemData() : static_cast<std::uint8_t>(0U)};
        eeprom.writeByte(logic->ledMemAddr(), byte);
    }

private:
    // -----------------------------------------------------------------------------
    std::unique_ptr<logic::Stub> createLogic()
    {
        return std::make_unique<logic::Stub>(
            led, toggleButton, tempButton, debounceTimer, toggleTimer, 
            tempTimer, serial, watchdog, eeprom, tempSensor);
    }
};

// -----------------------------------------------------------------------------
void runLogicThread(logic::Interface& logic, bool& stop) 
{
    // Run the logic loop as long as the stop flag is low.
    logic.run(stop);
}

// -----------------------------------------------------------------------------
void stopLogicThread(const std::size_t timeout_ms, bool& stop) 
{
    // Stop the logic loop on timeout.
    stop = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    stop = true;
}

/**
 * @brief Logic run test.
 *
 *        Verify that the the toggle button works as expected.
 */
TEST(Logic, ToggleButtonPress)
{
    Mock mock{};
    logic::Interface& logic{*mock.logic};
    mock.writeLedMemState(false);

    // Run system for 100 ms.
    bool stop{false};
    constexpr std::size_t testDuration_ms{10U};
    std::thread t1{runLogicThread, std::ref(logic), std::ref(stop)};
    std::thread t2{stopLogicThread, testDuration_ms, std::ref(stop)};
    t1.join();
    t2.join();
    
    // Ensure that the toggle timer and the LED is disabled at the start.
    mock.led.write(false);
    mock.toggleTimer.stop();

    // Case 1 - Press the toggle button, simulate button event.
    // Expect the toggle timer to be enabled.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    // Case 2 - Simulate that the toggle timer elapses, expect the LED to be enabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 3 - Simulate that the toggle timer elapses again, expect the LED to be disabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_FALSE(mock.led.read());
    }

    // Case 4 - Simulate that the toggle timer elapses once more, expect the LED to be enabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        EXPECT_TRUE(mock.led.read());
    }

    // Case 5 - Press the toggle button once more, simulate button event.
    // Expect the toggle timer to be disabled and the the LED is disabled at once.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
        EXPECT_FALSE(mock.led.read());
    }
}
} // namespace
} // namespace driver
