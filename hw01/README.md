# Homework 1 - Bit Manipulation and Status Word

## Description

This homework implements a small C library for bit manipulation and for decoding a 16-bit thermostat status word.

The bit library includes functions to print a value in binary, extract a bit field, replace a bit field, and sign-extend a value. The thermostat code uses these functions to decode the individual fields from a packed 16-bit status word.

## Build and Test

To compile the source files:

```bash
make
```

To build and run the tests:

```bash
make test
```

To remove the generated object files and test executable:

```bash
make clean
```

## Bit Functions

The library implements the following functions:

- `print_binary()` prints the requested number of bits from most significant to least significant and separates groups of four bits with spaces.
- `get_field()` extracts a field from a 32-bit word and shifts it down to bit 0.
- `set_field()` replaces a field in a 32-bit word using the lowest bits of the provided value.
- `sign_extend()` interprets the lowest requested number of bits as a signed two's-complement value.

## Input Ranges and Boundary Behavior

Valid widths are from 1 through 32.

Valid bit positions are from 0 through 31.

For `get_field()` and `set_field()`, `pos + width` must not be greater than 32.

The implementation handles boundary cases such as width 1, width 32, and position 31.

For an invalid position or width, `get_field()` returns 0.

For an invalid position or width, `set_field()` returns the original word unchanged.

If the value passed to `set_field()` is wider than the selected field, only the lowest `width` bits of the value are used.

## Thermostat Status Word

The 16-bit thermostat status word is decoded as follows:

- Bit 0: HEAT
- Bit 1: COOL
- Bit 2: FAN
- Bit 3: FAULT
- Bits 6-4: MODE
- Bit 7: RESERVED
- Bits 15-8: SETPOINT

The mode values are:

- 0: OFF
- 1: HEAT
- 2: COOL
- 3: AUTO
- 4: FAN_ONLY
- 5-7: INVALID

Invalid mode values from 5 through 7 are reported as `MODE_INVALID`.

The setpoint is interpreted as a signed 8-bit two's-complement value using `sign_extend()`.

## Testing

The test program checks all four bit manipulation functions and `status_unpack()`.

The tests include normal cases as well as boundary cases for width 1, width 32, position 31, a value wider than the destination field, negative sign extension, invalid field ranges, and thermostat status words.

The thermostat example `0x1631` is tested and decodes to a setpoint of 22 degrees, AUTO mode, heater on, compressor off, fan off, no fault, and the reserved bit clear.
