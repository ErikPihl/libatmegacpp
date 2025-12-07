/**
 * @brief Component tests for the logic implementation.
 */
#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
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
 * 
 * @tparam EepromSize EEPROM size in bytes (default = 1024).
 */
template <std::uint16_t EepromSize = 1024U>
struct Mock final
{
    // Generate a compiler error if the EEPROM size is set to 0.
    static_assert(0U < EepromSize, "EEPROM size must be greater than 0!");

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
    std::unique_ptr<logic::Stub> logicImpl;

    /** 
     * @brief Constructor.
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
        , logicImpl{nullptr}
    {}

    /**
     * @brief Destructor.
     */
    ~Mock() noexcept = default;

    /**
     * @brief Create logic implementation.
     * 
     * @return Reference to the logic implementation.
     */
    logic::Interface& createLogic()
    {
        logicImpl = std::make_unique<logic::Stub>(
            led, toggleButton, tempButton, debounceTimer, toggleTimer, 
            tempTimer, serial, watchdog, eeprom, tempSensor);
        return *logicImpl;
    }

    /**
     * @brief Run system.
     * 
     * @param[in] testDuration_ms Duration to run the system in ms (default = 10 ms).
     */
    void runSystem(const std::size_t testDuration_ms = 10U)
    {
        // Throw an exception if the logic is not initialized.
        if (nullptr == logicImpl) { throw std::invalid_argument("Logic not initialized!\n"); }
        
        // Run the system, stop after given duration has passed.
        bool stop{false};
        std::thread t1{runLogicThread, std::ref(*logicImpl), std::ref(stop)};
        std::thread t2{stopLogicThread, testDuration_ms, std::ref(stop)};
        t1.join();
        t2.join();
    }

    Mock(const Mock&)            = delete; // No copy constructor.
    Mock(Mock&&)                 = delete; // No move constructor.
    Mock& operator=(const Mock&) = delete; // No copy assignment.
    Mock& operator=(Mock&&)      = delete; // No move assignment.

private:
    // -----------------------------------------------------------------------------
    static void runLogicThread(logic::Interface& logic, bool& stop) noexcept
    {
        // Run the logic loop as long as the stop flag is low.
        logic.run(stop);
    }

    // -----------------------------------------------------------------------------
    static void stopLogicThread(const std::size_t timeout_ms, bool& stop) noexcept
    {
        // Stop the logic loop on timeout.
        stop = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        stop = true;
    }
};

/**
 * @brief Debounce handling test.
 *
 *        Verify that the debounce handling behaves as expected.
 */
TEST(Logic, DebounceHandling)
{
    // Create logic implementation and run the system.
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();

    // Case 1 - Press the toggle button, simulate button event.
    // Expect button interrupts to be disabled and the debounce timer to be enabled.
    // Expect the toggle timer to be enabled due to the toggle button being pressed.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        EXPECT_FALSE(mock.tempButton.isInterruptEnabled());
        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    // Case 2 - Call the button event handler before the debounce timer has timed out.
    // Expect this call to be ignored, i.e, expect the toggle timer to still be enabled.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }

    // Case 3 - Simulate debounce timer timeout, expect toggle button interrupt to be re-enabled.
    // Expect the debounce timer to be disabled.
    {
        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();
        EXPECT_TRUE(mock.toggleButton.isInterruptEnabled());
        EXPECT_FALSE(mock.debounceTimer.isEnabled());
    }

    // Case 4 - Press the toggle button again after the debounce timer has timed out, 
    // simulate button event.
    // Expect button interrupts to be disabled and the debounce timer to be enabled.
    // Expect the toggle timer to be disabled due to the toggle button being pressed.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);

        EXPECT_FALSE(mock.tempButton.isInterruptEnabled());
        EXPECT_TRUE(mock.debounceTimer.isEnabled());
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }
}

/**
 * @brief Toggle handling test.
 *
 *        Verify that the toggle handling behaves as expected.
 */
TEST(Logic, ToggleHandling)
{
    // Create logic implementation and run the system.
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();
    
    // Expect the toggle timer and the LED to be disabled at the start.
    EXPECT_FALSE(mock.led.read());
    EXPECT_FALSE(mock.toggleTimer.isEnabled());
    EXPECT_FALSE(mock.logicImpl->readToggleStateFromEeprom());

    // Case 1 - Press the temperature button, simulate button event.
    // Expect the toggle timer to not be enabled, since the wrong button was pressed.
    {
        mock.tempButton.write(true);
        logic.handleButtonEvent();
        mock.tempButton.write(false);
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
        mock.debounceTimer.stop();
    }

    // Case 2 - Press the toggle button, simulate button event.
    // Expect the toggle timer to be enabled.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
        mock.debounceTimer.stop();
    }

    // Case 3 - Simulate toggle timer timeout, expect the LED to be enabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        mock.toggleTimer.setTimedOut(false);
        EXPECT_TRUE(mock.led.read());
    }

    // Case 4 - Simulate that the toggle timer elapses again, expect the LED to be disabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        mock.toggleTimer.setTimedOut(false);
        EXPECT_FALSE(mock.led.read());
    }

    // Case 5 - Simulate that the toggle timer elapses once more, expect the LED to be enabled.
    {
        mock.toggleTimer.setTimedOut(true);
        logic.handleToggleTimerTimeout();
        mock.toggleTimer.setTimedOut(false);
        EXPECT_TRUE(mock.led.read());
    }

    // Case 6 - Press the toggle button once more, simulate button event.
    // Expect the toggle timer and LED to be disabled.
    {
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
        EXPECT_FALSE(mock.led.read());
        mock.debounceTimer.stop();
    }

    // Case 7 - Simulate temperature timer timeout, expect the LED to be unaffected.
    {
        const bool ledStateBefore{mock.led.read()};
        mock.tempTimer.setTimedOut(true);
        logic.handleTempTimerTimeout();
        mock.tempTimer.setTimedOut(false);
        const bool ledStateAfter{mock.led.read()};
        EXPECT_EQ(ledStateBefore, ledStateAfter);
    }

    // Case 8 - Simulate debounce timer timeout, expect the LED to be unaffected.
    {
        const bool ledStateBefore{mock.led.read()};
        mock.debounceTimer.setTimedOut(true);
        logic.handleDebounceTimerTimeout();
        mock.debounceTimer.setTimedOut(false);
        const bool ledStateAfter{mock.led.read()};
        EXPECT_EQ(ledStateBefore, ledStateAfter);
    }
}

/**
 * @brief Tempetaure handling test.
 *
 *        Verify that the temperature handling behaves as expected.
 */
TEST(Logic, TempHandling)
{
    // Create logic implementation and run the system.
    Mock mock{};
    logic::Interface& logic{mock.createLogic()};
    mock.runSystem();
    
    // Expect the temperature timer to be disabled at the start.
    EXPECT_TRUE(mock.tempTimer.isEnabled());

    // Set the temperature to 25 degrees Celsius.
    constexpr int16_t temp{25};
    mock.tempSensor.setTemp(temp);

    // Case 1 - Press the toggle button, simulate button event.
    // Expect the temperature to not be printed, since the wrong button was pressed.
    {
        const uint16_t tempPrintoutsBefore{mock.logicImpl->tempPrintoutCount()};
        mock.toggleButton.write(true);
        logic.handleButtonEvent();
        mock.toggleButton.write(false);
        mock.debounceTimer.stop();
        const uint16_t tempPrintoutsAfter{mock.logicImpl->tempPrintoutCount()};
        EXPECT_EQ(tempPrintoutsAfter, tempPrintoutsBefore);
    }

    // Case 2 - Press the temperature button, simulate button event.
    // Expect the temperature to be printed once.
    {
        const uint16_t tempPrintoutsBefore{mock.logicImpl->tempPrintoutCount()};
        mock.tempButton.write(true);
        logic.handleButtonEvent();
        mock.tempButton.write(false);
        const uint16_t tempPrintoutsAfter{mock.logicImpl->tempPrintoutCount()};
        EXPECT_EQ(tempPrintoutsAfter, tempPrintoutsBefore + 1U);
        mock.debounceTimer.stop();
    }

    // Case 3 - Simulate temperature timer timeout.
    // Expect the temperature to be printed once more.
    {
        const uint16_t tempPrintoutsBefore{mock.logicImpl->tempPrintoutCount()};
        mock.tempTimer.setTimedOut(true);
        logic.handleTempTimerTimeout();
        mock.tempTimer.setTimedOut(false);
        
        const uint16_t tempPrintoutsAfter{mock.logicImpl->tempPrintoutCount()};
        EXPECT_EQ(tempPrintoutsAfter, tempPrintoutsBefore + 1U);
    }
}

/**
 * @brief EEPROM handling test.
 *
 *        Verify that the EEPROM handling behaves as expected.
 */
TEST(Logic, Eeprom)
{
    // Case 1 - Verify that the toggle timer is disabled at startup if its EEPROM bit is not set.
    // This simulates the timer being disabled before the last poweroff.
    {    
        // Create logic implementation and run the system.
        Mock mock{};
        mock.createLogic();

        // Verify that the toggle timer is disabled after initialization.
        EXPECT_FALSE(mock.toggleTimer.isEnabled());
    }

    // Case 2 - Verify that the toggle timer is enabled at startup if its EEPROM bit is set.
    // This simulates the timer being enabled before the last poweroff.
    {    
        // Mark the toggle timer to have been enabled before poweroff by setting the
        // associated bit in EEPROM before creating the logic implementation.
        Mock mock{};
        mock.eeprom.writeByte(logic::Stub::toggleStateAddr(), true);
        
        // Create logic implementation and run the system.
        mock.createLogic();

        // Verify that the toggle timer was enabled during initialization.
        EXPECT_TRUE(mock.toggleTimer.isEnabled());
    }
}
} // namespace
} // namespace driver
