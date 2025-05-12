#include "config/config.hpp"
#include "common_types.hpp"
#include <cmath>
#include <string>

#include <esp_log.h>

#define RETURN_IF_ERROR(x)         \
    do {                           \
        esp_err_t __err_rc = (x);  \
        if (__err_rc != ESP_OK) {  \
            return __err_rc;       \
        }                          \
    } while (0)

namespace ina226
{
    static uint16_t compute_calibration_mohm(uint16_t shunt_res_milliohm)
    {
        // Calibration = 5120 / Rshunt (en milliohms)
        if (shunt_res_milliohm == 0)
            return 0;
        return static_cast<uint16_t>(CAL_FACTOR * 1u / CURRENT_LSB_MA * shunt_res_milliohm);
    }

    Config::Config(I2CDevices &dev, const ConfigParams &params)
        : ina226::INTERFACE(dev),
          params_(params)
    {
        params_.calibration.value = compute_calibration_mohm(params_.shunt_res_milliohm);
    }

    esp_err_t Config::get_registers(){
        RETURN_IF_ERROR(get_config_register());
        RETURN_IF_ERROR(get_config_register());
        RETURN_IF_ERROR(get_alert_mask_register());
        RETURN_IF_ERROR(get_alert_limit_register());
        return ESP_OK;
    }
    esp_err_t Config::set_registers(){
        RETURN_IF_ERROR(set_config_register());
        RETURN_IF_ERROR(set_config_register());
        RETURN_IF_ERROR(set_alert_mask_register());
        RETURN_IF_ERROR(set_alert_limit_register());
        return ESP_OK;
    }

}
