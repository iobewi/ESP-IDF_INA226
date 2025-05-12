#include "config/ina226-config_types.hpp"
#include "ina226-common_types.hpp"

#include "esp_log.h"

namespace ina226
{
    static const char *TAG = "INA226-CONFIG";

    void ConfigurationRegister::decode(uint16_t raw)
    {
        averaging = static_cast<AveragingMode>((raw >> 9) & 0x07);
        bus_conv_time = static_cast<ConversionTime>((raw >> 6) & 0x07);
        shunt_conv_time = static_cast<ConversionTime>((raw >> 3) & 0x07);
        mode = static_cast<OperatingMode>(raw & 0x07);
        
    }

    uint16_t ConfigurationRegister::encode() const {
        uint16_t reg = 0;
        reg |= (static_cast<uint16_t>(averaging) << 9);
        reg |= (static_cast<uint16_t>(bus_conv_time) << 6);
        reg |= (static_cast<uint16_t>(shunt_conv_time) << 3);
        reg |= static_cast<uint16_t>(mode);
        return reg;
    }
    

    void ConfigurationRegister::log() const
    {
        ESP_LOGI(TAG, "Raw value        : 0x%04X", encode());
        ESP_LOGI(TAG, "Averaging        : %s", to_string(averaging));
        ESP_LOGI(TAG, "Bus Conv Time    : %u", bus_conv_time);
        ESP_LOGI(TAG, "Shunt Conv Time  : %u", shunt_conv_time);
        ESP_LOGI(TAG, "Operating Mode   : %u", mode);
    }

    std::string ConfigurationRegister::to_json() const
    {
        return std::string("{") +
               "\"value\": " + std::to_string(encode()) + "," +
               "\"averaging\": " + to_string(averaging) + "," +
               "\"bus_conv_time\": " + to_string(bus_conv_time) + "," +
               "\"shunt_conv_time\": " + to_string(shunt_conv_time) + "," +
               "\"mode\": " + to_string(mode) +
               "}";
    }

    void CalibrationRegister::decode(uint16_t raw)
    {
        value = raw;
    }

    uint16_t CalibrationRegister::encode() const
    {
        return value;
    }

    void CalibrationRegister::log() const
    {
        ESP_LOGI(TAG, "Calibration Register (0x05): 0x%04X", value);
    }

    std::string CalibrationRegister::to_json() const
    {
        return std::string("{") +
               "\"value\": " + std::to_string(value) +
               "}";
    }

    void MaskEnableRegister::decode(uint16_t raw)
    {
        value = raw;
        SOL = raw & (1 << 15);
        SUL = raw & (1 << 14);
        BOL = raw & (1 << 13);
        BUL = raw & (1 << 12);
        POL = raw & (1 << 11);
        CNVR = raw & (1 << 10);
        AFF = raw & (1 << 4);
        CVRF = raw & (1 << 3);
        OVF = raw & (1 << 2);
        APOL = raw & (1 << 1);
        LEN = raw & (1 << 0);
    }

    uint16_t MaskEnableRegister::encode() const
    {
        uint16_t raw = 0;
        raw |= SOL ? (1 << 15) : 0;
        raw |= SUL ? (1 << 14) : 0;
        raw |= BOL ? (1 << 13) : 0;
        raw |= BUL ? (1 << 12) : 0;
        raw |= POL ? (1 << 11) : 0;
        raw |= CNVR ? (1 << 10) : 0;
        raw |= APOL ? (1 << 1) : 0;
        raw |= LEN ? (1 << 0) : 0;
        return raw;
    }

    void MaskEnableRegister::log() const
    {
        ESP_LOGI(TAG, "Register (0x06) = 0x%04X", value);
        ESP_LOGI(TAG, "SOL  (Shunt OV)     : %s", SOL ? "true" : "false");
        ESP_LOGI(TAG, "SUL  (Shunt UV)     : %s", SUL ? "true" : "false");
        ESP_LOGI(TAG, "BOL  (Bus OV)       : %s", BOL ? "true" : "false");
        ESP_LOGI(TAG, "BUL  (Bus UV)       : %s", BUL ? "true" : "false");
        ESP_LOGI(TAG, "POL  (Power OL)     : %s", POL ? "true" : "false");
        ESP_LOGI(TAG, "CNVR (Conv Ready En): %s", CNVR ? "true" : "false");
        ESP_LOGI(TAG, "AFF  (Alert Flag)   : %s", AFF ? "true" : "false");
        ESP_LOGI(TAG, "CVRF (Conv Ready F) : %s", CVRF ? "true" : "false");
        ESP_LOGI(TAG, "OVF  (Overflow)     : %s", OVF ? "true" : "false");
        ESP_LOGI(TAG, "APOL (Alert Polarity): %s", APOL ? "true" : "false");
        ESP_LOGI(TAG, "LEN  (Latch Enable) : %s", LEN ? "true" : "false");
    }

    std::string MaskEnableRegister::to_json() const
    {
        return std::string("{") +
               "\"value\": " + std::to_string(value) + "," +
               "\"SOL\": " + (SOL ? "true" : "false") + "," +
               "\"SUL\": " + (SUL ? "true" : "false") + "," +
               "\"BOL\": " + (BOL ? "true" : "false") + "," +
               "\"BUL\": " + (BUL ? "true" : "false") + "," +
               "\"POL\": " + (POL ? "true" : "false") + "," +
               "\"CNVR\": " + (CNVR ? "true" : "false") + "," +
               "\"AFF\": " + (AFF ? "true" : "false") + "," +
               "\"CVRF\": " + (CVRF ? "true" : "false") + "," +
               "\"OVF\": " + (OVF ? "true" : "false") + "," +
               "\"APOL\": " + (APOL ? "true" : "false") + "," +
               "\"LEN\": " + (LEN ? "true" : "false") +
               "}";
    }

    void AlertLimitRegister::decode(uint16_t raw)
    {
        switch (type)
        {
        case AlertType::ShuntVoltage:
            value = (raw * SHUNT_LSB_UV_X10) / 10; // µV
            break;
        case AlertType::BusVoltage:
            value = (raw * BUS_LSB_UV) / 1000; // mV
            break;
        case AlertType::PowerOverLimit:
            value = raw / POWER_FACTOR; // mW
            break;
        }
    }

    uint16_t AlertLimitRegister::encode() const
    {
        switch (type)
        {
        case AlertType::ShuntVoltage:
            return static_cast<uint16_t>((value * 10) / SHUNT_LSB_UV_X10);
        case AlertType::BusVoltage:
            return static_cast<uint16_t>((value * 1000) / BUS_LSB_UV);
        case AlertType::PowerOverLimit:
            return static_cast<uint16_t>(value * POWER_FACTOR);
        }
        return 0;
    }

    void AlertLimitRegister::log() const
    {
        const char *type_str = type == AlertType::ShuntVoltage ? "Shunt (µV)" : type == AlertType::BusVoltage ? "Bus (mV)"
                                                                                                              : "Power (mW)";
        ESP_LOGI(TAG, "Alert Limit Register (0x07): %s = %u (converted), raw = 0x%04X", type_str, value, encode());
    }

    std::string AlertLimitRegister::to_json() const
    {
        std::string type_str = type == AlertType::ShuntVoltage ? "ShuntVoltage" : type == AlertType::BusVoltage ? "BusVoltage"
                                                                                                                : "PowerOverLimit";
        return std::string("{") +
               "\"type\": \"" + type_str + "\"," +
               "\"value\": " + std::to_string(value) + "," +
               "\"raw_register\": " + std::to_string(encode()) +
               "}";
    }


    void ConfigParams::log() const {
        ESP_LOGI(TAG, "Shunt resistance : %u mOhm", shunt_res_milliohm);
        configuration.log();
        calibration.log();
        alert_mask.log();
        alert_limit.log();
    }

    std::string ConfigParams::to_json() const {
        return std::string("{") +
               "\"shunt_res_milliohm\": " + std::to_string(shunt_res_milliohm) + "," +
               "\"configuration\": " + configuration.to_json() + "," +
               "\"calibration\": " + calibration.to_json() + "," +
               "\"alert_mask\": " + alert_mask.to_json() + "," +
               "\"alert_limit\": " + alert_limit.to_json() +
               "}";
    }

}