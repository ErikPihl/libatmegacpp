/**
 * @brief Serial driver stub.
 */
#pragma once

#include <stdint.h>

#ifdef TESTSUITE
#include <iostream>
#endif

#include "driver/serial/interface.h"

namespace driver
{
namespace serial
{
/**
 * @brief Serial driver stub.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     * 
     * @param[in] baudRate_bps The baud rate in bits per second (default = 9600 bps).
     */
    explicit Stub(const uint32_t baudRate_bps = 9600U) noexcept
        : myBaudRate_bps{baudRate_bps}
        , myReceivedByte{}
        , myEnabled{true}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /** 
     * @brief Get the baud rate of the serial device. 
     * 
     * @return The baud rate in bps (bits per second).
     */
    uint32_t baudRate_bps() const noexcept override { return myBaudRate_bps; }

    /**
     * @brief Check whether the serial device is initialized.
     * 
     * @return True if the device is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Check whether the serial device is enabled.
     * 
     * @return True if the serial device is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Set enablement of serial device.
     * 
     * @param[in] enable Indicate whether to enable the device.
     */
    void setEnabled(const bool enable) noexcept override { myEnabled = enable; }
    
    /**
     * @brief Receive one byte of data.
     * 
     * @return The received byte.
     */
    uint8_t readByte() const noexcept override { return myReceivedByte; }

    /**
     * @brief Print the given string in the serial terminal.
     * 
     * @param[in] str The string to print.
     */
    void print(const char* str) const noexcept override
    {
        // Print in the terminal when testing.
        if ((!myEnabled) || (NULL == str)) { return; }
        #ifdef TESTSUITE
             std::cout << str;
        #endif
    }

    /**
     * @brief Simulate a received byte.
     * 
     * @param[in] byte The received byte.
     */
    void setByte(const uint8_t byte) noexcept { myReceivedByte = byte; }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Baud rate in bps (bits per second). */
    const uint32_t myBaudRate_bps;

    /** Received byte. */
    uint8_t myReceivedByte;

    /** Indicate whether serial transmission is enabled. */
    bool myEnabled;
};
} // namespace serial
} // namespace driver
