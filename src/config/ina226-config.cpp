#include "config/ina226-config.hpp"
#include "ina226-common_types.hpp"
#include <cmath>
#include <string>

#include <esp_log.h>

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
    Config::Config(I2CDevices &dev, const ConfigParams &params)
        : INTERFACE(dev),
          params_(params)
    {}

    esp_err_t Config::get_config(){
        uint16_t config = 0;
        RETURN_IF_ERROR(read_u16(params_.configuration.reg_addr,config));
        params_.configuration.set_raw(config);
        return ESP_OK;

    }

    esp_err_t Config::set_config(){
        uint16_t config = params_.configuration.get_raw();
        RETURN_IF_ERROR(write_u16(params_.configuration.reg_addr, config));
        return ESP_OK;
    }

    esp_err_t Config::get_calibration(){
        uint16_t config = 0;
        RETURN_IF_ERROR(read_u16(params_.calibration.reg_addr,config));
        params_.calibration.set_raw(config);
        return ESP_OK;
    }

    esp_err_t Config::get_alert_mask(){
        uint16_t config = 0;
        RETURN_IF_ERROR(read_u16(params_.alert_mask.reg_addr,config));
        params_.alert_mask.set_raw(config);
        return ESP_OK;
    }

    esp_err_t Config::get_alert_limit() {
        uint16_t config = 0;
        RETURN_IF_ERROR(read_u16(params_.alert_limit.reg_addr,config));
        params_.alert_limit.set_raw(config);
        return ESP_OK;
    }

    esp_err_t Config::set_calibration(){
        uint16_t config = params_.calibration.get_raw();
        RETURN_IF_ERROR(write_u16(params_.calibration.reg_addr, config));
        return ESP_OK;
    }

    esp_err_t Config::set_alert_mask(){
        uint16_t config = params_.alert_mask.get_raw();
        RETURN_IF_ERROR(write_u16(params_.alert_mask.reg_addr, config));
        return ESP_OK;
    }
 
    esp_err_t Config::set_alert_limit(){
        uint16_t config = params_.alert_limit.get_raw();
        RETURN_IF_ERROR(write_u16(params_.alert_limit.reg_addr, config));
        return ESP_OK;
    }

    esp_err_t Config::get(){
        RETURN_IF_ERROR(get_config());
        RETURN_IF_ERROR(get_calibration());
        RETURN_IF_ERROR(get_alert_mask());
        RETURN_IF_ERROR(get_alert_limit());
        return ESP_OK;
    }
    esp_err_t Config::set(){
        RETURN_IF_ERROR(set_config());
        RETURN_IF_ERROR(set_calibration());
        RETURN_IF_ERROR(set_alert_mask());
        RETURN_IF_ERROR(set_alert_limit());
        return ESP_OK;
    }
}
