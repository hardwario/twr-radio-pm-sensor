#include <application.h>
#include <bc_sps30.h>

#define UPDATE_INTERVAL_SECONDS 5

// LED instance
bc_led_t led;
// Button instance
bc_button_t button;
// PM sensor instance
bc_sps30_t sps30;

void sps30_event_handler(bc_sps30_t *self, bc_sps30_event_t event, void *event_param)
{
    if (event == BC_SPS30_EVENT_UPDATE)
    {
        bc_sps30_mass_concentration_t mass_concentration;

        if (bc_sps30_get_mass_concentration(&sps30, &mass_concentration))
        {
            bc_radio_pub_float("pm-sensor/mass-concentration/pm1.0", &mass_concentration.mc_1p0);
            bc_radio_pub_float("pm-sensor/mass-concentration/pm2.5", &mass_concentration.mc_2p5);
            bc_radio_pub_float("pm-sensor/mass-concentration/pm4.0", &mass_concentration.mc_4p0);
            bc_radio_pub_float("pm-sensor/mass-concentration/pm10", &mass_concentration.mc_10p0);
        }

        bc_sps30_number_concentration_t number_concentration;

        if (bc_sps30_get_number_concentration(&sps30, &number_concentration))
        {
            bc_radio_pub_float("pm-sensor/number-concentration/pm0.5", &number_concentration.nc_0p5);
            bc_radio_pub_float("pm-sensor/number-concentration/pm1.0", &number_concentration.nc_1p0);
            bc_radio_pub_float("pm-sensor/number-concentration/pm2.5", &number_concentration.nc_2p5);
            bc_radio_pub_float("pm-sensor/number-concentration/pm4.0", &number_concentration.nc_4p0);
            bc_radio_pub_float("pm-sensor/number-concentration/pm10", &number_concentration.nc_10p0);
        }

        float typical_particle_size;

        if (bc_sps30_get_typical_particle_size(&sps30, &typical_particle_size))
        {
            bc_radio_pub_float("pm-sensor/typical-particle-size", &typical_particle_size);
        }
    }
}

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_pulse(&led, 100);

        bc_sps30_measure(&sps30);
    }
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_OFF);

    // Initialize radio
    bc_radio_init(BC_RADIO_MODE_NODE_SLEEPING);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize particulate matter sensor
    bc_sps30_init(&sps30, BC_I2C_I2C0, 0x69);
    bc_sps30_set_event_handler(&sps30, sps30_event_handler, NULL);
    bc_sps30_set_update_interval(&sps30, UPDATE_INTERVAL_SECONDS * 1000);

    bc_radio_pairing_request("pm-sensor", VERSION);

    bc_led_pulse(&led, 2000);
}
