#pragma once

#include <cstdint>
#include <string>
#include "esp_log.h"

namespace ina226
{
    struct StatusRegister
    {
        static constexpr uint8_t reg_addr = 0x06;

        // Champs dynamiques
        bool shunt_over_limit = false;
        bool shunt_under_limit = false;
        bool bus_over_limit = false;
        bool bus_under_limit = false;
        bool power_over_limit = false;
        bool conversion_ready = false;
        bool alert_flag = false;

        // Champ brut pour log/debug
        uint16_t raw_value = 0;

        void decode(uint16_t reg);
        std::string to_json() const;
        void log() const;
    };
}
