#include "config/ina226-config_macro.hpp"
#include "sdkconfig.h"
#include "config/ina226-config.hpp"

namespace ina226
{
        ConfigParams load_config_from_kconfig()
        {
                ConfigParams params;

                // Résistance shunt
                params.shunt_res_milliohm = CONFIG_INA226_SHUNT_RESISTANCE_MILLIOHM;

                // Averaging
#if CONFIG_INA226_AVG_4
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_4;
#elif CONFIG_INA226_AVG_16
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_16;
#elif CONFIG_INA226_AVG_64
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_64;
#elif CONFIG_INA226_AVG_128
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_128;
#elif CONFIG_INA226_AVG_256
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_256;
#elif CONFIG_INA226_AVG_512
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_512;
#elif CONFIG_INA226_AVG_1024
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_1024;
#else
                params.configuration.averaging = ConfigurationRegister::AveragingMode::AVG_1;
#endif

                // Bus conversion time
#if CONFIG_INA226_BUS_CT_204US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_204us;
#elif CONFIG_INA226_BUS_CT_332US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_332us;
#elif CONFIG_INA226_BUS_CT_588US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_588us;
#elif CONFIG_INA226_BUS_CT_2116US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_2116us;
#elif CONFIG_INA226_BUS_CT_4156US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_4156us;
#elif CONFIG_INA226_BUS_CT_8244US
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_8244us;
#else
                params.configuration.bus_conv_time = ConfigurationRegister::ConversionTime::CT_1100us;
#endif

                // Shunt conversion time
#if CONFIG_INA226_SHUNT_CT_204US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_204us;
#elif CONFIG_INA226_SHUNT_CT_332US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_332us;
#elif CONFIG_INA226_SHUNT_CT_588US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_588us;
#elif CONFIG_INA226_SHUNT_CT_2116US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_2116us;
#elif CONFIG_INA226_SHUNT_CT_4156US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_4156us;
#elif CONFIG_INA226_SHUNT_CT_8244US
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_8244us;
#else
                params.configuration.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_1100us;
#endif

                // Alert Mask
                params.alert_mask.decode(CONFIG_INA226_ALERT_MASK);

                return params;
        }
}
