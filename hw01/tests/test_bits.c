#include <stdio.h>
#include <stdint.h>
#include "../bits.h"
#include "../status.h"

static int passed = 0;
static int failed = 0;

#define CHECK(name, actual, expected)                                  \
    do {                                                               \
        if ((actual) == (expected)) {                                  \
            printf("PASS: %s\n", name);                                \
            passed++;                                                  \
        } else {                                                       \
            printf("FAIL: %s\n", name);                                \
            printf("      expected %d, got %d\n",                      \
                   (int)(expected), (int)(actual));                     \
            failed++;                                                  \
        }                                                              \
    } while (0)

int main(void)
{
    status_t status;

    printf("Testing get_field()\n");

    CHECK("get_field normal",
          get_field(0x2CU, 2, 3), 3U);

    CHECK("get_field width 1",
          get_field(0x1U, 0, 1), 1U);

    CHECK("get_field width 32",
          get_field(0x12345678U, 0, 32), 0x12345678U);

    CHECK("get_field position 31",
          get_field(0x80000000U, 31, 1), 1U);

    CHECK("get_field invalid range",
          get_field(0xFFFFFFFFU, 31, 2), 0U);


    printf("\nTesting set_field()\n");

    CHECK("set_field normal",
          set_field(0x00U, 4, 3, 5U), 0x50U);

    CHECK("set_field width 1",
          set_field(0x00U, 31, 1, 1U), 0x80000000U);

    CHECK("set_field width 32",
          set_field(0xFFFFFFFFU, 0, 32, 0x12345678U),
          0x12345678U);

    CHECK("set_field value too wide",
          set_field(0x00U, 4, 3, 0xFU), 0x70U);

    CHECK("set_field invalid range",
          set_field(0x12345678U, 31, 2, 0U), 0x12345678U);


    printf("\nTesting sign_extend()\n");

    CHECK("sign_extend positive",
          sign_extend(0x16U, 8), 22);

    CHECK("sign_extend negative",
          sign_extend(0xF8U, 8), -8);

    CHECK("sign_extend width 1 negative",
          sign_extend(0x1U, 1), -1);

    CHECK("sign_extend most negative 8-bit",
          sign_extend(0x80U, 8), -128);

    CHECK("sign_extend width 32",
          sign_extend(0x80000000U, 32), INT32_MIN);


    printf("\nTesting print_binary()\n");
    printf("Expected: 0010 1100\n");
    printf("Actual:   ");
    print_binary(0x2CU, 8);
    printf("\n");


    printf("\nTesting status_unpack()\n");

    status = status_unpack(0x1631U);

    CHECK("0x1631 setpoint", status.setpoint, 22);
    CHECK("0x1631 mode", status.mode, MODE_AUTO);
    CHECK("0x1631 heat", status.heat, 1);
    CHECK("0x1631 cool", status.cool, 0);
    CHECK("0x1631 fan", status.fan, 0);
    CHECK("0x1631 fault", status.fault, 0);
    CHECK("0x1631 reserved", status.reserved, 0);

    status = status_unpack(0x0000U);

    CHECK("zero word setpoint", status.setpoint, 0);
    CHECK("zero word mode", status.mode, MODE_OFF);
    CHECK("zero word heat", status.heat, 0);

    status = status_unpack(0xF845U);

    CHECK("negative setpoint", status.setpoint, -8);
    CHECK("mode FAN_ONLY", status.mode, MODE_FAN_ONLY);
    CHECK("heat set", status.heat, 1);
    CHECK("fan set", status.fan, 1);

    status = status_unpack(0x0050U);

    CHECK("invalid mode", status.mode, MODE_INVALID);


    printf("\nSummary: %d passed, %d failed\n", passed, failed);

    if (failed != 0) {
        return 1;
    }

    return 0;
}
