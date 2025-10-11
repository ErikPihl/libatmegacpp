/**
 * @brief Generic logic implementation for an MCU with configurable hardware devices.
 */
#pragma once

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
 * 
 *            - A button connected toggles a timer.
 * 
 *            - The aforementioned timer toggles an LED after a given duration when enabled.
 * 
 *            - Another timer reduces the effect of contact bounces after pushing the button.
 * 
 *            - A watchdog timer is used to restart the program if it gets stuck somewhere.
 *
 *            - The LED state is written to EEPROM upon every change. This value is evaluated upon startup.
 * 
 *        This class is non-copyable and non-movable.
 */
class Logic final
{
public:
    /**
     * @brief Create a new logic implementation for a given system.
     *     
     * @param[in] led The LED to toggle.
     * @param[in] button Button used to toggle the toggle timer.
     * @param[in] debounceTimer Timer used to mitigate effects of contact bounces.
     * @param[in] toggleTimer Timer used to toggle the LED.
     * @param[in] serial Serial device used to print status messages.
     * @param[in] watchdog Watchdog timer that resets the program if it becomes unresponsive.
     * @param[in] eeprom EEPROM stream to write the status of the LED to EEPROM.
     * @param[in] adc ADC (currently unused).
     */
    explicit Logic(driver::gpio::Interface& led, driver::gpio::Interface& button, 
                   driver::timer::Interface& debounceTimer, driver::timer::Interface& toggleTimer,
                   driver::serial::Interface& serial, driver::watchdog::Interface& watchdog, 
                   driver::eeprom::Interface& eeprom, driver::adc::Interface& adc) noexcept;

    /**
     * @brief Delete the logic implementation.
     */
    ~Logic() noexcept;

    /**
     * @brief Enable serial transmission.
     * 
     * @param[in] enable Indicate whether to enable serial transmission.
     */
    void enableSerialTransmission(const bool enable) noexcept;

    /**
     * @brief Handle button event.
     * 
     *        Toggle the timer whenever the button is pressed. 
     * 
     *        Pin change interrupts are disabled for a debounce period after detecting button
     *        activity to mitigate the effects of contact bounce.
     */
    void handleButtonEvent() noexcept;

    /**
     * @brief Handle debounce timer timerout.
     * 
     *        Enable pin change interrupts after a debounce period following button activity to 
     *        mitigate the effects of contact bounce.
     */
    void handleDebounceTimerTimeout() noexcept;

    /**
     * @brief Handle toggle timer timeout.
     * 
     *        Toggle the LED when the associated timer is enabled.
     */
    void handleToggleTimerTimeout() noexcept;

    /**
     * @brief Run the system as long as voltage is supplied.                                                               
     */
    void run() noexcept;

    Logic()                        = delete; // No default constructor.
    Logic(const Logic&)            = delete; // No copy constructor.
    Logic(Logic&&)                 = delete; // No move constructor.
    Logic& operator=(const Logic&) = delete; // No copy assignment.
    Logic& operator=(Logic&&)      = delete; // No move assignment.

private:
    void handleButtonPressed() noexcept;
    void checkLedStateInEeprom() noexcept;
    void writeLedStateToEeprom() noexcept;
    bool readLedStateFromEeprom() const noexcept;

    /** Reference to the LED to toggle. */
    driver::gpio::Interface& myLed;

    /** Button used to toggle the toggle timer. */
    driver::gpio::Interface& myButton;

    /** Debounce timer used to mitigate effects of contact bounces. */
    driver::timer::Interface& myDebounceTimer;

    /** Timer used to toggle the LED. */
    driver::timer::Interface& myToggleTimer;

    /** Serial device used to print status messages. */
    driver::serial::Interface& mySerial;

    /** Watchdog timer that resets the program if it becomes unresponsive. */
    driver::watchdog::Interface& myWatchdog;

    /** EEPROM stream to write the status of the LED to EEPROM. */
    driver::eeprom::Interface& myEeprom;

    /** A/D converter (currently unused). */
    driver::adc::Interface& myAdc;
};
} // namespace logic