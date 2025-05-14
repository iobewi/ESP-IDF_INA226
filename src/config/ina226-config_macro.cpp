#include "config/ina226-config_macro.hpp"
#include "sdkconfig.h"
#include "config/ina226-config.hpp"

namespace ina226
{
        ConfigParams load_config_from_kconfig()
        {
                ConfigParams params;
                ConfigurationRegister::ConfigurationReg configreg;
                CalibrationRegister::CalibrationReg calreg;
                

                // Résistance shunt
                calreg.shunt_res_milliohm = CONFIG_INA226_SHUNT_RESISTANCE_MILLIOHM;

                // Averaging
#if CONFIG_INA226_AVG_4
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_4;
#elif CONFIG_INA226_AVG_16
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_16;
#elif CONFIG_INA226_AVG_64
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_64;
#elif CONFIG_INA226_AVG_128
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_128;
#elif CONFIG_INA226_AVG_256
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_256;
#elif CONFIG_INA226_AVG_512
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_512;
#elif CONFIG_INA226_AVG_1024
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_1024;
#else
                configreg.averaging = ConfigurationRegister::AveragingMode::AVG_1;
#endif

                // Bus conversion time
#if CONFIG_INA226_BUS_CT_204US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_204us;
#elif CONFIG_INA226_BUS_CT_332US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_332us;
#elif CONFIG_INA226_BUS_CT_588US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_588us;
#elif CONFIG_INA226_BUS_CT_2116US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_2116us;
#elif CONFIG_INA226_BUS_CT_4156US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_4156us;
#elif CONFIG_INA226_BUS_CT_8244US
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_8244us;
#else
                configreg.bus_conv_time = ConfigurationRegister::ConversionTime::CT_1100us;
#endif

                // Shunt conversion time
#if CONFIG_INA226_SHUNT_CT_204US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_204us;
#elif CONFIG_INA226_SHUNT_CT_332US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_332us;
#elif CONFIG_INA226_SHUNT_CT_588US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_588us;
#elif CONFIG_INA226_SHUNT_CT_2116US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_2116us;
#elif CONFIG_INA226_SHUNT_CT_4156US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_4156us;
#elif CONFIG_INA226_SHUNT_CT_8244US
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_8244us;
#else
                configreg.shunt_conv_time = ConfigurationRegister::ConversionTime::CT_1100us;
#endif

                params.configuration.set_values(configreg);
                params.calibration.set_value(calreg);
                // Alert Mask
                params.alert_mask.set_raw(CONFIG_INA226_ALERT_MASK);

                return params;
        }
}
