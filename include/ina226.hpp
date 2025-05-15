#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"

#include "ctrl/ina226-ctrl.hpp"
#include "config/ina226-config.hpp"
#include "status/ina226-status.hpp"

namespace ina226
{
    enum class OutputFormat
    {
        None,
        Log,
        JSON
    };

    class INA226Manager
    {
    public:
        INA226Manager(I2CDevices &i2c);

        // === API PUBLIQUE ===

        /// Initialise la task
        void init();

        /// Initialise la configuration (registre + alertes)
        esp_err_t init_device();

        /// Envoie un soft reset au capteur INA226
        esp_err_t reset();

        /// Écrit la configuration depuis les paramètres (Kconfig ou runtime)
        esp_err_t apply_config(Config &cfg);

        /// Gère une alerte si déclenchée par le GPIO
        esp_err_t handle_alert();

        /// Récupère les mesures courantes
        esp_err_t get_measurements(OutputFormat format = OutputFormat::None);

        /// Optionnel : affichage état alertes/config
        esp_err_t get_status(OutputFormat format = OutputFormat::None);


    private:
        I2CDevices &i2c_;
        Config cfg_;
        gpio_num_t alert_gpio_;
        STATUS status_;
        CTRL ctrl_;

        inline static const char *TAG = "INA226_MANAGER";
        bool ready_ = false;
        esp_err_t is_ready();

        TaskHandle_t task_handle_ = nullptr;

        static void task_wrapper(void *arg);
        static void IRAM_ATTR gpio_isr_handler(void *arg);
        void setup_interrupt(gpio_num_t gpio);
        void task_main();
    };

} // namespace ina226
