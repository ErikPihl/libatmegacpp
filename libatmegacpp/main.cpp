/**
 * @brief Demonstration of GPIO device drivers in C++:
 * 
 *        The following devices are used:
 *            - A button connected to pin 13 on the device toggles a timer.
 *            - The aforementioned timer toggles an LED every 100 ms when enabled.
 *            - Another timer reduces the effect of contact bounces after pushing the button.
 *            - A watchdog timer is used to restart the program if it gets stuck somewhere.
 *            - An EEPROM stream is used to store the LED state. On startup, this value is read;
 *              if the last stored state before power down was "on," the LED will automatically blink.
 */
#include "driver/atmega328p/adc.h"
#include "driver/atmega328p/eeprom.h"
#include "driver/atmega328p/gpio.h"
#include "driver/atmega328p/serial.h"
#include "driver/atmega328p/timer.h"
#include "driver/atmega328p/watchdog.h"
#include "logic/logic.h"

using namespace driver::atmega328p;

namespace
{
/** Pointer to the logic implementation. */
logic::Logic* myLogic{nullptr};

/**
 * @brief Callback for the button.
 * 
 *        This callback is invoked when a button event occurs.
 */
void buttonCallback() noexcept { myLogic->handleButtonEvent(); }

/**
 * @brief Callback for the debounce timer timeout.
 * 
 *        This callback is invoked when the debounce timer times out.
 */
void debounceTimerCallback() noexcept { myLogic->handleDebounceTimerTimeout(); }

/**
 * @brief Callback for the toggle timeout.
 * 
 *        This callback is invoked when the toggle timer times out.
 */
void toggleTimerCallback() noexcept { myLogic->handleToggleTimerTimeout(); }

} // namespace

/**
 * @brief Initialize and run the system on the target MCU.
 * 
 * @return 0 on termination of the program (should never occur).
 */
int main()
{
    constexpr uint32_t debounceTimerTimeout{300U};
    constexpr uint32_t toggleTimerTimeout{100U};

    // Initialize the GPIO devices.
    Gpio led{8U, Gpio::Direction::Output};
    Gpio button{13U, Gpio::Direction::InputPullup, buttonCallback};

    // Initialize the timers.
    Timer debounceTimer{debounceTimerTimeout, debounceTimerCallback};
    Timer toggleTimer{toggleTimerTimeout, toggleTimerCallback};

    // Obtain a reference to the singleton serial device instance.
    auto& serial{Serial::getInstance()};

    // Obtain a reference to the singleton watchdog timer instance.
    auto& watchdog{Watchdog::getInstance()};

    // Obtain a reference to the singleton EEPROM instance.
    auto& eeprom{Eeprom::getInstance()};

    // Obtain a reference to the singleton ADC instance.
    auto& adc{Adc::getInstance()};

    // Initialize the logic implementation with the given hardware.
    logic::Logic logic{led, button, debounceTimer, toggleTimer, serial, watchdog, eeprom, adc};
    myLogic = &logic;

    // Run the application perpetually on the target MCU.
    myLogic->run();

    // This point should never be reached; the system is intended to run indefinitely on the target MCU.
    return 0;
}
