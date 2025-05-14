#pragma once

#include <cstdint>
#include <string>

#include "esp_err.h"

#include "ina226-interface.hpp"
#include "config/ina226-config_types.hpp"

namespace ina226
{
    class Config : public INTERFACE
    {
    public:
        Config(I2CDevices &dev, 
            const ConfigParams& params = {});
  
            
        esp_err_t get_config();
        esp_err_t get_calibration();
        esp_err_t get_alert_mask();
        esp_err_t get_alert_limit();
        esp_err_t get();

        esp_err_t set_config();
        esp_err_t set_calibration();
        esp_err_t set_alert_mask();
        esp_err_t set_alert_limit();
        esp_err_t set();


        ConfigParams& datas() { return params_; };
        const ConfigParams& datas() const { return params_; };

    private:
        ConfigParams params_;
        inline static const char *TAG = "INA226-CONFIG";
    };

} // namespace ina226
