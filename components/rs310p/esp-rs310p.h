/**
 * esp-rs310.h
 *
 * Header file for esphome-rs310p
 *
 * Author: Scott McGimpsey @mister-scott
 * Last Updated: 2022-11-18
 * License: BSD
 *
 * Requirements:
 * - https://github.com/mister-scott/esp-rs310p
 * - ESPHome 1.18.0 or greater
 */
#define USE_CALLBACKS

#include "esphome.h"
#include "esphome/core/preferences.h"

using namespace esphome;

#ifndef ESPRS310P_H
#define ESPRS310P_H

static const char* TAG = "RS310P"; // Logging tag

static const char* ESPRS310P_VERSION = "1.0.0";

/* If polling interval is greater than 9 seconds, the HeatPump
library reconnects, but doesn't then follow up with our data request.*/
static const uint32_t ESPRS310P_POLL_INTERVAL_DEFAULT = 500; // in milliseconds,
                                                           // 0 < X <= 9000
static const uint8_t ESPRS310P_MIN_VOLTAGE = 0;   // Volts V,
                                                  // defined by hardware
static const uint8_t ESPRS310P_MAX_VOLTAGE = 30;  // Volts V,
                                                  //defined by hardware
static const uint8_t ESPRS310P_MIN_CURRENT = 0;  // Amps A,
                                                  //defined by hardware
static const uint8_t ESPRS310P_MAX_CURRENT = 10;  // Amps A,
                                                  //defined by hardware
// static const float   ESPRS310P_TEMPERATURE_STEP = 0.5; // temperature setting step,
//                                                     // in degrees C

class RS310P : public PollingComponent, public output::Output {

    public:

        /**
         * Create a new MitsubishiHeatPump object
         *
         * Args:
         *   hw_serial: pointer to an Arduino HardwareSerial instance
         *   poll_interval: polling interval in milliseconds
         */
        MitsubishiHeatPump(
            HardwareSerial* hw_serial,
            uint32_t poll_interval=ESPRS310P_POLL_INTERVAL_DEFAULT
        );

        // Print a banner with library information.
        void banner() {
            ESP_LOGI(TAG, "ESPHome MitsubishiHeatPump version %s",
                    ESPRS310P_VERSION);
        }

        // Set the baud rate. Must be called before setup() to have any effect.
        void set_baud_rate(int);

        // print the current configuration
        void dump_config() override;

        // handle a change in settings as detected by the HeatPump library.
        void hpSettingsChanged();

        // Handle a change in status as detected by the HeatPump library.
        void hpStatusChanged(heatpumpStatus currentStatus);

        // Set up the component, initializing the HeatPump object.
        void setup() override;

        // This is called every poll_interval.
        void update() override;

        // Configure the climate object with traits that we support.
        climate::ClimateTraits traits() override;

        // Get a mutable reference to the traits that we support.
        climate::ClimateTraits& config_traits();

        // Debugging function to print the object's state.
        void dump_state();

        // Handle a request from the user to change settings.
        void control(const climate::ClimateCall &call) override;

        // Use the temperature from an external sensor. Use
        // set_remote_temp(0) to switch back to the internal sensor.
        void set_remote_temperature(float);

    protected:
        // HeatPump object using the underlying Arduino library.
        HeatPump* hp;

        // The ClimateTraits supported by this HeatPump.
        climate::ClimateTraits traits_;

        // Allow the HeatPump class to use get_hw_serial_
        friend class HeatPump;

        //Accessor method for the HardwareSerial pointer
        HardwareSerial* get_hw_serial_() {
            return this->hw_serial_;
        }

        //Print a warning message if we're using the sole hardware UART on an
        //ESP8266 or UART0 on ESP32
        void check_logger_conflict_();

        // various prefs to save mode-specific temperatures, akin to how the IR
        // remote works.
        ESPPreferenceObject cool_storage;
        ESPPreferenceObject heat_storage;
        ESPPreferenceObject auto_storage;

        optional<float> cool_setpoint;
        optional<float> heat_setpoint;
        optional<float> auto_setpoint;

        static void save(float value, ESPPreferenceObject& storage);
        static optional<float> load(ESPPreferenceObject& storage);

    private:
        // Retrieve the HardwareSerial pointer from friend and subclasses.
        HardwareSerial *hw_serial_;
        int baud_ = 0;
};

#endif