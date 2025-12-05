/**
 * @brief Test cases for the logic implementation.
 */
#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "logic/logic.h"
#include "test/driver/mock.h"

using namespace ::testing;

namespace logic
{
namespace
{
/**
 * @brief Structure of mock instances.
 */
struct Mock
{
    driver::gpio::Mock led;
    driver::gpio::Mock toggleButton;
    driver::gpio::Mock tempButton;
    driver::timer::Mock debounceTimer;
    driver::timer::Mock toggleTimer;
    driver::timer::Mock tempTimer;
    driver::serial::Mock serial;
    driver::watchdog::Mock watchdog;
    driver::eeprom::Mock eeprom;
    driver::tempsensor::Mock tempSensor;

    /** 
     * @brief Create a new mock instance.
     * 
     * @param[in] silenceTrivialCalls True to silence trivial calls (default = true).
     */
    Mock(const bool silenceTrivialCalls = true);
    
    /**
     * @brief Create a new logic instance.
     * 
     * @return Pointer to the new logic instance.
     */
    std::unique_ptr<logic::Interface> createLogic();

    /**
     * @brief Silence initialization calls.
     */
    void silenceInitCalls();

    /**
     * @brief Silence cleanup calls.
     */
    void silenceCleanupCalls();

    /**
     * @brief Silence status calls.
     */
    void silenceStatusCalls();

    /** 
     * @brief Set LED enablement bit in EEPROM. 
     * 
     * @param[in] enable True to set the enablement bit, false otherwise.
     */
    void setLedStatusInEeprom(const bool enable);

};

// -----------------------------------------------------------------------------
Mock::Mock(const bool silenceTrivialCalls)
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
{
    if (silenceTrivialCalls)
    {
        silenceInitCalls();
        silenceCleanupCalls();
        silenceStatusCalls();
    }
}

// -----------------------------------------------------------------------------
std::unique_ptr<logic::Interface> Mock::createLogic()
{
    // Return new logic instance.
    return std::make_unique<logic::Logic>(
        led, toggleButton, tempButton, debounceTimer, toggleTimer, 
        tempTimer, serial, watchdog, eeprom, tempSensor);
}

// -----------------------------------------------------------------------------
void Mock::silenceInitCalls()
{
    EXPECT_CALL(toggleButton, enableInterrupt(true)).WillOnce(Return());
    EXPECT_CALL(tempButton, enableInterrupt(true)).WillOnce(Return());
    EXPECT_CALL(tempTimer, start()).WillOnce(Return());
    EXPECT_CALL(serial, print(_)).WillRepeatedly(Return());
    EXPECT_CALL(serial, setEnabled(true)).WillOnce(Return());
    EXPECT_CALL(watchdog, setEnabled(true)).WillOnce(Return());
    EXPECT_CALL(eeprom, setEnabled(true)).WillOnce(Return());
    EXPECT_CALL(watchdog, reset()).WillRepeatedly(Return());
}

// -----------------------------------------------------------------------------
void Mock::silenceCleanupCalls()
{
    EXPECT_CALL(led, write(false)).WillOnce(Return());
    EXPECT_CALL(toggleButton, enableInterrupt(false)).WillOnce(Return());
    EXPECT_CALL(tempButton, enableInterrupt(false)).WillOnce(Return());
    EXPECT_CALL(debounceTimer, stop()).WillRepeatedly(Return());
    EXPECT_CALL(toggleTimer, stop()).WillRepeatedly(Return());
    EXPECT_CALL(tempTimer, stop()).WillRepeatedly(Return());
    EXPECT_CALL(serial, setEnabled(false)).WillOnce(Return());
    EXPECT_CALL(watchdog, setEnabled(false)).WillOnce(Return());
    EXPECT_CALL(eeprom, setEnabled(false)).WillOnce(Return());
}

// -----------------------------------------------------------------------------
void Mock::silenceStatusCalls()
{
    ON_CALL(debounceTimer, isEnabled()).WillByDefault(Return(true));
    ON_CALL(toggleTimer, isEnabled()).WillByDefault(Return(true));
    ON_CALL(tempTimer, isEnabled()).WillByDefault(Return(true));
    ON_CALL(serial, isEnabled()).WillByDefault(Return(true));
    ON_CALL(watchdog, isEnabled()).WillByDefault(Return(true));
    ON_CALL(eeprom, isEnabled()).WillByDefault(Return(true));

    EXPECT_CALL(debounceTimer, isEnabled()).WillRepeatedly(Return(true));
    EXPECT_CALL(toggleTimer, isEnabled()).WillRepeatedly(Return(true));
    EXPECT_CALL(tempTimer, isEnabled()).WillRepeatedly(Return(true));
    EXPECT_CALL(serial, isEnabled()).WillRepeatedly(Return(true));
    EXPECT_CALL(watchdog, isEnabled()).WillRepeatedly(Return(true));
    EXPECT_CALL(eeprom, isEnabled()).WillRepeatedly(Return(true));
}

// -----------------------------------------------------------------------------
void Mock::setLedStatusInEeprom(const bool enabled)
{
    const std::uint8_t bitVal{static_cast<std::uint8_t>(enabled ? 1U : 0U)};
    ON_CALL(eeprom, isAddressValid(_, _)).WillByDefault(Return(true));
    ON_CALL(eeprom, readByte(_)).WillByDefault(Return(bitVal));
    EXPECT_CALL(eeprom, isAddressValid(_, _)).WillRepeatedly(Return(true));
    EXPECT_CALL(eeprom, readByte(_)).WillRepeatedly(Return(bitVal));
}

// -----------------------------------------------------------------------------
void runThread(logic::Interface& logic, bool& stop) { logic.run(stop); }

// -----------------------------------------------------------------------------
void stopThread(const std::size_t waitTime_ms, bool& stop) 
{
    stop = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime_ms));
    stop = true;
}

/**
 * @brief Logic run test.
 *
 *        Verify that the logic system can be constructed and run with all mock dependencies.
 */
TEST(Logic, Run)
{
    Mock mock{};
    std::unique_ptr<logic::Interface> logic{mock.createLogic()};
    mock.setLedStatusInEeprom(true);

    // Run system for five seconds.
    bool stop{false};
    constexpr std::size_t testDuration_ms{5000U};
    std::thread t1{runThread, std::ref(*logic), std::ref(stop)};
    std::thread t2{stopThread, testDuration_ms, std::ref(stop)};
    t1.join();
    t2.join();
    
    // Case 1 - Simulate toggle button press, expect the toggle timer to be toggled.
    {
        ON_CALL(mock.toggleButton, read()).WillByDefault(Return(true));
        ON_CALL(mock.toggleTimer, isEnabled()).WillByDefault(Return(true));
        EXPECT_CALL(mock.toggleButton, enableInterruptOnPort(false)).WillOnce(Return());
        EXPECT_CALL(mock.debounceTimer, start()).WillOnce(Return());
        EXPECT_CALL(mock.toggleTimer, toggle()).WillOnce(Return());
        logic->handleButtonEvent();
    }
}
} // namespace
} // namespace driver
