/**
 * @brief GPIO driver implementation details for ATmega328P.
 */
#include "arch/avr/hw_platform.h"
#include "driver/gpio/atmega328p.h"
#include "utils/callback_array.h"
#include "utils/utils.h"

namespace driver 
{
namespace gpio
{
namespace
{
/**
 * @brief Structure of indexes for callbacks associated with the I/O ports.
 */
struct CbIndex
{
    /** Index for callback associated with I/O port B. */
    static constexpr uint8_t PortB{0U};

    /** Index for callback associated with I/O port C. */
    static constexpr uint8_t PortC{1U};

    /** Index for callback associated with I/O port D. */
    static constexpr uint8_t PortD{2U};
};

/**
 * @brief Structure of pin offsets, i.e. the discrepancy between the Arduino and the ATmega328p
 *        pin numbers, for each I/O port.
 */
struct PinOffset
{
    /** Pin offset for I/O port B. */
    static constexpr uint8_t PortB{8U};

    /** Pin offset for I/O port C. */
    static constexpr uint8_t PortC{14U};

    /** Pin offset for I/O port D. */
    static constexpr uint8_t PortD{0U};
};

/** The number of available I/O ports. */
constexpr uint8_t IoPortCount{3U};

/** The number of available GPIO pins. */
constexpr uint8_t PinCount{20U};

/** Pointers to callbacks. */
container::CallbackArray<IoPortCount> myCallbacks{};

/** Pin registry (1 = reserved, 0 = free). */
uint32_t myPinRegistry{};

// -----------------------------------------------------------------------------
constexpr bool isPinFree(const uint8_t id) noexcept
{
    return PinCount > id ? !utils::read(myPinRegistry, id) : false;
}

// -----------------------------------------------------------------------------
constexpr bool isDirectionValid(const Direction direction) noexcept
{
    return static_cast<uint8_t>(Direction::Count) > static_cast<uint8_t>(direction);
}
} // namespace

/**
 * @brief GPIO hardware structure.
 */
struct Atmega328p::Hardware 
{
    /** Reference to data direction register (DDRx). */
    volatile uint8_t& dirReg;

    /** Reference to port (output) register (PORTx). */
    volatile uint8_t& portReg;

    /** Reference to pin (input) register (PINx). */
    volatile uint8_t& pinReg;

    /** Reference to pin change interrupt mask register (PCMSKx). */
    volatile uint8_t& pcMskReg;

    /** Control bit in the pin change interrupt control register (PCIEx). */
    const uint8_t pciBit;

    /** Pin offset associated with the port. */
    const uint8_t pinOffset;

    /** I/O port associated with the GPIO. */
    const IoPort port;
};

/** Hardware structure for I/O port B. */
struct Atmega328p::Hardware Atmega328p::myHwPortB
{
    .dirReg    = DDRB,
    .portReg   = PORTB,
    .pinReg    = PINB,
    .pcMskReg  = PCMSK0,
    .pciBit    = PCIE0,
    .pinOffset = PinOffset::PortB,
    .port      = Atmega328p::IoPort::B,
};

/** Hardware structure for I/O port C. */
struct Atmega328p::Hardware Atmega328p::myHwPortC
{
    .dirReg    = DDRC,
    .portReg   = PORTC,
    .pinReg    = PINC,
    .pcMskReg  = PCMSK1,
    .pciBit    = PCIE1,
    .pinOffset = PinOffset::PortC,
    .port      = Atmega328p::IoPort::C,
};

/** Hardware structure for I/O port D. */
struct Atmega328p::Hardware Atmega328p::myHwPortD
{
    .dirReg    = DDRD,
    .portReg   = PORTD,
    .pinReg    = PIND,
    .pcMskReg  = PCMSK2,
    .pciBit    = PCIE2,
    .pinOffset = PinOffset::PortD,
    .port      = Atmega328p::IoPort::D,
};

// -----------------------------------------------------------------------------
Atmega328p::Atmega328p(const uint8_t pin, const Direction direction, void (*callback)()) noexcept
    : myHw{reserve(pin, direction)}
    , myId{pin}
    , myPin{getPhysicalPin()}
{ 
    // Set up GPIO if initialization succeeded.
    if (isInitialized()) { setup(direction, callback); }
}

// -----------------------------------------------------------------------------
Atmega328p::~Atmega328p() noexcept 
{   
    // Free resources used for the GPIO before deletion.
    if (isInitialized())
    {
        enableInterrupt(false);
        utils::clear(myHw->dirReg, myPin);
        utils::clear(myHw->portReg, myPin);
        utils::clear(myPinRegistry, myId);
        myHw = nullptr; 
    }
}

// -----------------------------------------------------------------------------
bool Atmega328p::isInitialized() const noexcept { return nullptr != myHw; }

// -----------------------------------------------------------------------------
bool Atmega328p::read() const noexcept 
{ 
    return isInitialized() ? utils::read(myHw->pinReg, myPin) : false;
}

// -----------------------------------------------------------------------------
void Atmega328p::write(const bool output) noexcept
{
    if (!isInitialized()) { return; }

    // Set/clear the output as requested.
    if (output) { utils::set(myHw->portReg, myPin); }
    else { utils::clear(myHw->portReg, myPin); }
}

// -----------------------------------------------------------------------------
void Atmega328p::toggle() noexcept 
{ 
    if (!isInitialized()) { return; }
    utils::set(myHw->pinReg, myPin); 
}

// -----------------------------------------------------------------------------
void Atmega328p::enableInterruptOnPort(const bool enable) noexcept 
{ 
    if (!isInitialized()) { return; }

    // Enable/disable interrupts on the associated port as requested.
    if (enable) { utils::set(PCICR, myHw->pciBit); }
    else { utils::clear(PCICR, myHw->pciBit); }
}

// -----------------------------------------------------------------------------
void Atmega328p::enableInterrupt(const bool enable) noexcept
{
    if (!isInitialized()) { return; }

    // Enable/disable interrupts on the associated pin as requested.
    if (enable)
    {
        utils::globalInterruptEnable();
        utils::set(PCICR, myHw->pciBit);
        utils::set(myHw->pcMskReg, myPin);
    }
    else { utils::clear(myHw->pcMskReg, myPin); }
}

// -----------------------------------------------------------------------------
void Atmega328p::blink(const uint16_t& blinkSpeed_ms) noexcept
{
    toggle();
    utils::delay_ms(blinkSpeed_ms);
}

// -----------------------------------------------------------------------------
uint8_t Atmega328p::getPhysicalPin() const noexcept
{
    return nullptr != myHw ? myId - myHw->pinOffset : static_cast<uint8_t>(-1);
}

// -----------------------------------------------------------------------------
void Atmega328p::setup(const Direction direction, void (*callback)()) const noexcept
{
    // Set the GPIO direction by writing to the hardware registers.
    if (Direction::InputPullup == direction) { utils::set(myHw->portReg, myPin); } 
    else if (Direction::Output == direction) { utils::set(myHw->dirReg, myPin); }

    // Register the given callback for the associated I/O port.
    if (nullptr != callback)
    {
        if (PORTB == myHw->portReg) { myCallbacks.add(callback, CbIndex::PortB); }
        else if (PORTC == myHw->portReg) { myCallbacks.add(callback, CbIndex::PortC); } 
        else if (PORTD == myHw->portReg) { myCallbacks.add(callback, CbIndex::PortD); }
    }
}

// -----------------------------------------------------------------------------
Atmega328p::Hardware* Atmega328p::reserve(const uint8_t id, const Direction direction) noexcept
{
    // Return a nullptr if the given pin is reserved or the direction is invalid.
    if (!isPinFree(id) || !isDirectionValid(direction)) { return nullptr; }

    // Initialize the hardware, register the given pin on success.
    Hardware* hardware{findHardware(id)};
    if (nullptr != hardware) { utils::set(myPinRegistry, id); }

    // Return a pointer to the hardware used, or a nullptr on failure.
    return hardware;
}

// -----------------------------------------------------------------------------
Atmega328p::Hardware* Atmega328p::findHardware(const uint8_t id) noexcept
{
    // Return the hardware associated with the given ID, or nullptr on failure.
    if (utils::inRange(id, Port::B0, Port::B5))      { return &myHwPortB; }
    else if (utils::inRange(id, Port::C0, Port::C5)) { return &myHwPortC; }
    else if (utils::inRange(id, Port::D0, Port::D7)) { return &myHwPortD; }
    return nullptr;
}

// -----------------------------------------------------------------------------
ISR(PCINT0_vect) { myCallbacks.invoke(CbIndex::PortB); }

// -----------------------------------------------------------------------------
ISR(PCINT1_vect) { myCallbacks.invoke(CbIndex::PortC); }

// -----------------------------------------------------------------------------
ISR(PCINT2_vect) { myCallbacks.invoke(CbIndex::PortD); }

} // namespace gpio
} // namespace driver
