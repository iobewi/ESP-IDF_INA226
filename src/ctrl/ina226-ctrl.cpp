#include "ctrl/ina226-ctrl.hpp"
#include "ina226-common_types.hpp"

#include "esp_log.h"

#define RETURN_IF_ERROR(x)                          \
    do {                                             \
        esp_err_t __err_rc = (x);                   \
        if (__err_rc != ESP_OK) {                   \
            ESP_LOGE("RETURN_IF_ERROR",             \
                     "%s failed at %s:%d → %s",     \
                     #x, __FILE__, __LINE__,        \
                     esp_err_to_name(__err_rc));    \
            return __err_rc;                        \
        }                                            \
    } while (0)


namespace ina226
{
    static const char *TAG = "INA226-CTRL";

    esp_err_t CTRL::send_reset()
    {
        uint16_t config;
        RETURN_IF_ERROR(read_u16(REG_CONFIG, config));
        config |= RESET_COMMAND;
        RETURN_IF_ERROR(write_u16(REG_CONFIG, config));
        return ESP_OK;
    }

    esp_err_t CTRL::get_shunt_voltage()
    {
        int16_t val;
        RETURN_IF_ERROR(read_s16(REG_SHUNT_VOLTAGE, val));
        shunt_voltage_uv = ((val * SHUNT_LSB_UV_X10) / 10);
        return ESP_OK;
    }

    esp_err_t CTRL::get_bus_voltage()
    {
        uint16_t val;
        RETURN_IF_ERROR(read_u16(REG_BUS_VOLTAGE, val));
        bus_voltage_mv = (val * BUS_LSB_UV) / 1000;
        return ESP_OK;
    }

    esp_err_t CTRL::get_current()
    {
        int16_t val;
        RETURN_IF_ERROR(read_s16(REG_CURRENT, val));
        current_ma = static_cast<int32_t>(val) * CURRENT_LSB_MA;
        return ESP_OK;
    }

    esp_err_t CTRL::get_power()
    {
        uint16_t val;
        RETURN_IF_ERROR(read_u16(REG_POWER, val));
        power_mw = static_cast<uint32_t>(val) * POWER_LSB_MW;
        return ESP_OK;
    }

    esp_err_t CTRL::get()
    {
        RETURN_IF_ERROR(get_shunt_voltage());
        RETURN_IF_ERROR(get_bus_voltage());
        RETURN_IF_ERROR(get_power());
        RETURN_IF_ERROR(get_current());
        return ESP_OK;
    }

    void CTRL::log() const
    {
        ESP_LOGI(TAG, "Shunt voltage : %d µV", shunt_voltage_uv);
        ESP_LOGI(TAG, "Bus voltage   : %u mV", bus_voltage_mv);
        ESP_LOGI(TAG, "Current       : %d mA", current_ma);
        ESP_LOGI(TAG, "Power         : %u mW", power_mw);
    }

    std::string CTRL::to_json() const
    {
        return std::string("{") +
               "\"shunt_uv\": " + std::to_string(shunt_voltage_uv) + "," +
               "\"bus_mv\": " + std::to_string(bus_voltage_mv) + "," +
               "\"current_ma\": " + std::to_string(current_ma) + "," +
               "\"power_mw\": " + std::to_string(power_mw) +
               "}";
    }

} // namespace ina226
