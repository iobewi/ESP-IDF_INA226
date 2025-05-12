#pragma once

#include "esp_err.h"
#include <cstdint>
#include <string>

#include "interface.hpp"




namespace ina226
{
    static_assert(std::is_class<ina226::INTERFACE>::value, "ina226::INTERFACE is not a class");
    class CTRL : public ina226::INTERFACE
    {
    public:
        explicit CTRL(I2CDevices &dev) : ina226::INTERFACE(dev) {}

        int32_t shunt_voltage_uv;
        uint32_t bus_voltage_mv;
        uint32_t power_mw;
        int32_t current_ma;

        esp_err_t get_shunt_voltage();
        esp_err_t get_bus_voltage();
        esp_err_t get_power();
        esp_err_t get_current();
        esp_err_t get();

        esp_err_t send_reset();

        void log() const;
        std::string to_json() const;

    private:
        esp_err_t read_u16(uint8_t reg, uint16_t &out);
        esp_err_t read_s16(uint8_t reg, int16_t &out);

        static constexpr uint8_t REG_CONFIG = 0X00;
        static constexpr uint8_t REG_SHUNT_VOLTAGE = 0X01;
        static constexpr uint8_t REG_BUS_VOLTAGE = 0X02;
        static constexpr uint8_t REG_POWER = 0X03;
        static constexpr uint8_t REG_CURRENT = 0X04;

        static constexpr uint16_t RESET_COMMAND = 1 << 15; // [15] Reset (0 = pas de reset, 1 = reset)
    };

} // namespace ina226
