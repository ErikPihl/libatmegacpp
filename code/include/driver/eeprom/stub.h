/**
 * @brief EEPROM stub.
 */
#pragma once

#include <stdint.h>

#include "driver/eeprom/interface.h"

namespace driver 
{
namespace eeprom
{
/**
 * @brief EEPROM stub.
 * 
 * @tparam MemSize EEPROM size in bytes.
 */
template<uint16_t MemSize>
struct Stub : public Interface
{
    // Generate a compiler error if the EEPROM size is set to 0.
    static_assert(0U < MemSize, "EEPROM size must be larger than 0!");

    /**
     * @brief Create a new EEPROM stub instance.
     */
    Stub() noexcept
        : myMemory{}
        , myEnabled{true}
    {}

    /**
     * @brief Get the size of the EEPROM.
     * 
     * @return The size of the EEPROM in bytes.
     */
    uint16_t size() const noexcept override { return MemSize; }

    /**
     * @brief Check whether the EEPROM stream is initialized.
     * 
     * @return True if the EEPROM stream is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Indicate whether the EEPROM stream is enabled.
     * 
     * @return True if the EEPROM stream is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Set enablement of EEPROM stream.
     * 
     * @param[in] enable Indicate whether to enable the EEPROM stream.
     */
    void setEnabled(const bool enable) noexcept override { myEnabled = enable; }

    /**
     * @brief Check whether the given address is valid.
     * 
     * @param[in] address Starting address.
     * @param[in] dataSize Data size in bytes.
     * 
     * @return True if the address is valid, false otherwise.
     */
    bool isAddressValid(const uint16_t address, const uint8_t dataSize) const noexcept override
    {
        return MemSize > (address + dataSize); 
    }

    /**
     * @brief Write byte in EEPROM.
     * 
     * @param[in] address Destination address.
     * @param[in] data Data to write.
     */
    void writeByte(const uint16_t address, const uint8_t data) noexcept override
    {
        if (myEnabled && (MemSize > address)) { myMemory[address] = data; }
    }

    /**
     * @brief Read byte in EEPROM.
     * 
     * @param[in] address The address to read from.
     * 
     * @return The data at the given address.
     */
    uint8_t readByte(const uint16_t address) const noexcept override
    {
        return myEnabled && (MemSize > address) ? myMemory[address] : 0U;
    }

private:
    /** EEPROM memory. */
    uint8_t myMemory[MemSize]{};

    /** Indicate whether the EEPROM stream is enabled. */
    bool myEnabled;
};
} // namespace eeprom
} // namespace driver
