# AVRA Assembler - Comprehensive Project Analysis

## Executive Summary

AVRA is an advanced macro assembler for Atmel AVR microcontrollers, written in C99. It serves as a compatible alternative to Atmel's official AVRASM32 assembler with additional features and enhancements. The project is currently on the **performance-optimizations** branch with recent work focused on implementing hash table optimizations for O(1) symbol lookups and removing unnecessary profiling infrastructure.

**Project Stats:**
- Total Lines of Code: ~8,900 LOC (C source only, excluding tests/docs)
- Largest Module: COFF support (2,093 lines)
- Main Executable: ~128 KB (compiled binary)
- Current Version: 1.4.2
- Build System: Platform-dependent Makefiles (Linux, Windows, macOS, etc.)

---

## 1. Overall Codebase Organization

### Directory Structure

```
avra-fork/
├── src/                          # Core assembler source code
│   ├── avra.c/h                 # Main driver and data structures
│   ├── parser.c                 # Line-by-line parsing engine
│   ├── mnemonic.c               # Instruction encoding (largest module)
│   ├── directiv.c               # Directive processing
│   ├── expr.c                   # Expression evaluation
│   ├── macro.c                  # Macro expansion system
│   ├── device.c/h               # Device definitions and flags
│   ├── hashtable.c/h            # Symbol lookup hash tables [NEW]
│   ├── coff.c/h                 # COFF debug file generation
│   ├── file.c                   # Output file handling (HEX, EEPROM)
│   ├── args.c/h                 # Command-line argument parsing
│   ├── map.c                    # Map file generation
│   ├── stdextra.c               # String utilities
│   ├── misc.h                   # Common macros and definitions
│   ├── stab.h                   # Symbol table structures
│   └── makefiles/               # Platform-specific build rules
│       ├── Makefile.linux       # Primary build target
│       ├── Makefile.mingw32
│       ├── Makefile.osx
│       └── ...                  # Other platforms (amiga, emx, etc.)
├── docs/                         # Comprehensive documentation
│   ├── REFERENCE.md             # Programming reference guide
│   ├── INSTRUCTION-QUICK-REF.md # Instruction set quick reference
│   ├── DIRECTIVES.md            # Directive documentation
│   └── README.md                # Documentation index
├── includes/                     # AVR device definition files (.inc)
│   ├── m328pdef.inc             # ATmega328P definitions
│   ├── m4809def.inc             # ATmega4809 definitions
│   └── ...                      # ~150+ device files
├── tests/                        # Test suites
│   ├── regression/              # Regression test cases
│   └── overlap/                 # Overlap detection tests
├── examples/                     # Example assembly programs
├── Makefile                      # Top-level build control
├── README.md                     # Project overview
├── USAGE.md                      # User guide and AVRA features
├── TODO                          # Outstanding work items
├── CHANGELOG.md                  # Version history
└── AUTHORS                       # Contributor list

```

### Key Module Responsibilities

| Module | Size | Purpose | Key Functions |
|--------|------|---------|----------------|
| **avra.c** | 956 lines | Main driver, global state | main(), assemble(), print_msg(), segment management |
| **parser.c** | 396 lines | Line parsing orchestrator | parse_file(), parse_line(), get_next_token() |
| **mnemonic.c** | 845 lines | Instruction encoding | parse_mnemonic(), get_mnemonic_type(), instruction dispatch |
| **directiv.c** | 980 lines | Directive processing | parse_directive(), conditional assembly (.ifdef, .if, etc.) |
| **expr.c** | 566 lines | Expression parser | get_expr(), operator precedence, symbol resolution |
| **macro.c** | 543 lines | Macro expansion | read_macro(), expand_macro(), macro_label tracking |
| **device.c** | 254 lines | Device support | get_device(), device flags, 150+ supported devices |
| **hashtable.c** | 188 lines | Symbol lookup [NEW] | hash_table_insert/lookup, FNV-1a hashing (O(1) lookups) |
| **coff.c** | 2,093 lines | Debug symbols | write_coff_file(), COFF format generation |
| **file.c** | 329 lines | Output generation | write_prog_word(), open/close hex files, EEPROM output |
| **args.c** | 303 lines | CLI parsing | Command-line option processing |

---

## 2. Key Components and Their Relationships

### Data Flow Architecture

```
INPUT (Assembly File)
    |
    v
[args.c] - Parse command-line arguments
    |
    v
[avra.c - main()] - Initialize program context
    |
    +---> [device.c] - Load device capabilities
    |
    +---> [hashtable.c] - Initialize symbol lookup tables
    |
    v
PASS 1 (Symbol Resolution)
    |
    +---> [parser.c - parse_file()]
            |
            +---> [parser.c - parse_line()] - Line tokenization
                    |
                    +---> [directiv.c] - Process directives (.org, .db, .define, etc.)
                    |
                    +---> [mnemonic.c] - Parse instructions
                    |
                    +---> [expr.c] - Evaluate expressions
                    |
                    +---> [macro.c] - Handle macro expansion
                    |
                    +---> [hashtable.c] - Insert symbols (O(1))
    |
    v
PASS 2 (Code Generation)
    |
    +---> Same as PASS 1, but with symbol resolution enabled
    |
    v
OUTPUT
    |
    +---> [file.c] - Generate .hex file (Intel HEX format)
    |
    +---> [file.c] - Generate .eep.hex (EEPROM)
    |
    +---> [coff.c] - Generate .coff (debug symbols)
    |
    +---> [map.c] - Generate .map (symbol map)
    |
    v
[avra.c - main()] - Report statistics and exit
```

### Core Data Structures (prog_info)

The central state container:

```c
struct prog_info {
    // Input/Output
    struct args *args;              // Command-line arguments
    struct file_info *fi;           // Current file being parsed
    FILE *list_file;                // Listing file output
    FILE *obj_file;                 // Object file (HEX)
    FILE *coff_file;                // Debug symbols
    
    // Device Configuration
    struct device *device;          // Target AVR device info
    
    // Memory Segments
    struct segment_info *cseg;      // Code segment (Flash)
    struct segment_info *dseg;      // Data segment (RAM)
    struct segment_info *eseg;      // EEPROM segment
    struct segment_info *segment;   // Current active segment
    
    // Symbol Tables (O(1) lookup with hash tables)
    struct hash_table *label_hash;      // Labels (fast lookup)
    struct hash_table *constant_hash;   // Constants (fast lookup)
    struct hash_table *variable_hash;   // Variables (fast lookup)
    struct label *first_label;          // Labels (linked list)
    struct label *first_constant;       // Constants
    struct label *first_variable;       // Variables
    
    // Macro System
    struct macro *first_macro;          // Defined macros
    struct macro_call *macro_call;      // Current macro expansion context
    struct macro_call *first_macro_call;// Call stack
    
    // Preprocessing
    struct def *first_def;              // .define directives
    struct location *first_ifdef_blacklist;  // ifdef filtering
    struct location *first_ifndef_blacklist; // ifndef filtering
    
    // Error/Warning Tracking
    int error_count;                // Total errors encountered
    int warning_count;              // Total warnings
    int max_errors;                 // Max before abort
    
    // State
    int pass;                       // PASS_1 or PASS_2
    int conditional_depth;          // Nested .if depth
    time_t time;                    // Build timestamp for meta tags
    
    // Memory Overlap Tracking
    struct orglist *first_orglist;  // Used memory regions
    int effective_overlap;          // Overlap handling mode
};
```

### Symbol Resolution with Hash Tables (Recent Optimization)

**Before (commit 7fd8f92):** Implemented in last 2 commits
- Added `hashtable.c` and `hashtable.h`
- FNV-1a hash algorithm with case-insensitive matching
- 256-bucket hash table for ~O(1) symbol lookups
- Reduced search time from O(n) to O(1 + α) where α << 2

**Impact:** 10-50x performance improvement for projects with many symbols

```c
// Hash table structure (separate chaining)
struct hash_table {
    struct label *buckets[256];  // Hash buckets
    int count;                   // Total entries
    int collisions;              // Statistics
};

// Used for:
- Labels
- Constants  
- Variables
```

---

## 3. Build System

### Build Architecture

**Top-level Makefile:**
```makefile
OS ?= linux
VERSION = 1.4.2
PREFIX ?= /usr/local
TARGET_INCLUDE_PATH ?= $(PREFIX)/include/avr

# Main targets
all:
    $(MAKE) -C src -f makefiles/Makefile.$(OS)

install: all
    install -d $(DESTDIR)$(PREFIX)/bin
    install -m 755 src/avra $(DESTDIR)$(PREFIX)/bin
    install -d $(DESTDIR)$(TARGET_INCLUDE_PATH)
    cp includes/* $(DESTDIR)$(TARGET_INCLUDE_PATH)

check: all
    cd tests/regression && ./runtests.sh
```

**Platform Makefiles (src/makefiles/Makefile.*):**

Linux (primary):
```makefile
CC ?= gcc
CFLAGS ?= -Wall -O3
LDFLAGS ?= -s

SOURCES = avra.c device.c parser.c expr.c mnemonic.c \
          directiv.c macro.c file.c map.c coff.c args.c \
          stdextra.c hashtable.c

avra: $(SOURCES:.c=.o)
    $(CC) -o $@ $(OBJECTS) $(LDFLAGS)
```

Supported Platforms:
- Linux (primary, well-tested)
- Windows (MinGW32)
- macOS
- Amiga (GCC, SAS C)
- EMX, OS/2, LCC, BSD

### Compilation Flow

```
make
├── Defines: VERSION, DEFAULT_INCLUDE_PATH
├── Compiles: 13 source files -> 13 object files
├── Links: avra binary with -s (strip symbols)
└── Output: src/avra (~128 KB)
```

### Dependencies Analysis

```
Core Dependencies:
- misc.h        <- Used by all modules (macros, types)
- avra.h        <- Defines prog_info, includes most others
- args.h        <- Command-line argument handling
- device.h      <- Device definitions
- hashtable.h   <- Symbol lookup tables [NEW]

Module Graph:
avra.c → { device.c, parser.c, file.c, map.c, coff.c, args.c, hashtable.c }
parser.c → { expr.c, mnemonic.c, directiv.c, macro.c, hashtable.c }
mnemonic.c → { device.c }
directiv.c → { device.c }
expr.c → {} (standalone)
```

---

## 4. Main Entry Points and How the Assembler Works

### Entry Point: main() in avra.c

```c
int main(int argc, const char *argv[])
{
    // 1. Parse command-line arguments
    args = handle_args(argc, argv);
    
    // 2. Initialize program info structure
    pi = init_prog_info(...);
    pi->device = get_device(pi, device_name);
    
    // 3. Load device memory configuration
    init_segment_size(pi, pi->device);
    
    // 4. Load command-line -D defines
    load_arg_defines(pi);
    
    // 5. Assemble (2 passes)
    assemble(pi);  // PASS_1 and PASS_2
    
    // 6. Free resources
    free_pi(pi);
    
    return error_count > 0 ? 1 : 0;
}
```

### Two-Pass Assembly Process

**PASS 1: Symbol Collection**
```
Parse file
├── Parse each line
│   ├── Tokenize
│   ├── Identify directive/instruction/label
│   ├── For labels: Add to hash table (hashtable.c)
│   ├── For definitions: Add to symbol tables
│   ├── For directives: Execute (.org, .db, etc.)
│   ├── For instructions: Count bytes, validate syntax
│   └── Don't emit code yet
├── Track memory usage (segments)
└── Report errors (don't halt on first error)
```

**PASS 2: Code Generation**
```
Parse file again
├── Parse each line (same tokenization)
├── Now symbols are resolved via hash table lookups (O(1))
├── For instructions: Encode to binary
├── For data: Write .db/.dw values
├── Emit to output files:
│   ├── .hex file (Intel HEX format, via file.c)
│   ├── .eep.hex (EEPROM data)
│   ├── .coff file (debug symbols, via coff.c)
│   └── .map file (symbol map, via map.c)
└── Report final error count
```

### Line Parsing Pipeline (parse_line in parser.c)

```
Input: "  ldi r16, 0xFF  ; Load 255"
  |
  v
Tokenization (get_next_token)
  |
  v
Token: "ldi"  → Identify as instruction
Token: "r16"  → Register operand
Token: "0xFF" → Expression evaluation
  |
  v
Dispatch to Handlers
  ├─> mnemonic.c: parse_mnemonic()
  │   └─> Encode instruction (e.g., ldi → 0xE0FF)
  ├─> directiv.c: parse_directive() (if .xyz)
  │   └─> Handle .org, .db, .define, .ifdef, etc.
  ├─> macro.c: expand_macro() (if macro call)
  ├─> expr.c: get_expr() (for expressions)
  └─> hashtable.c: Insert symbol (if label)
  |
  v
Output
  ├─> Pass 1: Just track symbol, advance PC
  └─> Pass 2: Emit code to output files
```

### Expression Evaluation (expr.c)

```
Input: "0x100 + 2*OFFSET - 1"
  |
  v
Tokenize into elements: [0x100, +, 2, *, OFFSET, -, 1]
  |
  v
Operator Precedence Handling
  ├─> Unary operators: +, -, ~, !
  ├─> Precedence levels: 1=~! 2=* 3=+- 4=& 5=^ 6=|
  └─> Recursive precedence parsing
  |
  v
Symbol Resolution (hashtable lookup - O(1))
  └─> OFFSET → hash_table_lookup() → value
  |
  v
Calculate: (0x100 + 2*value - 1)
  |
  v
Output: Integer result
```

### Macro Expansion System (macro.c)

```
Macro Definition Pass:
.macro add16 rd, rs
    add @rd, @rs
    adc @rd + 1, @rs + 1
.endmacro

Usage: add16 r20, r16
  |
  v
Expansion
  ├─> Replace @rd with r20
  ├─> Replace @rs with r16
  ├─> Expand macro body line-by-line
  ├─> Handle local labels (unique per expansion)
  └─> Recursive macro calls supported
  |
  v
Expanded Code
  add r20, r16
  adc r21, r17
```

### Device Support System (device.c)

```
Device Definition
├─> Name: "atmega328p"
├─> Flash size: 16K words = 32K bytes
├─> RAM start: 0x0100
├─> RAM size: 2K bytes
├─> EEPROM size: 1K bytes
└─> Flags: Instruction set capabilities
    ├─> DF_NO_MUL (no multiply instructions)
    ├─> DF_NO_JMP (no 32-bit jumps)
    ├─> DF_TINY1X (minimal instruction set)
    ├─> DF_NO_MOVW (no MOVW instruction)
    └─> ... (16 flag bits for different features)

Supported Devices: 150+ AVR types
├─> Classic: ATmega328P, ATmega2560, ATtiny85
├─> Modern: ATmega4809 (new megaAVR)
├─> Old: AT90S1200
└─> Special: AT90USB, AT90CAN (with peripherals)
```

---

## 5. Existing TODOs, FIXMEs, and Known Issues

### Critical Issues (TODO file at root)

1. **LDS/STS I/O Addressing** - LDS/STS instructions don't properly address I/O memory region

2. **ATmega8 CALL Bug** - CALL instruction generates incorrect code for ATmega8

3. **Recursion Crash** - Recursive macro calls cause SIGSEGV (buffer overflow likely)

4. **Conditional Forward References** - Conditional assembly with forward-referenced labels on .DW generates incorrect code

5. **Expression Parsing Edge Case** - AVRA fails on expressions like "((str_start_%)<<1)"

6. **Include File Remark Duplication** - If last line in include file is a remark, written twice in listing

7. **Inconsistent Hex Output** - Diagnostic messages use both %x and %X inconsistently

### Architecture Concerns (TODO file)

8. **Args Handling** - Overcomplicated argument parsing. TODO: Simplify using suckless-style approach

9. **Mnemonic Dispatch** - mnemonic.c is giant if-elseif chain (845 lines). TODO: Use function pointers and instruction table

10. **Device Flags** - Device capability system is error-prone and messy
    - Current: 16-bit flag field (all bits used!)
    - Problem: Multiple cases of wrong flags assigned to devices
    - Proposal: Follow gcc-avr approach or use per-device capability bits

11. **Preprocessor Limitations** - No true C preprocessor
    - Workaround: Recommend external preprocessor
    - Issue: '#directive' syntax confusingly looks like C preprocessor

12. **Lexical Analysis** - Too tightly integrated with rest of code
    - TODO: Better separation of concerns

13. **Ifdef Blacklist Tracking** - Using file/line numbers is "kinda hacky"

14. **Platform-Specific Makefiles** - Some look old and probably broken (Amiga, OS/2, etc.)

15. **Code Style** - No consistent style enforcement (brace placement, etc.)

16. **C89 Legacy** - Codebase uses custom bool type instead of C99 stdbool.h

17. **Platform Portability** - Using unistd.h (not POSIX-compliant on Windows)

### Code-Level Issues (grep results)

```
Location               | Type    | Issue
-----------------------+---------+------------------------------------------
mnemonic.c             | XXX     | advance - needs verification
expr.c (6 locations)   | TODO    | Overflow checking on number parsing
parser.c               | TODO    | Verify temp variable necessity
parser.c               | XXX     | TERM_END and TERM_COMMA treated identically
directiv.c             | XXX     | Move code to emit function
directiv.c             | TODO    | UNDEF directive not fully implemented
directiv.c             | XXX     | PAD padding logic unclear
```

### Test Coverage

- Regression tests: 23 test cases in `tests/regression/`
- Overlap detection tests: 18 test cases in `tests/overlap/`
- No unit tests for individual modules
- Manual testing primarily

---

## 6. Recent Changes and Current Work

### Recent Commits (Last 6 Months)

#### Commit c7759a7 (Latest - Oct 16, 2025)
**"Remove unnecessary performance profiling infrastructure"**

- Deleted: src/perf.c, src/perf.h (performance profiling framework)
- Deleted: docs/PERFORMANCE.md, HASH_TABLE_OPTIMIZATION.md, etc.
- Removed: PERF_START/PERF_END macros from codebase
- Removed: --perf command-line option
- Kept: Hash table optimizations (actual performance improvement)

**Rationale:** Over-engineered for assembler tool that processes small files in milliseconds. Hash tables provide O(1) lookup without profiling overhead.

#### Commit 7fd8f92 (Oct 16, 2025)
**"Add performance analysis framework and hash table optimization"** 

**Major Changes:**
- Implemented hash table module (src/hashtable.c/h)
  - FNV-1a hash algorithm
  - 256-bucket separate chaining
  - Case-insensitive symbol matching
  - O(1) average-case lookups

- Modified src/avra.c (138 lines changed)
  - Initialize hash tables in init_prog_info()
  - Use hash_table_lookup() in search_symbol()
  - Insert symbols via hashtable on definition

- Modified src/parser.c (6 lines added)
  - Hash table insertion for label definitions

- Updated Makefile.linux
  - Added hashtable.c to build

**Impact:** 
- Symbol lookup changed from O(n) to O(1)
- 10-50x performance improvement for large projects
- Measurable impact on compile time for 500+ symbol projects

#### Commit e1b53af (Oct 14, 2025)
**"Remove incorrect ATmega4809 stack pointer initialization from documentation"**

- Fixed: docs/REFERENCE.md - Removed misleading ATmega4809 stack setup example
- Issue: ATmega4809 uses different memory addressing than classic AVR

#### Commit eac6396 (Oct 14, 2025)
**"Add comprehensive documentation for AVR assembly programming"**

- Created: docs/REFERENCE.md (18 KB - comprehensive guide)
- Created: docs/INSTRUCTION-QUICK-REF.md (14 KB - quick lookup)
- Updated: docs/README.md (indexed all documentation)
- Content includes: Architecture, examples, device specs, online resources

#### Commit 6778ce0 (Oct 14, 2025)
**"Fix ATmega4809 memory specifications"**

- Fixed: Device definition for ATmega4809
- Corrected: RAM start address and size
- Impact: Code targeting ATmega4809 now compiles correctly

### Current Branch: performance-optimizations

Status: Clean working tree (all changes committed)

Key themes:
1. Performance improvements via hash tables
2. Code cleanup (removed profiling cruft)
3. Documentation expansion
4. Device specification fixes

### Build Status

```bash
make               # Successful compilation
make check         # Regression tests pass
make install       # Works with standard PREFIX
```

---

## 7. Architecture Quality Assessment

### Strengths

1. **Modular Design** - Clear separation between parsing, encoding, and I/O
2. **Two-Pass Assembly** - Enables forward references (except some edge cases)
3. **Extensive Device Support** - 150+ AVR variants supported
4. **Output Flexibility** - HEX, COFF debug, map files, EEPROM
5. **Rich Directive Support** - Far exceeds AVRASM32 feature set
6. **Macro System** - Powerful macro expansion with local labels
7. **Recent Optimization** - Hash tables for O(1) symbol lookups

### Weaknesses

1. **Giant Switch Statements** - mnemonic.c and directiv.c hard to maintain
2. **Device Flag System** - 16-bit field fully used, error-prone scaling
3. **Tight Coupling** - Lexical analysis mixed with parsing logic
4. **Platform Support** - Many platform Makefiles likely broken/untested
5. **Error Handling** - Accumulates errors but doesn't always halt correctly
6. **Code Style** - Inconsistent formatting and naming conventions
7. **Limited Testing** - Only regression tests, no unit tests
8. **Memory Leaks** - No systematic cleanup in error paths

### Scalability Concerns

- **Device flags** - 16-bit field saturated (proposal in TODO)
- **File size limits** - LINEBUFFER_LENGTH = 4096 (fixed buffer)
- **Macro nesting** - MAX_NESTED_MACROLOOPS = 256 (arbitrary limit)
- **Hash table collision chains** - Could degrade if >256 symbols per bucket
- **Memory overhead** - prog_info structure gets large with many symbols

---

## 8. Build and Deployment

### Compilation

```bash
cd /home/w4gns/Projects/avra-fork
make                    # Builds for Linux (default)
make OS=mingw32         # Windows cross-compile
```

Output: `src/avra` (128 KB executable)

### Installation

```bash
make install            # Installs to /usr/local
make PREFIX=/opt/avra install  # Custom location
```

Installs:
- Binary: $(PREFIX)/bin/avra
- Include files: $(PREFIX)/include/avr/*.inc

### Testing

```bash
make check              # Run regression tests
cd tests/regression && ./runtests.sh  # Manual test run
```

Test Coverage:
- 23 regression tests
- Tests: instruction encoding, directives, macros, overlap detection
- All tests currently pass

---

## 9. Project Maturity and Development Status

### Version History

- v0.1-0.7: John Anders Haugum (initial development)
- v0.8-1.1: Tobias Weber
- v1.2: Burkhard Arenfeld
- v1.3: Jerry Jacobs
- v1.4.x: Virgil Dupras (current maintainer, resumed after 8-year hiatus)

### Current Activity Level

- Repository: Actively maintained
- Recent Work: Performance optimization focus (hash tables)
- Contributors: Multiple active developers
- Issues: Accepting pull requests
- Documentation: Recently expanded (Oct 2025)

### Production Readiness

- Stable for most use cases
- Well-tested on classic AVR devices
- Some edge cases remain (see TODO file)
- Used in embedded systems development
- Compatible with AVRASM2 for most code

---

## 10. Command-Line Interface

### Usage

```bash
avra [-f][O|M|I|G] output_format
     [-o <filename>] output file name
     [-d <filename>] debug file (COFF)
     [-e <filename>] EEPROM file
     [-l <filename>] listing file
     [-m <mapfile>] symbol map
     [-D <symbol>[=value]] define
     [-I <dir>] include directory
     [-W <warning>] warning suppression
     [-O e|w|i] overlap handling (error/warning/ignore)
     [--listmac] include macro expansion in listing
     [--max_errors N] max errors before exit
     [--devices] list supported devices
     [--version] version info
     [-h|--help] this help
     <file.asm>
```

### Key Options

- `-D` - Command-line symbol definitions (processed before assembly)
- `-I` - Additional include search paths
- `-l` - Generate listing file (source with line numbers and generated code)
- `-m` - Generate map file (symbol addresses)
- `-O` - Overlap detection policy (critical for code organization)
- `--listmac` - Debug aid: show macro expansions in listing

---

## Summary

AVRA is a well-architected, mature assembler for AVR microcontrollers with recent performance optimizations. The codebase is currently focused on:

1. **Performance** (hash tables for O(1) symbol lookup)
2. **Stability** (bug fixes for edge cases)
3. **Documentation** (comprehensive guides added)
4. **Device Support** (regular updates for new AVR variants)

The recent "performance-optimizations" branch work has successfully implemented O(1) symbol table lookups while maintaining backward compatibility. Code is production-ready with well-understood limitations documented in the TODO file.

Key files to understand the project:
- `src/avra.c` - Entry point and main control flow
- `src/parser.c` - Line-by-line parsing orchestrator
- `src/hashtable.c` - Recent O(1) symbol lookup optimization
- `src/mnemonic.c` - Instruction encoding (largest, most complex)
- `docs/REFERENCE.md` - User-facing documentation

