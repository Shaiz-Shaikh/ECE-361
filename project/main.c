#include <stdio.h>
#include "iom361_r4.h"
#include "sensor.h"

int main(void)
{
    int rc;
    uint32_t *base = iom361_initialize(1, 4, &rc);   /* 1 switch, 4 LEDs */

    if (base == NULL) {
        fprintf(stderr, "iom361_initialize failed: %d\n", rc);
        return 1;
    }

    _iom361_set_sensor1(28.5f, 60.0f);   /* a typical warm backyard day */

    float temp_c;
    float rh_pct;
    rc = sensor_read(base, &temp_c, &rh_pct);
    if (rc != 0) {
        fprintf(stderr, "sensor_read failed: %d\n", rc);
        return 1;
    }

    printf("Weather station\n");
    printf("Temperature: %.1f C\n", temp_c);
    printf("Humidity: %.1f %%RH\n", rh_pct);
    return 0;
}
