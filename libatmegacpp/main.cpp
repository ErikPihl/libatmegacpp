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
#include "driver/adc/atmega328p.h"
#include "driver/eeprom/atmega328p.h"
#include "driver/gpio/atmega328p.h"
#include "driver/serial/atmega328p.h"
#include "driver/timer/atmega328p.h"
#include "driver/watchdog/atmega328p.h"
#include "logic/logic.h"

using namespace driver;

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
    gpio::Atmega328p led{8U, gpio::Atmega328p::Direction::Output};
    gpio::Atmega328p button{13U, gpio::Atmega328p::Direction::InputPullup, buttonCallback};

    // Initialize the timers.
    timer::Atmega328p debounceTimer{debounceTimerTimeout, debounceTimerCallback};
    timer::Atmega328p toggleTimer{toggleTimerTimeout, toggleTimerCallback};

    // Obtain a reference to the singleton serial device instance.
    auto& serial{serial::Atmega328p::getInstance()};

    // Obtain a reference to the singleton watchdog timer instance.
    auto& watchdog{watchdog::Atmega328p::getInstance()};

    // Obtain a reference to the singleton EEPROM instance.
    auto& eeprom{eeprom::Atmega328p::getInstance()};

    // Obtain a reference to the singleton ADC instance.
    auto& adc{adc::Atmega328p::getInstance()};

    // Initialize the logic implementation with the given hardware.
    logic::Logic logic{led, button, debounceTimer, toggleTimer, serial, watchdog, eeprom, adc};
    myLogic = &logic;

    // Run the application perpetually on the target MCU.
    myLogic->run();

    // This point should never be reached; the system is intended to run indefinitely on the target MCU.
    return 0;
}
