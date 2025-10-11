/**
 * @brief Implementation details of serial driver.
 */
#include <avr/io.h>

#include "driver/serial/atmega328p.h"
#include "utils/utils.h"

namespace driver 
{
namespace serial
{
namespace
{
/**
 * @brief Structure holding serial parameters.
 */
struct Param
{
    // Baud rate in bps.
    static constexpr uint32_t BaudRate_bps{9600U};

    // Carriage return character.
    static constexpr char CarriageReturn{'\r'};

    // New line character.
    static constexpr char NewLine{'\n'};
};

// -----------------------------------------------------------------------------
void transmitChar(const char character) noexcept
{
    // Wait until the previous character has been sent.
    while (!utils::read(UCSR0A, UDRE0));

    // Put the new character in the transmission register.
    UDR0 = character;
}
} // namespace 

// -----------------------------------------------------------------------------
Interface& Atmega328p::getInstance() noexcept
{ 
    // Create and initialize the singleton serial instance (once only).
    static Atmega328p myInstance{};

    // Return a reference to the singleton serial instance, cast to the corresponding interface.
    return myInstance; 
}

// -----------------------------------------------------------------------------
uint32_t Atmega328p::baudRate_bps() const { return Param::BaudRate_bps; }

// -----------------------------------------------------------------------------
bool Atmega328p::isInitialized() const noexcept { return true; }

// -----------------------------------------------------------------------------
bool Atmega328p::isEnabled() const noexcept { return myEnabled; }

// -----------------------------------------------------------------------------
void Atmega328p::setEnabled(const bool enable) noexcept { myEnabled = enable; }

// -----------------------------------------------------------------------------
Atmega328p::Atmega328p() noexcept 
    : myEnabled{true}
{ 
    // Baud rate value corresponding to 9600 kbps.
    constexpr uint16_t baudRateValue{103U};

    // Enable UART transmission.
    utils::set(UCSR0B, TXEN0);

    // Set the data size to eight bits per byte.
    utils::set(UCSR0C, UCSZ00, UCSZ01);

    // Set the baud rate to 9600 kbps.
    UBRR0 = baudRateValue;

    // Send carriage return to align the first message left.
    UDR0 = Param::CarriageReturn;
}

// -----------------------------------------------------------------------------
void Atmega328p::print(const char* message) const noexcept
{
    // Terminate the function if serial transmission isn't enabled.
    if (!myEnabled) { return; }

    // Transmit each character of the string one by one.
    for (const char* it{message}; *it; ++it)
    {
        transmitChar(*it);
        
        // Send new line characters instead of carriage returns.
        if (Param::CarriageReturn == *it) 
        { 
            transmitChar(Param::NewLine); 
        }
    }
}
} // namespace serial
} // namespace driver
