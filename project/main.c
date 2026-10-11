#include <stdio.h>
#include "iom361_r4.h"

int main(void)
{
    int rc;
    uint32_t *base = iom361_initialize(1, 4, &rc);   /* 1 switch, 4 LEDs */

    if (base == NULL) {
        fprintf(stderr, "iom361_initialize failed: %d\n", rc);
        return 1;
    }

    printf("Weather station\n");
    return 0;
}
