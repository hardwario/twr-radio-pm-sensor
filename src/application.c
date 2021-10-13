#include <application.h>
#include <twr_sps30.h>

#define UPDATE_INTERVAL_MINUTES 10
#define STARTUP_SECONDS 30

// LED instance
twr_led_t led;
// Button instance
twr_button_t button;
// PM sensor instance
twr_sps30_t sps30;

void sps30_event_handler(twr_sps30_t *self, twr_sps30_event_t event, void *event_param)
{
    if (event == TWR_SPS30_EVENT_UPDATE)
    {
        twr_sps30_mass_concentration_t mass_concentration;

        if (twr_sps30_get_mass_concentration(&sps30, &mass_concentration))
        {
            twr_radio_pub_float("pm-sensor/mass-concentration/pm1.0", &mass_concentration.mc_1p0);
            twr_radio_pub_float("pm-sensor/mass-concentration/pm2.5", &mass_concentration.mc_2p5);
            twr_radio_pub_float("pm-sensor/mass-concentration/pm4.0", &mass_concentration.mc_4p0);
            twr_radio_pub_float("pm-sensor/mass-concentration/pm10", &mass_concentration.mc_10p0);
        }

        twr_sps30_number_concentration_t number_concentration;

        if (twr_sps30_get_number_concentration(&sps30, &number_concentration))
        {
            twr_radio_pub_float("pm-sensor/number-concentration/pm0.5", &number_concentration.nc_0p5);
            twr_radio_pub_float("pm-sensor/number-concentration/pm1.0", &number_concentration.nc_1p0);
            twr_radio_pub_float("pm-sensor/number-concentration/pm2.5", &number_concentration.nc_2p5);
            twr_radio_pub_float("pm-sensor/number-concentration/pm4.0", &number_concentration.nc_4p0);
            twr_radio_pub_float("pm-sensor/number-concentration/pm10", &number_concentration.nc_10p0);
        }

        float typical_particle_size;

        if (twr_sps30_get_typical_particle_size(&sps30, &typical_particle_size))
        {
            twr_radio_pub_float("pm-sensor/typical-particle-size", &typical_particle_size);
        }
    }
    else if (event == TWR_SPS30_EVENT_ERROR)
    {
        twr_led_set_mode(&led, TWR_LED_MODE_BLINK);
    }
}

void button_event_handler(twr_button_t *self, twr_button_event_t event, void *event_param)
{
    if (event == TWR_BUTTON_EVENT_PRESS)
    {
        twr_led_pulse(&led, 100);

        twr_sps30_measure(&sps30);
    }
}

void application_init(void)
{
    // Initialize LED
    twr_led_init(&led, TWR_GPIO_LED, false, false);
    twr_led_set_mode(&led, TWR_LED_MODE_OFF);

    // Initialize radio
    twr_radio_init(TWR_RADIO_MODE_NODE_SLEEPING);

    // Initialize button
    twr_button_init(&button, TWR_GPIO_BUTTON, TWR_GPIO_PULL_DOWN, false);
    twr_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize particulate matter sensor
    twr_sps30_init(&sps30, TWR_I2C_I2C1, 0x69);
    twr_sps30_set_event_handler(&sps30, sps30_event_handler, NULL);
    twr_sps30_set_startup_time(&sps30, STARTUP_SECONDS * 1000);
    twr_sps30_set_update_interval(&sps30, UPDATE_INTERVAL_MINUTES * 60 * 1000);

    twr_radio_pairing_request("pm-sensor", VERSION);

    twr_led_pulse(&led, 2000);
}
