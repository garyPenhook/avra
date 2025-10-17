# XMEGA RMW (Read-Modify-Write) Instructions Documentation

## Overview

AVRA now supports four atomic Read-Modify-Write (RMW) instructions that were added to XMEGA B, C, and AU processor models. These instructions perform atomic read-modify-write operations on memory-mapped registers, particularly useful for coordinating with DMA peripherals like USB controllers.

## Supported Instructions

### 1. XCH (Exchange)
- **Opcode:** `0x9204`
- **Binary Format:** `1001 001d dddd 0100`
- **Operation:** Exchange contents of register Rd with memory location pointed to by Z
- **Syntax:** `xch Z, Rd`
- **Example:** `xch Z, r0`
- **Cycles:** 2
- **Description:** Atomically swaps the value in register Rd with the value in memory at address Z

### 2. LAS (Load and Set)
- **Opcode:** `0x9205`
- **Binary Format:** `1001 001d dddd 0101`
- **Operation:** Load from memory at Z into Rd, then set bits in memory that are set in Rd
- **Syntax:** `las Z, Rd`
- **Example:** `las Z, r5`
- **Cycles:** 2
- **Description:** Atomically performs Mem[Z] |= Rd, loading the original value into Rd

### 3. LAC (Load and Clear)
- **Opcode:** `0x9206`
- **Binary Format:** `1001 001d dddd 0110`
- **Operation:** Load from memory at Z into Rd, then clear bits in memory that are set in Rd
- **Syntax:** `lac Z, Rd`
- **Example:** `lac Z, r10`
- **Cycles:** 2
- **Description:** Atomically performs Mem[Z] &= ~Rd, loading the original value into Rd

### 4. LAT (Load and Toggle)
- **Opcode:** `0x9207`
- **Binary Format:** `1001 001d dddd 0111`
- **Operation:** Load from memory at Z into Rd, then toggle bits in memory that are set in Rd
- **Syntax:** `lat Z, Rd`
- **Example:** `lat Z, r15`
- **Cycles:** 2
- **Description:** Atomically performs Mem[Z] ^= Rd, loading the original value into Rd

## Usage Examples

### Atomic Bit Manipulation

```asm
; Atomically set bit 0 of a memory location
ldi r0, 0x01
las Z, r0      ; Set bit, r0 now contains original value

; Atomically clear bit 1
ldi r0, 0x02
lac Z, r0      ; Clear bit, r0 contains original value

; Atomically toggle bits 0-3
ldi r0, 0x0F
lat Z, r0      ; Toggle bits, r0 contains original value
```

### DMA Coordination Example

```asm
; Safely exchange status flags with DMA peripheral
mov r1, r20        ; Copy current flags to r1
xch Z, r1          ; Exchange with peripheral, r1 now has peripheral flags
; Process the swapped values...
```

### Atomic Flag Update Pattern

```asm
; Safely update control register while preserving other bits
ldi r2, (1 << CONTROL_BIT)
las Z, r2          ; Set control bit atomically
tst r2             ; Check if bit was already set
breq bit_was_clear
; Handle bit already set case...
```

## Device Support

### Supported Devices
- **XMEGA B-Series** (ATxmegaXXB4U)
- **XMEGA C-Series** (ATxmegaXXC4U)
- **XMEGA AU-Series** (ATxmegaXXA4U variants with AU suffix)

### Not Supported
- ATtiny series
- AT90 series
- Standard ATmega series
- XMEGA A, D, E series (earlier models)

### Device Flag
- Devices without RMW support will have the `DF_NO_RMW` flag (0x10000) set
- When assembling for unsupported devices, RMW instructions will generate an error

## Implementation Details

### Instruction Table Entry
```c
{"xch",   0x9204, DF_NO_RMW},
{"las",   0x9205, DF_NO_RMW},
{"lac",   0x9206, DF_NO_RMW},
{"lat",   0x9207, DF_NO_RMW},
```

### Operand Parsing
- **First Operand:** Must be the Z register (direct or indirect addressing)
- **Second Operand:** Must be a general-purpose register (r0-r31)
- **Addressing:** Always uses Z register for memory addressing
- **Register Encoding:** Destination register (Rd) bits [8:4] of instruction

### Opcode Encoding
All RMW instructions follow the same base format:
```
15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 1  0  0  1  0  0  1  d  d  d  d  d  0  x  x  x
```

Where:
- `d d d d d` = 5-bit destination register (r0-r31)
- `x x x` = 3-bit operation code (100=XCH, 101=LAS, 110=LAC, 111=LAT)

## Memory Layout Requirement

All RMW instructions access memory through the Z register. Before using any RMW instruction, Z must be initialized to point to the target memory location:

```asm
; Setup Z to point to memory location
ldi r30, lo8(target_address)    ; Load low byte into r30 (ZL)
ldi r31, hi8(target_address)    ; Load high byte into r31 (ZH)

; Now use RMW instruction
xch Z, r0
```

## Error Handling

### Unsupported Device Error
```
Error: XCH instruction is not supported on ATmega328p
```
This error occurs when attempting to use RMW instructions on devices that don't support them.

### Missing Operand Error
```
Error: xch needs a second operand
```
This error occurs if only one operand is provided.

### Invalid Addressing Mode Error
```
Error: xch only supports Z register addressing
```
This error occurs if an addressing mode other than Z is used.

## Performance Considerations

- All RMW instructions execute in **2 cycles** (1 cycle for read, 1 cycle for write)
- These are **atomic operations** - no interrupts occur between the read and write
- Useful for implementing **lock-free synchronization** between CPU and peripherals
- Essential for **USB peripheral communication** on XMEGA C variants

## Historical Context

These instructions were added in 2010 to enhance XMEGA processor support and are not available in earlier AVR microcontroller families. They specifically target the XMEGA B, C, and AU processor variants which feature enhanced peripheral capabilities including high-speed USB interfaces that benefit from atomic register operations.

## See Also

- AVR Instruction Set Manual
- XMEGA B Datasheet
- XMEGA C Datasheet
- XMEGA AU Series Datasheet

## References

- [AVR Freaks Discussion on RMW Instructions](https://www.avrfreaks.net/forum/lac-las-lat)
- [Stack Overflow - RMW Instructions](https://stackoverflow.com/questions/8901275/)
- Microchip AVR Instruction Set Manual (DS40002198)
