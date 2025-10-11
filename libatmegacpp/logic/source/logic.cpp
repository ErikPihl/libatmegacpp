/**
 * @brief Generic logic implementation details for an MCU with configurable hardware devices.
 */
#include <stdint.h>

#include "driver/adc/interface.h"
#include "driver/eeprom/interface.h"
#include "driver/gpio/interface.h"
#include "driver/serial/interface.h"
#include "driver/timer/interface.h"
#include "driver/watchdog/interface.h"
#include "logic/logic.h"

namespace logic
{
/**
 * @brief Structure of LED state parameters.
 */
namespace LedState
{
    /** LED state address in EEPROM. */
    static constexpr uint8_t address{0U};

    /** Enabled state value in EEPROM. */
    static constexpr uint8_t enabled{1U};
};

// -----------------------------------------------------------------------------
Logic::Logic(driver::gpio::Interface& led, driver::gpio::Interface& button,
             driver::timer::Interface& debounceTimer, driver::timer::Interface& toggleTimer,
               driver::serial::Interface& serial, driver::watchdog::Interface& watchdog,
               driver::eeprom::Interface& eeprom, driver::adc::Interface& adc) noexcept
    : myLed{led}
    , myButton{button}
    , myDebounceTimer{debounceTimer}
    , myToggleTimer{toggleTimer}
    , mySerial{serial}
    , myWatchdog{watchdog}
    , myEeprom{eeprom}
    , myAdc{adc}
{
    myButton.enableInterrupt(true);
    mySerial.setEnabled(true);
    myWatchdog.setEnabled(true);
    myEeprom.setEnabled(true);
    checkLedStateInEeprom();
}

// -----------------------------------------------------------------------------
Logic::~Logic() noexcept
{
    myLed.write(false);
    myButton.enableInterrupt(false);
    myDebounceTimer.stop();
    myToggleTimer.stop();
    myWatchdog.setEnabled(false);
}

// -----------------------------------------------------------------------------
void Logic::enableSerialTransmission(const bool enable) noexcept
{
    mySerial.setEnabled(enable);
}

// -----------------------------------------------------------------------------
void Logic::handleButtonEvent() noexcept
{
    myButton.enableInterruptOnPort(false);
    myDebounceTimer.start();
    if (myButton.read()) { handleButtonPressed(); }
}

// -----------------------------------------------------------------------------
void Logic::handleDebounceTimerTimeout() noexcept
{
    myDebounceTimer.stop();
    myButton.enableInterruptOnPort(true);
}

// -----------------------------------------------------------------------------
void Logic::handleToggleTimerTimeout() noexcept 
{ 
    mySerial.printf("Toggling the LED!\n");
    myLed.toggle(); 
}

// -----------------------------------------------------------------------------
void Logic::run() noexcept
{
    mySerial.printf("Running the system!\n");
    
    while (1)
    {
        myWatchdog.reset();
    }
}

// -----------------------------------------------------------------------------
void Logic::handleButtonPressed() noexcept
{
    mySerial.printf("Button pressed!\n");
    myToggleTimer.toggle();
    writeLedStateToEeprom();

    if (myToggleTimer.isEnabled()) { mySerial.printf("Toggle timer enabled!\n"); }
    else
    {
        mySerial.printf("Toggle timer disabled!\n");
        myLed.write(false);
    }
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
    myEeprom.write(LedState::address, myToggleTimer.isEnabled());
}

// -----------------------------------------------------------------------------
bool Logic::readLedStateFromEeprom() const noexcept
{
    uint8_t state{};
    return myEeprom.read(LedState::address, state) ? LedState::enabled == state : false;
}
} // namespace logic