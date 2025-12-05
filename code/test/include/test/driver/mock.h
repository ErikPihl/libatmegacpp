/**
 * @brief Driver mocks.
 */
#pragma once

#ifdef TESTSUITE

#include <cstdint>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "driver/adc/interface.h"
#include "driver/eeprom/interface.h"
#include "driver/gpio/interface.h"
#include "driver/serial/interface.h"
#include "driver/tempsensor/interface.h"
#include "driver/timer/interface.h"
#include "driver/watchdog/interface.h"

namespace driver::adc
{
/**
 * @brief A/D converter mock.
 */
class Mock : public Interface 
{
public:
    MOCK_METHOD(std::uint8_t, resolution, (), (const, noexcept, override));
    MOCK_METHOD(std::uint16_t, maxValue, (), (const, noexcept, override));
    MOCK_METHOD(double, supplyVoltage, (), (const, noexcept, override));
    MOCK_METHOD(uint16_t, read, (std::uint8_t), (const, noexcept, override));
    MOCK_METHOD(double, dutyCycle, (std::uint8_t), (const, noexcept, override));
    MOCK_METHOD(double, inputVoltage, (std::uint8_t), (const, noexcept, override));
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, isEnabled, (), (const, noexcept, override));
    MOCK_METHOD(void, setEnabled, (bool), (noexcept, override));
    MOCK_METHOD(bool, isChannelValid, (std::uint8_t), (const, noexcept, override));
};
} // namespace driver::adc

namespace driver::eeprom
{
/**
 * @brief EEPROM (Electrically Erasable Programmable ROM) mock.
 */
class Mock : public Interface
{
public:
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, isEnabled, (), (const, noexcept, override));
    MOCK_METHOD(void, setEnabled, (bool), (noexcept, override));
    MOCK_METHOD(bool, isAddressValid, (std::uint16_t, std::uint8_t), (const, noexcept, override));
    MOCK_METHOD(void, writeByte, (std::uint16_t, std::uint8_t), (const, noexcept, override));
    MOCK_METHOD(std::uint8_t, readByte, (std::uint16_t), (const, noexcept, override));
};
} // namespace driver::eeprom

namespace driver::gpio
{
/** 
 * @brief GPIO driver mock. 
 */
class Mock : public Interface
{
public:
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, read, (), (const, noexcept, override));
    MOCK_METHOD(void, write, (bool), (noexcept, override));
    MOCK_METHOD(void, toggle, (), (noexcept, override));
    MOCK_METHOD(void, enableInterrupt, (bool), (noexcept, override));
    MOCK_METHOD(void, enableInterruptOnPort, (bool), (noexcept, override));
};
} // namespace driver::gpio

namespace driver::serial
{
/**
 * @brief Serial driver interface.
 */
class Mock : public Interface 
{
public:
    MOCK_METHOD(std::uint32_t, baudRate_bps, (), (const, noexcept, override));
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, isEnabled, (), (const, noexcept, override));
    MOCK_METHOD(void, setEnabled, (bool), (noexcept, override));
    MOCK_METHOD(void, print, (const char*), (const, noexcept, override));
};
} // namespace driver::serial

namespace driver::tempsensor
{
/**
 * @brief Temperature sensor mock.
 */
class Mock : public Interface
{
public:
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(std::int16_t, read, (), (const, noexcept, override));
};
} // namespace driver::tempsensor

namespace driver::timer
{
/**
 * @brief Timer driver mock.
 */
class Mock : public Interface
{
public:
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, isEnabled, (), (const, noexcept, override));
    MOCK_METHOD(bool, hasTimedOut, (), (noexcept, override));
    MOCK_METHOD(std::uint32_t, timeout_ms, (), (const, noexcept, override));
    MOCK_METHOD(void, setTimeout_ms, (std::uint32_t), (noexcept, override));
    MOCK_METHOD(void, start, (), (noexcept, override));
    MOCK_METHOD(void, stop, (), (noexcept, override));
    MOCK_METHOD(void, toggle, (), (noexcept, override));
    MOCK_METHOD(void, restart, (), (noexcept, override));
};
} // namespace driver::timer

namespace driver::watchdog
{
/**
 * @brief Watchdog timer driver mock.
 */
class Mock : public Interface
{
public:
    MOCK_METHOD(bool, isInitialized, (), (const, noexcept, override));
    MOCK_METHOD(bool, isEnabled, (), (const, noexcept, override));
    MOCK_METHOD(void, setEnabled, (bool), (noexcept, override));
    MOCK_METHOD(std::uint16_t, timeout_ms, (), (const, noexcept, override));
    MOCK_METHOD(void, reset, (), (noexcept, override));
};
} // namespace driver::watchdog

#endif /** IFDEF TESTSUITE */
