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
 * @brief Toggle button test.
 *
 *        Verify that the system behaves as excepted when the toggle button works.
 */
TEST(Logic, ToggleButtonPress)
{
    Mock mock{};
    mock.logic->writeLedStateToEeprom(false);
    logic::Interface& logic{*mock.logic};

    // Run system for 10 ms.
    bool stop{false};
    constexpr std::size_t testDuration_ms{10U};
    std::thread t1{runLogicThread, std::ref(logic), std::ref(stop)};
    std::thread t2{stopLogicThread, testDuration_ms, std::ref(stop)};
    t1.join();
    t2.join();
    
    // Expect the toggle timer and the LED to be disabled at the start.
    EXPECT_FALSE(mock.led.read());
    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.logic->readLedStateFromEeprom());

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
        mock.toggleTimer.setTimedOut(false);
        EXPECT_TRUE(mock.led.read());
    }

    // Case 3 - Simulate that the toggle timer elapses again, expect the LED to be disabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        mock.toggleTimer.setTimedOut(false);
        EXPECT_FALSE(mock.led.read());
    }

    // Case 4 - Simulate that the toggle timer elapses once more, expect the LED to be enabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        mock.toggleTimer.setTimedOut(false);
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

/**
 * @brief Temperature button test.
 *
 *        Verify that the system behaves as excepted when the temperature button works.
 */
TEST(Logic, TempButtonPress)
{
    Mock mock{};
    logic::Interface& logic{*mock.logic};

    // Run system for 10 ms.
    bool stop{false};
    constexpr std::size_t testDuration_ms{10U};
    std::thread t1{runLogicThread, std::ref(logic), std::ref(stop)};
    std::thread t2{stopLogicThread, testDuration_ms, std::ref(stop)};
    t1.join();
    t2.join();
    
    // Expect the temperature timer to be disabled at the start.
    EXPECT_TRUE(mock.tempTimer.isEnabled());

    // Set the temperature to 25 degrees Celsius.
    constexpr int16_t temp{25};
    mock.tempSensor.setTemp(temp);

    // Case 1 - Press the temperature button, simulate button event.
    // Expect the temperature to be printed once.
    {
        const uint16_t tempPrintoutsBefore{mock.logic->tempPrintoutCount()};
        mock.tempButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_TRUE(mock.debounceTimer.isEnabled());

        const uint16_t tempPrintoutsAfter{mock.logic->tempPrintoutCount()};
        EXPECT_EQ(tempPrintoutsAfter, tempPrintoutsBefore + 1U);
    }

    // Case 1 - Simulate temperature timer timeout.
    // Expect the temperature to be printed once more.
    {
        const uint16_t tempPrintoutsBefore{mock.logic->tempPrintoutCount()};
        mock.tempTimer.setTimedOut(true);
        logic.handleTempTimerTimeout();
        mock.tempTimer.setTimedOut(false);
        
        const uint16_t tempPrintoutsAfter{mock.logic->tempPrintoutCount()};
        EXPECT_EQ(tempPrintoutsAfter, tempPrintoutsBefore + 1U);
    }
}
} // namespace
} // namespace driver
