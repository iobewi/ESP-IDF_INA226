#pragma once

#include <cstdint>
#include <string>

#include "esp_err.h"

#include "interface.hpp"
#include "config/config_types.hpp"

namespace ina226
{
    class Config : public ina226::INTERFACE
    {
    public:
        Config(I2CDevices &dev, 
            const ConfigParams& params = {});

        template <typename T>
        esp_err_t read_register_and_decode(const char* name, T& reg)
        {
            uint8_t data[2];
            esp_err_t err = read_register(reg.reg_addr, data, 2);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to read %s (err=0x%x)", name, err);
                return err;
            }
        
            reg.decode((data[0] << 8) | data[1]);
            return ESP_OK;
        }
        
        template <typename T>
        esp_err_t write_register_from_encode(const char* name, T& reg, bool verify = false)
        {
            uint16_t val = reg.encode();
            uint8_t data[2] = { static_cast<uint8_t>(val >> 8), static_cast<uint8_t>(val & 0xFF) };
        
            esp_err_t err = write_register(reg.reg_addr, data, 2);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to write %s (err=0x%x)", name, err);
                return err;
            }
        
            if (verify)
            {
                uint8_t readback[2];
                err = read_register(reg.reg_addr, readback, 2);
                if (err != ESP_OK)
                {
                    ESP_LOGE(TAG, "Failed to verify write of %s (err=0x%x)", name, err);
                    return err;
                }
        
                if (readback[0] != data[0] || readback[1] != data[1])
                {
                    ESP_LOGW(TAG, "Verification failed for %s: wrote 0x%02X%02X, read 0x%02X%02X",
                             name, data[0], data[1], readback[0], readback[1]);
                    return ESP_FAIL;
                }
            }
        
            return ESP_OK;
        }
        
        esp_err_t get_calibration_register() { return read_register_and_decode("CALIBRATION_REG", params_.calibration); };
        esp_err_t get_config_register() { return read_register_and_decode("CONFIG_REG", params_.configuration); };
        esp_err_t get_alert_mask_register() { return read_register_and_decode("ALERT_MASK_REG", params_.alert_mask); };
        esp_err_t get_alert_limit_register() { return read_register_and_decode("ALERT_LIMIT_REG", params_.alert_limit); };

        esp_err_t set_calibration_register() { return write_register_from_encode("CALIBRATION_REG", params_.calibration); };
        esp_err_t set_config_register() { return write_register_from_encode("CONFIG_REG", params_.configuration); };
        esp_err_t set_alert_mask_register() { return write_register_from_encode("ALERT_MASK_REG", params_.alert_mask); };
        esp_err_t set_alert_limit_register() { return write_register_from_encode("ALERT_LIMIT_REG", params_.alert_limit); };

        esp_err_t get_registers();
        esp_err_t set_registers();

        ConfigParams& datas() { return params_; };
        const ConfigParams& datas() const { return params_; };

    private:
        ConfigParams params_;
        inline static const char *TAG = "INA226-CONFIG";
    };

} // namespace ina226
