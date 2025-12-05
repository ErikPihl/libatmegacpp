/**
 * @brief Demonstration of GPIO device drivers in C++.
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
 */
#include "driver/adc/atmega328p.h"
#include "driver/eeprom/atmega328p.h"
#include "driver/gpio/atmega328p.h"
#include "driver/serial/atmega328p.h"
#include "driver/tempsensor/tmp36.h"
#include "driver/timer/atmega328p.h"
#include "driver/watchdog/atmega328p.h"
#include "logic/logic.h"

using namespace driver;

namespace
{
/** Pointer to the logic implementation. */
logic::Logic* myLogic{nullptr};

namespace callback
{
/**
 * @brief Callback for the buttons.
 * 
 *        This callback is invoked when a button event occurs.
 */
void button() noexcept { myLogic->handleButtonEvent(); }

/**
 * @brief Callback for the debounce timer.
 * 
 *        This callback is invoked when the debounce timer times out.
 */
void debounceTimer() noexcept { myLogic->handleDebounceTimerTimeout(); }

/**
 * @brief Callback for the toggle timer.
 * 
 *        This callback is invoked when the toggle timer times out.
 */
void toggleTimer() noexcept { myLogic->handleToggleTimerTimeout(); }

/**
 * @brief Callback for the temperature timer.
 * 
 *        This callback is invoked when the temperature timer times out.
 */
void tempTimer() noexcept { myLogic->handleTempTimerTimeout(); }

} // namespace callback
} // namespace

/**
 * @brief Initialize and run the system on the target MCU.
 * 
 * @return 0 on termination of the program (should never occur).
 */
int main()
{
    // Set pin numbers.
    constexpr uint8_t tempSensorPin{2U};
    constexpr uint8_t ledPin{8U};
    constexpr uint8_t toggleButtonPin{12U};
    constexpr uint8_t tempButtonPin{13U};

    // Set timeouts.
    constexpr uint32_t debounceTimerTimeout{300U};
    constexpr uint32_t toggleTimerTimeout{100U};
    constexpr uint32_t tempTimerTimeout{60000U};

    constexpr auto input{gpio::Atmega328p::Direction::InputPullup};
    constexpr auto output{gpio::Atmega328p::Direction::Output};

    // Initialize the GPIO devices.
    gpio::Atmega328p led{ledPin, output};
    gpio::Atmega328p toggleButton{toggleButtonPin, input, callback::button};
    gpio::Atmega328p tempButton{tempButtonPin, input, callback::button};

    // Initialize the timers.
    timer::Atmega328p debounceTimer{debounceTimerTimeout, callback::debounceTimer};
    timer::Atmega328p toggleTimer{toggleTimerTimeout, callback::toggleTimer};
    timer::Atmega328p tempTimer{tempTimerTimeout, callback::tempTimer};

    // Obtain a reference to the singleton serial device instance.
    auto& serial{serial::Atmega328p::getInstance()};

    // Obtain a reference to the singleton watchdog timer instance.
    auto& watchdog{watchdog::Atmega328p::getInstance()};

    // Obtain a reference to the singleton EEPROM instance.
    auto& eeprom{eeprom::Atmega328p::getInstance()};

    // Obtain a reference to the singleton ADC instance.
    auto& adc{adc::Atmega328p::getInstance()};

    // Initialize the TMP36 temperature sensor.
    tempsensor::Tmp36 tempSensor{tempSensorPin, adc};

    // Initialize the logic implementation with the given hardware.
    logic::Logic logic{led, 
                       toggleButton, 
                       tempButton, 
                       debounceTimer, 
                       toggleTimer, 
                       tempTimer,
                       serial, 
                       watchdog, 
                       eeprom, 
                       tempSensor};
    myLogic = &logic;

    // Run the application perpetually on the target MCU.
    const bool stop{false};
    myLogic->run(stop);

    // This point should never be reached; the system is intended to run indefinitely on the target MCU.
    return 0;
}
