#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include "OneWireSensor.h"
#include "CRC.h"

#define ONEWIRE_PIN     1

static void delay_func(uint16_t us)
{
        delayMicroseconds(us);
}

static void set_pin(uint16_t state)
{
        pinMode(ONEWIRE_PIN, OUTPUT);
        digitalWrite(ONEWIRE_PIN, state ? HIGH : LOW);
}

static uint16_t get_pin()
{
        pinMode(ONEWIRE_PIN, INPUT);
        return digitalRead(ONEWIRE_PIN);
}

static uint8_t crc8(uint8_t *buf, uint8_t size)
{
        return Crc8Dallas(buf, size);
}

int main()
{
        OneWireError err;

        wiringPiSetup();

        printf("OneWire example\n");

        /* Create one wire interface */
        OneWire ow = OneWire(delay_func, set_pin, get_pin);
        ow.high_pulse_wait_us = 50;
        ow.low_pulse_wait_us = 5;

        /* Create one sensor and attach it to interface */
        OneWireSensor sensor = OneWireSensor(&ow, RESOLUTION_12_BIT, crc8);

        /* Start converting data */
        err = sensor.start_conversion_skip_rom();

        printf("Start conv errcode: %s\n", sensor.error_desc(err));

        sleep(1);

        /* Read result from sensor */
        err = sensor.get_result_skip_rom();

        printf("Getting conv result: %s\n", sensor.error_desc(err));

        printf("Temperature: %.2f °C\n", sensor.temperature);

        /* Create list of sensors referenced to existing one and connect to interface */
        OneWireSensors sensors = OneWireSensors(&ow, &sensor, 10);

        /* Search for devices over one wire bus */
        err = sensors.search_sensors();

        printf("Search sensors passed with error code: %u, found sensros: %u\n",
                err, sensors.get_devices_found());

        for (uint8_t i = 0; i != sensors.get_devices_found(); i++)
        {
                OneWireSensor *s = sensors.get_sensor(i);
                printf("%2.2u: SN = %X%X%X%X%X%X%X%X\n", i + 1,
                        s->sn[0], s->sn[1], s->sn[2], s->sn[3],
                        s->sn[4], s->sn[5], s->sn[6], s->sn[7]);
                s->resolution = RESOLUTION_12_BIT;
                s->init();
                s->start_conversion();
                delayMicroseconds(s->get_conv_time_ms() * 1000);
                s->get_result();
                printf("Temperature: %.2f °C, resolution: %u\n", s->temperature, s->resolution);
        }

        return 0;
}
