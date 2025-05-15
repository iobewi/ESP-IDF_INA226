#include "status/ina226-status_types.hpp"
#include <sstream>

namespace ina226
{
    static const char *TAG = "INA226-STATUS";

    void StatusRegister::decode(uint16_t reg)
    {
        raw_value = reg;
        shunt_over_limit = reg & (1 << 15);
        shunt_under_limit = reg & (1 << 14);
        bus_over_limit = reg & (1 << 13);
        bus_under_limit = reg & (1 << 12);
        power_over_limit = reg & (1 << 11);
        conversion_ready = reg & (1 << 10);
        alert_flag = reg & (1 << 9);
    }

    void StatusRegister::log() const
    {
        ESP_LOGI(TAG, "Reg[0x06]=0x%04X CNVR=%d AFF=%d POL=%d", raw_value, conversion_ready, alert_flag, power_over_limit);
        ESP_LOGI(TAG, "BOL=%d BUL=%d SOL=%d SUL=%d",
                 bus_over_limit, bus_under_limit, shunt_over_limit, shunt_under_limit);
    }

    std::string StatusRegister::to_json() const
    {
        return std::string("{") +
               "\"shunt_over_limit\": " + (shunt_over_limit ? "true" : "false") + "," +
               "\"shunt_under_limit\": " + (shunt_under_limit ? "true" : "false") + "," +
               "\"bus_over_limit\": " + (bus_over_limit ? "true" : "false") + "," +
               "\"bus_under_limit\": " + (bus_under_limit ? "true" : "false") + "," +
               "\"power_over_limit\": " + (power_over_limit ? "true" : "false") + "," +
               "\"conversion_ready\": " + (conversion_ready ? "true" : "false") + "," +
               "\"alert_flag\": " + (alert_flag ? "true" : "false") +
               "}";
    }
}
