#include "config/ina226-config_macro.hpp"
#include "ina226.hpp"
#include "sdkconfig.h"

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


#define HANDLE_OUTPUT(format, obj)                            \
    do {                                                      \
        switch (format)                                       \
        {                                                     \
            case OutputFormat::Log:                           \
                obj.log();                                    \
                break;                                        \
            case OutputFormat::JSON:                          \
                printf("%s\n", obj.to_json().c_str());        \
                break;                                        \
            case OutputFormat::None:                          \
            default:                                          \
                break;                                        \
        }                                                     \
    } while (0)

namespace ina226
{

    static const char *TAG = "INA226";

    inline esp_err_t return_if_not_ready(bool ready, const char* tag)
    {
        if (!ready)
        {
            ESP_LOGE(tag, "Tentative d’utilisation sans initialisation préalable");
            return ESP_ERR_INVALID_STATE;
        }
        return ESP_OK;
    }

    INA226Manager::INA226Manager(I2CDevices &i2c)
        : i2c_(i2c),
          cfg_(i2c_),
          alert_gpio_(gpio_num_t(CONFIG_INA226_INT_ALERT)),
          status_(i2c_),
          ctrl_(i2c_)
          {}

    // === API PUBLIQUE ===

    void INA226Manager::init()
    {
        xTaskCreatePinnedToCore(task_wrapper, "STUSB_Task", 4096, this, 5, &task_handle_, 0);
    }

    esp_err_t INA226Manager::init_device()
    {
        RETURN_IF_ERROR(is_ready());
        RETURN_IF_ERROR(reset());
        vTaskDelay(pdMS_TO_TICKS(100));
        RETURN_IF_ERROR(apply_config(cfg_));
        RETURN_IF_ERROR(get_status(OutputFormat::Log));
        cfg_.datas().log();
        return ESP_OK;
    }

    esp_err_t INA226Manager::is_ready()
    {
        const int max_attempts = 10;
        const int delay_ms = 50;
        esp_err_t err = ESP_OK;
        for (int attempt = 0; attempt < max_attempts; ++attempt)
        {
            err = ctrl_.ready();
                if (err == ESP_OK)
                {
                    ESP_LOGI(TAG, "INA226 détecté sur le bus (tentative %d)", attempt + 1);
                    break;
                }
            ESP_LOGW(TAG, "Tentative %d/%d : INA226 non prêt (err=0x%x)", attempt + 1, max_attempts, err);
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
        }
        
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "INA226 non détecté après %d tentatives", max_attempts);
            ready_ = false;
            return ESP_ERR_TIMEOUT; // ou err si tu veux refléter la dernière erreur de ready()
        }
         ready_ = true;
        return ESP_OK;
    }

    esp_err_t INA226Manager::apply_config(Config &cfg)
    {
        RETURN_IF_ERROR(return_if_not_ready(ready_, TAG));
        ConfigParams from_kconfig = load_config_from_kconfig();
        RETURN_IF_ERROR(cfg.get_config());
        cfg.datas().configuration.set_values(from_kconfig.configuration.get_values());
        cfg.datas().calibration.set_raw(from_kconfig.calibration.get_value());
        cfg.datas().alert_mask.set_values(from_kconfig.alert_mask.get_values());
        cfg.datas().alert_limit.set_raw(from_kconfig.alert_limit.get_value());
        return cfg.set();
    }

    /// Envoie un soft reset au INA226
    esp_err_t INA226Manager::reset()
    {
        RETURN_IF_ERROR(return_if_not_ready(ready_, TAG));
        return ctrl_.send_reset();
    }

    esp_err_t INA226Manager::handle_alert()
    {
        RETURN_IF_ERROR(return_if_not_ready(ready_, TAG));
        ESP_LOGW(TAG, "ALERT triggered!");
        RETURN_IF_ERROR(ctrl_.get());
        ctrl_.log();    
        // Optionnel : tu peux ici faire un traitement plus ciblé si tu sais quelle alerte est active
        // Ex : si MASK_ENABLE (0x06) a le bit "Shunt Over-Voltage", alors tu vérifies ça ici
    
        return ESP_OK;
    }

    esp_err_t INA226Manager::get_status(OutputFormat format)
    {
        RETURN_IF_ERROR(return_if_not_ready(ready_, TAG));
        RETURN_IF_ERROR(status_.get());
        HANDLE_OUTPUT(format, status_);
        return ESP_OK;
    }

    esp_err_t INA226Manager::get_measurements(OutputFormat format)
    {
        RETURN_IF_ERROR(return_if_not_ready(ready_, TAG));
        RETURN_IF_ERROR(ctrl_.get());
        HANDLE_OUTPUT(format, ctrl_);
        return ESP_OK;
    }

    void INA226Manager::task_wrapper(void *arg)
    {
        static_cast<INA226Manager *>(arg)->task_main();
    }

    void IRAM_ATTR INA226Manager::gpio_isr_handler(void *arg)
    {
        auto *self = static_cast<INA226Manager *>(arg);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(self->task_handle_, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    void INA226Manager::setup_interrupt(gpio_num_t gpio)
    {
        static bool isr_installed = false;
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << gpio);
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);

        if (!isr_installed)
        {
            esp_err_t err = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
            if (err == ESP_OK || err == ESP_ERR_INVALID_STATE)
            {
                isr_installed = true;
            }
            else
            {
                ESP_LOGE(TAG, "Failed to install ISR service: %s", esp_err_to_name(err));
            }
        }

        gpio_isr_handler_add(gpio, gpio_isr_handler, this);
    }

    void INA226Manager::task_main()
    {
        setup_interrupt(alert_gpio_);
        init_device();

        while (true)
        {
            if (gpio_get_level(alert_gpio_) == 0 || ulTaskNotifyTake(pdTRUE, portMAX_DELAY))
            {
                ESP_LOGE(TAG, "Alert");
                handle_alert();
            }
        }
    }
};