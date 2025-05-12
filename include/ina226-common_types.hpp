#pragma once
#include <cstdint>

namespace ina226 {

    // === Constantes de calcul ===

    /// Facteur de calibration : 0.00512 / Current_LSB → utilisé pour compute_calibration()
    static constexpr uint32_t CAL_FACTOR = 5120; // 0.00512 / 0.001 = 5120

    // === Constantes LSB exprimées en unités entières ===

    /// Shunt Voltage Register LSB : 2.5 µV → exprimé en dixièmes de µV pour rester en entier
    static constexpr uint16_t SHUNT_LSB_UV_X10 = 25; // 2.5 µV × 10

    /// Bus Voltage Register LSB : 1.25 mV = 1250 µV
    static constexpr uint16_t BUS_LSB_UV = 1250;

    /// Current Register LSB (hypothèse de Current_LSB = 1 mA)
    static constexpr uint16_t CURRENT_LSB_MA = 1;

    /// Power Register LSB : 25 × Current_LSB = 25 mW
    static constexpr uint16_t POWER_LSB_MW = 25;

    /// Facteur d’échelle entre courant et puissance
    /// (Current_LSB = 1 mA → Power = current × POWER_FACTOR)
    static constexpr uint16_t POWER_FACTOR = 1000 / CURRENT_LSB_MA * POWER_LSB_MW; // = 40

}
