#include "config/config_macro.hpp"
#include "ina226.hpp"
#include "sdkconfig.h"

#define RETURN_IF_ERROR(x)         \
    do {                           \
        esp_err_t __err_rc = (x);  \
        if (__err_rc != ESP_OK) {  \
            return __err_rc;       \
        }                          \
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

    INA226Manager::INA226Manager(I2CDevices &i2c)
        : i2c_(i2c),
          cfg_(i2c, load_config_from_kconfig()),
          alert_gpio_(gpio_num_t(CONFIG_INA226_INT_ALERT)),
          status_(i2c_)
          {}

    // === API PUBLIQUE ===

    void INA226Manager::init()
    {
        xTaskCreatePinnedToCore(task_wrapper, "STUSB_Task", 4096, this, 5, &task_handle_, 1);
    }

    esp_err_t INA226Manager::init_device()
    {
        RETURN_IF_ERROR(reset());
        // Pause de stabilisation (empirique mais nécessaire)
        vTaskDelay(pdMS_TO_TICKS(2));
        RETURN_IF_ERROR(apply_config());
        RETURN_IF_ERROR(get_status(OutputFormat::Log));
        return ESP_OK;
    }

    /// Envoie un soft reset au INA226
    esp_err_t INA226Manager::reset()
    {
        CTRL ctrl(i2c_);
        return ctrl.send_reset();
    }

    esp_err_t INA226Manager::apply_config()
    {
        return cfg_.set_registers();
    }

    esp_err_t INA226Manager::handle_alert()
    {
        ESP_LOGW(TAG, "ALERT triggered!");
        CTRL ctrl(i2c_);
        RETURN_IF_ERROR(ctrl.get());
        ctrl.log();    
        // Optionnel : tu peux ici faire un traitement plus ciblé si tu sais quelle alerte est active
        // Ex : si MASK_ENABLE (0x06) a le bit "Shunt Over-Voltage", alors tu vérifies ça ici
    
        return ESP_OK;
    }

    esp_err_t INA226Manager::get_status(OutputFormat format)
    {
        RETURN_IF_ERROR(status_.get());
        HANDLE_OUTPUT(format, status_);
        return ESP_OK;
    }

    esp_err_t INA226Manager::get_measurements(OutputFormat format)
    {
        CTRL ctrl(i2c_);
        RETURN_IF_ERROR(ctrl.get());
        HANDLE_OUTPUT(format, ctrl);
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
                handle_alert();
            }
        }
    }
};