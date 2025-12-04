/**
 * @brief Generic logic implementation for an MCU with configurable hardware devices.
 */
#pragma once

#include "logic/interface.h"

namespace driver
{
namespace adc
{
/** ADC (A/D converter) interface. */
class Interface;
} // namespace adc

namespace eeprom
{
/** EEPROM (Electrically Erasable Programmable ROM) stream interface. */
class Interface;
} // namespace eeprom

namespace gpio
{
/** GPIO interface. */
class Interface;
} // namespace gpio

namespace serial
{
/** Serial transmission interface. */
class Interface;
} // namespace serial

namespace tempsensor
{
/** Temperature sensor interface. */
class Interface;
} // namespace tempsensor

namespace timer
{
/** Timer interface. */
class Interface;
} // namespace timer

namespace watchdog
{
/** Watchdog timer interface. */
class Interface;
} // namespace watchdog
} // namespace driver

namespace logic
{
/**
 * @brief Generic logic for an MCU with configurable hardware devices.
 * 
 *        The following devices are used:
 *            - A button to toggle a blink timer.
 *            - A button to read the surrounding temperature.
 *            - A blink timer to toggle an LED when enabled.
 *            - A temperature timer to print the temperature on timeout.
 *            - A debounce timer to reduce the effect of contact bounces after pushing the buttons.
 *            - A serial device to print serial data via UART.
 *            - A watchdog timer to restart the program if it gets stuck somewhere.
 *            - An EEPROM stream to store the LED state. On startup, this value is read; if the
 *              last stored state before power down was "on," the LED will automatically blink.
 *            - A temperature sensor to read the surrounding temperature.
 * 
 *        This class is non-copyable and non-movable.
 */
class Logic final : public Interface
{
public:
    /**
     * @brief Create a new logic implementation for a given system.
     *     
     * @param[in] led The LED to toggle.
     * @param[in] toggleButton Button to toggle the toggle timer.
     * @param[in] tempButton Button to read the temperature.
     * @param[in] debounceTimer Timer to mitigate effects of contact bounces.
     * @param[in] toggleTimer Timer to toggle the LED.
     * @param[in] tempTimer Timer to read the temperature.
     * @param[in] serial Serial device to print status messages.
     * @param[in] watchdog Watchdog timer that resets the program if it becomes unresponsive.
     * @param[in] eeprom EEPROM stream to write the status of the LED to EEPROM.
     * @param[in] tempSensor Temperature sensor.
     */
    explicit Logic(driver::gpio::Interface& led,
                   driver::gpio::Interface& toggleButton,
                   driver::gpio::Interface& tempButton, 
                   driver::timer::Interface& debounceTimer, 
                   driver::timer::Interface& toggleTimer,
                   driver::timer::Interface& tempTimer,
                   driver::serial::Interface& serial, 
                   driver::watchdog::Interface& watchdog, 
                   driver::eeprom::Interface& eeprom, 
                   driver::tempsensor::Interface& tempSensor) noexcept;

    /**
     * @brief Destructor.
     */
    ~Logic() noexcept override;

    /**
     * @brief Run the system as long as voltage is supplied.                                                               
     */
    void run() noexcept override;

    /**
     * @brief Handle button event.
     * 
     *        Toggle the timer whenever the toggle button is pressed. 
     *        Predict the temperature and restart the temperature timer whenever the temperature 
     *        button is pressed.
     * 
     *        Pin change interrupts are disabled for a debounce period after detecting button
     *        activity to mitigate the effects of contact bounce.
     */
    void handleButtonEvent() noexcept override;

    /**
     * @brief Handle debounce timer timerout.
     * 
     *        Enable pin change interrupts after a debounce period following button activity to 
     *        mitigate the effects of contact bounce.
     */
    void handleDebounceTimerTimeout() noexcept override;

    /**
     * @brief Handle toggle timer timeout.
     * 
     *        Toggle the LED when the associated timer is enabled.
     */
    void handleToggleTimerTimeout() noexcept override;

    /**
     * @brief Handle temperature timer timeout.
     * 
     *        Read the surrounding temperature.
     */
    void handleTempTimerTimeout() noexcept override;

    Logic()                        = delete; // No default constructor.
    Logic(const Logic&)            = delete; // No copy constructor.
    Logic(Logic&&)                 = delete; // No move constructor.
    Logic& operator=(const Logic&) = delete; // No copy assignment.
    Logic& operator=(Logic&&)      = delete; // No move assignment.

private:
    void handleToggleButtonPressed() noexcept;
    void handleTempButtonPressed() noexcept;
    void checkLedStateInEeprom() noexcept;
    void writeLedStateToEeprom() noexcept;
    bool readLedStateFromEeprom() const noexcept;

    /** Reference to the LED to toggle. */
    driver::gpio::Interface& myLed;

    /** Button to toggle the toggle timer. */
    driver::gpio::Interface& myToggleButton;

    /** Button to read the temperature. */
    driver::gpio::Interface& myTempButton;

    /** Debounce timer to mitigate effects of contact bounces. */
    driver::timer::Interface& myDebounceTimer;

    /** Timer to toggle the LED. */
    driver::timer::Interface& myToggleTimer;

    /** Timer to read the temperature. */
    driver::timer::Interface& myTempTimer;

    /** Serial device to print status messages. */
    driver::serial::Interface& mySerial;

    /** Watchdog timer that resets the program if it becomes unresponsive. */
    driver::watchdog::Interface& myWatchdog;

    /** EEPROM stream to write the status of the LED to EEPROM. */
    driver::eeprom::Interface& myEeprom;

    /** Temperature sensor. */
    driver::tempsensor::Interface& myTempSensor;
};
} // namespace logic