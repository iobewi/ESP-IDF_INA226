#pragma once

#include "esp_err.h"
#include <cstdint>
#include "ina226-interface.hpp"
#include "status/ina226-status_types.hpp"  // Contient struct StatusRegister avec decode/log/to_json

namespace ina226
{
    class STATUS : public INTERFACE
    {
    public:
        explicit STATUS(I2CDevices &dev) : INTERFACE(dev) {}

        StatusRegister status; // Représente les flags du registre 0x06 (Mask/Enable)

        /**
         * Lit et décode le registre 0x06 de statut.
         */
        esp_err_t get()
        {
            uint16_t value;
            esp_err_t err = read_u16(StatusRegister::reg_addr, value);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read INA226 status (err=0x%x)", err);
                return err;
            }

            status.decode(value);
            return ESP_OK;
        }

        void log() const { status.log(); }
        std::string to_json() const { return status.to_json(); }

    private:
        inline static const char *TAG = "INA226-STATUS";
    };
} // namespace ina226
