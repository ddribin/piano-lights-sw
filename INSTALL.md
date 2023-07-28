# Installation

## Fuses

[Default fuses](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p) for an ATmega238/P are:

| Low    | High   | Extended |
|--------|--------|----------|
| `0x62` | `0xD9` | `0xFF`   |

[Change fuses](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p) to (which are the same as the Arduino Uno):

| Low    | High   | Extended |
|--------|--------|----------|
| `0xFF` | `0xDE` | `0xFD`   |

Changes to Low Fuse:
- Use 16 Mhz external oscillator instead of internal RC oscillator
- Disables the clock divider

Changes to High Fuse:
- Change boot flash section from 2048 to 256 bytes
- Enable boot reset vector

Changes to Extended Fuse:
- Enable brown out detection at V_CC = 2.7V

Read fuses with:

```
avrdude -qq -c usbtiny -p atmega328 -U lfuse:r:-:r -U hfuse:r:-:r -U efuse:r:-:r | hexdump -C
```

Write fuses with:

```
avrdude -c usbtiny -p atmega328 -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFD:m
```

Read firmware:

```
avrdude -c usbtiny -p m328 -U flash:r:piano-lights.hex:i
```
```
avrdude -c usbtiny -p m328 -U flash:r:piano-lights.bin:r
```
