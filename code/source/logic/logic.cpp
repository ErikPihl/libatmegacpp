/**
 * @brief Generic logic implementation details for an MCU with configurable hardware devices.
 */
#include <stdint.h>

#include "driver/adc/interface.h"
#include "driver/eeprom/interface.h"
#include "driver/gpio/interface.h"
#include "driver/serial/interface.h"
#include "driver/tempsensor/interface.h"
#include "driver/timer/interface.h"
#include "driver/watchdog/interface.h"
#include "logic/logic.h"

namespace logic
{
// -----------------------------------------------------------------------------
Logic::Logic(driver::gpio::Interface& led,
             driver::gpio::Interface& toggleButton,
             driver::gpio::Interface& tempButton, 
             driver::timer::Interface& debounceTimer, 
             driver::timer::Interface& toggleTimer,
             driver::timer::Interface& tempTimer,
             driver::serial::Interface& serial, 
             driver::watchdog::Interface& watchdog, 
             driver::eeprom::Interface& eeprom, 
             driver::tempsensor::Interface& tempSensor) noexcept
    : myLed{led}
    , myToggleButton{toggleButton}
    , myTempButton{tempButton}
    , myDebounceTimer{debounceTimer}
    , myToggleTimer{toggleTimer}
    , myTempTimer{tempTimer}
    , mySerial{serial}
    , myWatchdog{watchdog}
    , myEeprom{eeprom}
    , myTempSensor{tempSensor}
{
    // Initialize system.
    myToggleButton.enableInterrupt(true);
    myTempButton.enableInterrupt(true);
    myTempTimer.start();
    mySerial.setEnabled(true);
    myWatchdog.setEnabled(true);
    myEeprom.setEnabled(true);
    checkLedStateInEeprom();
}

// -----------------------------------------------------------------------------
Logic::~Logic() noexcept
{
    // Disable system.
    myLed.write(false);
    myToggleButton.enableInterrupt(false);
    myTempButton.enableInterrupt(false);
    myDebounceTimer.stop();
    myToggleTimer.stop();
    myTempTimer.stop();
    mySerial.setEnabled(false);
    myWatchdog.setEnabled(false);
    myEeprom.setEnabled(false);
    myToggleTimer.stop();
}

// -----------------------------------------------------------------------------
void Logic::run(const bool& stop) noexcept
{
    mySerial.printf("Running the system!\n");

    // Run the system continuously.
    while (!stop) 
    { 
        // Regularly reset the watchdog to avoid system reset.
        myWatchdog.reset(); 
    }
}

// -----------------------------------------------------------------------------
void Logic::handleButtonEvent() noexcept
{
    // Disable interrupts on the I/O ports to mitigate effects of debouncing.
    myToggleButton.enableInterruptOnPort(false);
    myTempButton.enableInterruptOnPort(false);
    myDebounceTimer.start();

    // Handle specific button event when pressed.
    if (myToggleButton.read()) { handleToggleButtonPressed(); }
    else if (myTempButton.read()) { handleTempButtonPressed(); }
}

// -----------------------------------------------------------------------------
void Logic::handleDebounceTimerTimeout() noexcept
{
    // Re-enable interrupts on the ports after debounce timer timeout.
    myDebounceTimer.stop();
    myToggleButton.enableInterruptOnPort(true);
    myTempButton.enableInterruptOnPort(true);
}

// -----------------------------------------------------------------------------
void Logic::handleToggleTimerTimeout() noexcept 
{
    // Toggle the LED on toggle timer timeout. 
    myLed.toggle(); 
}

// -----------------------------------------------------------------------------
void Logic::handleTempTimerTimeout() noexcept 
{ 
    // Read and print the temperature on temperature timer timeout.
    const int16_t temperature{myTempSensor.read()};
    mySerial.printf("Temperature: %d\n", temperature);
}

// -----------------------------------------------------------------------------
void Logic::handleToggleButtonPressed() noexcept
{
    // Toggle the toggle timer on pressdown, safe the current LED state in EEPROM.
    myToggleTimer.toggle();
    writeLedStateToEeprom();

    if (myToggleTimer.isEnabled()) { mySerial.printf("Toggle timer enabled!\n"); }
    else
    {
        // Immediately disable the LED if the toggle timer is disabled to ensure that the LED
        // isn't stuck in an enabled state.
        mySerial.printf("Toggle timer disabled!\n");
        myLed.write(false);
    }
}

// -----------------------------------------------------------------------------
void Logic::handleTempButtonPressed() noexcept
{
    // Read and print the temperature on pressdown.
    // Restart the temperature timer.
    const int16_t temperature{myTempSensor.read()};
    mySerial.printf("Temperature: %d\n", temperature);
    myTempTimer.restart();
}

// -----------------------------------------------------------------------------
void Logic::checkLedStateInEeprom() noexcept
{
    if (readLedStateFromEeprom())
    {
        myToggleTimer.start();
        mySerial.printf("Toggle timer enabled!\n");
    }
}

// -----------------------------------------------------------------------------
void Logic::writeLedStateToEeprom() noexcept
{ 
    myEeprom.write(LedState::Address, myToggleTimer.isEnabled());
}

// -----------------------------------------------------------------------------
bool Logic::readLedStateFromEeprom() const noexcept
{
    uint8_t state{};
    return myEeprom.read(LedState::Address, state) ? LedState::Enabled == state : false;
}
} // namespace logic