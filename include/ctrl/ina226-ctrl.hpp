#pragma once

#include "esp_err.h"
#include <cstdint>
#include <string>

#include "ina226-interface.hpp"




namespace ina226
{
    static_assert(std::is_class<INTERFACE>::value, "INTERFACE is not a class");
    class CTRL : public INTERFACE
    {
    public:
        explicit CTRL(I2CDevices &dev) : INTERFACE(dev) {}

        int32_t shunt_voltage_uv;
        uint32_t bus_voltage_mv;
        uint32_t power_mw;
        int32_t current_ma;

        esp_err_t ready();
        esp_err_t get_shunt_voltage();
        esp_err_t get_bus_voltage();
        esp_err_t get_power();
        esp_err_t get_current();
        esp_err_t get();

        esp_err_t send_reset();

        void log() const;
        std::string to_json() const;

    private:
        inline static const char *TAG = "INA226-CTRL";
        static constexpr uint8_t REG_CONFIG = 0X00;
        static constexpr uint8_t REG_SHUNT_VOLTAGE = 0X01;
        static constexpr uint8_t REG_BUS_VOLTAGE = 0X02;
        static constexpr uint8_t REG_POWER = 0X03;
        static constexpr uint8_t REG_CURRENT = 0X04;
        static constexpr uint8_t REG_DEVICE_ID = 0xFE;
        static constexpr uint16_t VALUE_DEVICE_ID  = 0x5449;

        static constexpr uint16_t RESET_COMMAND = 1 << 15; // [15] Reset (0 = pas de reset, 1 = reset)
    };

} // namespace ina226
