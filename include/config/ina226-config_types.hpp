#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace ina226
{
    struct ConfigurationRegister
    {
        // Valeurs possibles d'averaging (bits 9:11)
        enum class AveragingMode : uint8_t
        {
            AVG_1 = 0b000,
            AVG_4 = 0b001,
            AVG_16 = 0b010,
            AVG_64 = 0b011,
            AVG_128 = 0b100,
            AVG_256 = 0b101,
            AVG_512 = 0b110,
            AVG_1024 = 0b111
        };

        static std::string to_string(AveragingMode mode)
        {
            using AM = ConfigurationRegister::AveragingMode;
            switch (mode)
            {
            case AM::AVG_1:
                return "1 sample";
            case AM::AVG_4:
                return "4 samples";
            case AM::AVG_16:
                return "16 samples";
            case AM::AVG_64:
                return "64 samples";
            case AM::AVG_128:
                return "128 samples";
            case AM::AVG_256:
                return "256 samples";
            case AM::AVG_512:
                return "512 samples";
            case AM::AVG_1024:
                return "1024 samples";
            default:
                return "Unknown";
            }
        };

        // Temps de conversion pour Bus ou Shunt (bits 3:5 ou 6:8)
        enum class ConversionTime : uint8_t
        {
            CT_140us = 0b000,
            CT_204us = 0b001,
            CT_332us = 0b010,
            CT_588us = 0b011,
            CT_1100us = 0b100,
            CT_2116us = 0b101,
            CT_4156us = 0b110,
            CT_8244us = 0b111
        };

        static std::string to_string(ConversionTime time)
        {
            using CT = ConfigurationRegister::ConversionTime;
            switch (time)
            {
            case CT::CT_140us:
                return "140 us";
            case CT::CT_204us:
                return "204 us";
            case CT::CT_332us:
                return "332 us";
            case CT::CT_588us:
                return "588 us";
            case CT::CT_1100us:
                return "1100 us";
            case CT::CT_2116us:
                return "2116 us";
            case CT::CT_4156us:
                return "4156 us";
            case CT::CT_8244us:
                return "8244 us";
            default:
                return "Unknown";
            }
        }

        enum class OperatingMode : uint8_t
        {
            PowerDown = 0b000,
            ShuntTriggered = 0b001,
            BusTriggered = 0b010,
            ShuntAndBusTriggered = 0b011,
            ShuntContinuous = 0b101,
            BusContinuous = 0b110,
            ShuntAndBusContinuous = 0b111
        };

        static std::string to_string(OperatingMode mode)
        {
            using OM = ConfigurationRegister::OperatingMode;
            switch (mode)
            {
            case OM::PowerDown:
                return "Power-Down";
            case OM::ShuntTriggered:
                return "Shunt Triggered";
            case OM::BusTriggered:
                return "Bus Triggered";
            case OM::ShuntAndBusTriggered:
                return "Shunt + Bus Triggered";
            case OM::ShuntContinuous:
                return "Shunt Continuous";
            case OM::BusContinuous:
                return "Bus Continuous";
            case OM::ShuntAndBusContinuous:
                return "Shunt + Bus Continuous";
            default:
                return "Unknown";
            }
        }

        static constexpr uint8_t reg_addr = 0x00;

        // Champs extraits du registre
        AveragingMode averaging = AveragingMode::AVG_1;             // Bits 9:11
        ConversionTime bus_conv_time = ConversionTime::CT_1100us;   // Bits 6:8
        ConversionTime shunt_conv_time = ConversionTime::CT_1100us; // Bits 3:5
        OperatingMode mode = OperatingMode::ShuntAndBusContinuous;  // Bits 0:2

        void decode(uint16_t raw);
        uint16_t encode() const;

        void log() const;
        std::string to_json() const;
    };

    struct CalibrationRegister
    {
        static constexpr uint8_t reg_addr = 0x05;
        uint16_t value = 0;

        void decode(uint16_t raw);
        uint16_t encode() const;

        void log() const;
        std::string to_json() const;
    };

    struct MaskEnableRegister
    {
        static constexpr uint8_t reg_addr = 0x06;
        uint16_t value = 0;

        // RW bits
        bool SOL = false;
        bool SUL = false;
        bool BOL = false;
        bool BUL = false;
        bool POL = false;
        bool CNVR = false;
        bool APOL = false;
        bool LEN = false;

        // R bits
        bool AFF = false;
        bool CVRF = false;
        bool OVF = false;

        void decode(uint16_t raw);
        uint16_t encode() const;

        void log() const;
        std::string to_json() const;
    };

    struct AlertLimitRegister
    {
        static constexpr uint8_t reg_addr = 0x07;

        enum class AlertType : uint8_t
        {
            ShuntVoltage,  // µV
            BusVoltage,    // mV
            PowerOverLimit // mW
        };

        uint16_t value = 0; // valeur convertie dans l’unité associée
        AlertType type = AlertType::ShuntVoltage;

        void decode(uint16_t raw);
        uint16_t encode() const;

        void log() const;
        std::string to_json() const;
    };

    struct ConfigParams
    {
        uint16_t shunt_res_milliohm = 100;
        ConfigurationRegister configuration = {};
        CalibrationRegister calibration = {};
        MaskEnableRegister alert_mask = {};
        AlertLimitRegister alert_limit {};

        void log() const;
        std::string to_json() const;
    };

};