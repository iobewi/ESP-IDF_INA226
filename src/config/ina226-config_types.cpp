#include "config/ina226-config_types.hpp"
#include "ina226-common_types.hpp"

#include "esp_log.h"

namespace ina226
{
    static const char *TAG = "INA226-CONFIG";

    ConfigurationRegister::ConfigurationReg ConfigurationRegister::get_values() const
    {
        ConfigurationReg values;
        values.averaging = static_cast<AveragingMode>((raw_ >> 9) & 0x07);
        values.bus_conv_time = static_cast<ConversionTime>((raw_ >> 6) & 0x07);
        values.shunt_conv_time = static_cast<ConversionTime>((raw_ >> 3) & 0x07);
        values.mode = static_cast<OperatingMode>(raw_ & 0x07);
        return values;
    }

    void ConfigurationRegister::set_values(ConfigurationRegister::ConfigurationReg values)
    {

        raw_ &= ~(0b111 << 9);  // Clear averaging
        raw_ |= (static_cast<uint16_t>(values.averaging) << 9);
    
        raw_ &= ~(0b111 << 6);  // Clear bus_conv_time
        raw_ |= (static_cast<uint16_t>(values.bus_conv_time) << 6);
    
        raw_ &= ~(0b111 << 3);  // Clear shunt_conv_time
        raw_ |= (static_cast<uint16_t>(values.shunt_conv_time) << 3);
    
        raw_ &= ~0b111;         // Clear mode
        raw_ |= static_cast<uint16_t>(values.mode);
    }

    void ConfigurationRegister::log() const
    {
        ConfigurationReg values = get_values();
        ESP_LOGI(TAG, "Raw value        : 0x%04X", raw_);
        ESP_LOGI(TAG, "Averaging        : %s", to_string(values.averaging));
        ESP_LOGI(TAG, "Bus Conv Time    : %s", to_string(values.bus_conv_time));
        ESP_LOGI(TAG, "Shunt Conv Time  : %s", to_string(values.shunt_conv_time));
        ESP_LOGI(TAG, "Operating Mode   : %s", to_string(values.mode));
    }

    std::string ConfigurationRegister::to_json() const
    {
        ConfigurationReg values = get_values();
        return std::string("{") +
               "\"value\": " + std::to_string(raw_) + "," +
               "\"averaging\": " + to_string(values.averaging) + "," +
               "\"bus_conv_time\": " + to_string(values.bus_conv_time) + "," +
               "\"shunt_conv_time\": " + to_string(values.shunt_conv_time) + "," +
               "\"mode\": " + to_string(values.mode) +
               "}";
    }

    void CalibrationRegister::set_value(CalibrationReg values)
    {

        if (values.shunt_res_milliohm == 0 || values.max_current_ma == 0) return ;
    
        // LSB initial pour couvrir max_current_ma sur 15 bits
        uint32_t current_lsb_uA = (static_cast<uint32_t>(values.max_current_ma) * 1000 + MAX_CAL) / (MAX_CAL + 1);
    
        // On augmente le LSB jusqu’à obtenir un CAL ≤ 32767
        while ((CAL_CONST / (current_lsb_uA * values.shunt_res_milliohm)) > MAX_CAL) {
            current_lsb_uA++;
            if (current_lsb_uA > 100000) return; // protection de boucle
        }
        
        uint64_t denom = static_cast<uint64_t>(current_lsb_uA) * values.shunt_res_milliohm;
        if (denom == 0) return; // Overflow ou mauvaise entrée

        raw_ = static_cast<uint16_t>(CAL_CONST / denom);
    }

    void CalibrationRegister::log() const
    {
        uint16_t value = get_value();
        ESP_LOGI(TAG, "Calibration Register (0x05): 0x%04X", raw_);
        ESP_LOGI(TAG, "Calibration Value: %u", value);
    }

    std::string CalibrationRegister::to_json() const
    {
        uint16_t value = get_value();
        return std::string("{") +
               "\"value\": " + std::to_string(value) +
               "}";
    }

    MaskEnableRegister::MaskEnableReg MaskEnableRegister::get_values() const
    {
        MaskEnableReg values;
        // RW bits
        if (raw_ & (1 << 15))
            values.alert_type = AlertType::ShuntOverVoltage;
        else if (raw_ & (1 << 14))
            values.alert_type = AlertType::ShuntUnderVoltage;
        else if (raw_ & (1 << 13))
            values.alert_type = AlertType::BusOverVoltage;
        else if (raw_ & (1 << 12))
            values.alert_type = AlertType::BusUnderVoltage;
        else if (raw_ & (1 << 11))
            values.alert_type = AlertType::PowerOverLimit;
        else
            values.alert_type = AlertType::None;

        values.conversion_ready = raw_ & (1 << 10);
        values.alert_function_flag = raw_ & (1 << 4);
        values.conversion_ready_flag = raw_ & (1 << 3);
        values.math_overflow_flag = raw_ & (1 << 2);
        values.alert_polarity_bit = raw_ & (1 << 1);
        values.alert_latch_enable = raw_ & (1 << 0);
        return values;
    }

    void MaskEnableRegister::set_values(MaskEnableRegister::MaskEnableReg values)
    {
        // Clear tous les bits d’alerte configurables
        raw_ &= ~((1 << 15) | (1 << 14) | (1 << 13) | (1 << 12) | (1 << 11)); // alert_type bits
        raw_ &= ~(1 << 10); // CNVR
        raw_ &= ~(1 << 1);  // APOL
        raw_ &= ~(1 << 0);  
        switch (values.alert_type)
        {
        case AlertType::ShuntOverVoltage:
            raw_ |= (1 << 15);
            break;
        case AlertType::ShuntUnderVoltage:
            raw_ |= (1 << 14);
            break;
        case AlertType::BusOverVoltage:
            raw_ |= (1 << 13);
            break;
        case AlertType::BusUnderVoltage:
            raw_ |= (1 << 12);
            break;
        case AlertType::PowerOverLimit:
            raw_ |= (1 << 11);
            break;
        case AlertType::None:
            break;
        }
        raw_ |= values.conversion_ready ? (1 << 10) : 0;
        raw_ |= values.alert_polarity_bit ? (1 << 1) : 0;
        raw_ |= values.alert_latch_enable ? (1 << 0) : 0;
    }

    void MaskEnableRegister::log() const
    {
        auto v = get_values();

        ESP_LOGI(TAG, "Register (0x06) = 0x%04X", raw_);

        const char *type_str = nullptr;
        switch (v.alert_type)
        {
        case AlertType::ShuntOverVoltage:
            type_str = "Shunt Over Voltage";
            break;
        case AlertType::ShuntUnderVoltage:
            type_str = "Shunt Under Voltage";
            break;
        case AlertType::BusOverVoltage:
            type_str = "Bus Over Voltage";
            break;
        case AlertType::BusUnderVoltage:
            type_str = "Bus Under Voltage";
            break;
        case AlertType::PowerOverLimit:
            type_str = "Power Over Limit";
            break;
        case AlertType::None:
            type_str = "None";
            break;
        }

        ESP_LOGI(TAG, "Alert Type             : %s", type_str);
        ESP_LOGI(TAG, "CNVR (Conv Ready En)   : %s", v.conversion_ready ? "true" : "false");
        ESP_LOGI(TAG, "AFF  (Alert Flag)      : %s", v.alert_function_flag ? "true" : "false");
        ESP_LOGI(TAG, "CVRF (Conv Ready F)    : %s", v.conversion_ready_flag ? "true" : "false");
        ESP_LOGI(TAG, "OVF  (Overflow)        : %s", v.math_overflow_flag ? "true" : "false");
        ESP_LOGI(TAG, "APOL (Alert Polarity)  : %s", v.alert_polarity_bit ? "true" : "false");
        ESP_LOGI(TAG, "LEN  (Latch Enable)    : %s", v.alert_latch_enable ? "true" : "false");
    }

    std::string MaskEnableRegister::to_json() const
    {
        auto v = get_values();

        const char *type_str = nullptr;
        switch (v.alert_type)
        {
        case AlertType::ShuntOverVoltage:
            type_str = "shunt_over_voltage";
            break;
        case AlertType::ShuntUnderVoltage:
            type_str = "shunt_under_voltage";
            break;
        case AlertType::BusOverVoltage:
            type_str = "bus_over_voltage";
            break;
        case AlertType::BusUnderVoltage:
            type_str = "bus_under_voltage";
            break;
        case AlertType::PowerOverLimit:
            type_str = "power_over_limit";
            break;
        case AlertType::None:
            type_str = "none";
            break;
        }

        return std::string("{") +
               "\"value\": " + std::to_string(raw_) + "," +
               "\"alert_type\": \"" + type_str + "\"," +
               "\"CNVR\": " + (v.conversion_ready ? "true" : "false") + "," +
               "\"AFF\": " + (v.alert_function_flag ? "true" : "false") + "," +
               "\"CVRF\": " + (v.conversion_ready_flag ? "true" : "false") + "," +
               "\"OVF\": " + (v.math_overflow_flag ? "true" : "false") + "," +
               "\"APOL\": " + (v.alert_polarity_bit ? "true" : "false") + "," +
               "\"LEN\": " + (v.alert_latch_enable ? "true" : "false") +
               "}";
    }

    uint32_t AlertLimitRegister::get_value() const
    {
        switch (type_)
        {
        case AlertType::ShuntOverVoltage:
        case AlertType::ShuntUnderVoltage:
            return (raw_ * SHUNT_LSB_UV_X10) / 10; // µV
        case AlertType::BusOverVoltage:
        case AlertType::BusUnderVoltage:
            return (raw_ * BUS_LSB_UV) / 1000; // mV
        case AlertType::PowerOverLimit:
            return raw_ / POWER_FACTOR; // mW
        case AlertType::None:
            return 0;
        }
        return 0;
    }

    void AlertLimitRegister::set_value(uint32_t value)
    {
        if (SHUNT_LSB_UV_X10 == 0 || BUS_LSB_UV == 0 || POWER_FACTOR == 0) {
            raw_ = 0;
            return;
        }
        
        switch (type_)
        {
        case AlertType::ShuntOverVoltage:
        case AlertType::ShuntUnderVoltage:
            raw_ = (value * 10) / SHUNT_LSB_UV_X10;
            break;
        case AlertType::BusOverVoltage:
        case AlertType::BusUnderVoltage:
            raw_ = (value * 1000) / BUS_LSB_UV;
            break;
        case AlertType::PowerOverLimit:
            raw_ = (value * POWER_FACTOR);
            break;
        case AlertType::None:
            raw_ = 0;
            break;
        }
    }

    void AlertLimitRegister::log() const
    {
        const char *type_str = nullptr;
        switch (type_)
        {
            case AlertType::ShuntOverVoltage:
            case AlertType::ShuntUnderVoltage:
                type_str = "Shunt (µV)";
                break;
            case AlertType::BusOverVoltage:
            case AlertType::BusUnderVoltage:
                type_str = "Bus (mV)";
                break;
            case AlertType::PowerOverLimit:
                type_str = "Power (mW)";
                break;
            case AlertType::None:
                type_str = "None";
                break;
        }
        ESP_LOGI(TAG, "Alert Limit Register (0x07): %s = %u (converted), raw = 0x%04X", type_str, get_value(), raw_);
    }

    std::string AlertLimitRegister::to_json() const
    {
        const char *type_str = nullptr;
        switch (type_)
        {
            case AlertType::ShuntOverVoltage:   type_str = "ShuntOverVoltage"; break;
            case AlertType::ShuntUnderVoltage:  type_str = "ShuntUnderVoltage"; break;
            case AlertType::BusOverVoltage:     type_str = "BusOverVoltage"; break;
            case AlertType::BusUnderVoltage:    type_str = "BusUnderVoltage"; break;
            case AlertType::PowerOverLimit:     type_str = "PowerOverLimit"; break;
            case AlertType::None:               type_str = "None"; break;
        }
        return std::string("{") +
               "\"type\": \"" + type_str + "\"," +
               "\"value\": " + std::to_string(get_value()) + "," +
               "\"raw_register\": " + std::to_string(raw_) +
               "}";
    }

    void ConfigParams::log() const
    {
        configuration.log();
        calibration.log();
        alert_mask.log();
        alert_limit.log();
    }

    std::string ConfigParams::to_json() const
    {
        return std::string("{") +
               "\"configuration\": " + configuration.to_json() + "," +
               "\"calibration\": " + calibration.to_json() + "," +
               "\"alert_mask\": " + alert_mask.to_json() + "," +
               "\"alert_limit\": " + alert_limit.to_json() +
               "}";
    }

}