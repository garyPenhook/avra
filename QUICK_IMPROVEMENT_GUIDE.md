# Quick Improvement Guide - AVRA Assembler

## TL;DR - Next 4-5 Day Task

**Recommended Improvement:** Modernize to C99 + Refactor `mnemonic.c`

```
Phase 1 (1 day):   C99 modernization (stdbool.h)
Phase 2 (2-3 days): Refactor mnemonic.c function pointers
Phase 3 (1 day):    Test & validate
```

---

## Code Locations for All Identified Issues

### 1. C99 Modernization

**Files to Modify:**
- `src/misc.h` - Contains custom bool definition
- `src/avra.h` - May have related definitions
- All source files - Replace `True`/`False` with `true`/`false`

**Current Code (misc.h):**
```c
#define False 0
#define True 1
typedef int bool;
```

**Target:**
```c
#include <stdbool.h>
// Remove True/False defines, use true/false globally
```

**Find & Replace:**
```bash
# Find all True/False usage:
grep -r "True\|False" src/

# Replace strategy:
# 1. Replace True -> true
# 2. Replace False -> false
# 3. Add #include <stdbool.h> to misc.h or each file needing it
# 4. Remove #define True/False from misc.h
```

---

### 2. Mnemonic.c Refactor (HIGHEST PRIORITY)

**File:** `src/mnemonic.c`

**Main Problem Area:** Lines 346-670 in `parse_mnemonic()` function

**Current Structure:**
```c
parse_mnemonic(struct prog_info *pi)  // Line 346
{
    // ... setup code ...

    if (mnemonic <= MNEMONIC_BREAK) {
        // Handle no-operand instructions
    } else if (mnemonic <= MNEMONIC_ELPM) {
        // Handle indirect operand
    } else {
        // Check for operand presence
        if (mnemonic <= MNEMONIC_BCLR) {
            // BSET/BCLR handling
        } else if (mnemonic <= MNEMONIC_ROL) {
            // Single register
        } else if (mnemonic <= MNEMONIC_RCALL) {
            // Branch
        } else if (mnemonic <= MNEMONIC_CALL) {
            // CALL/JMP
        } else if (mnemonic <= MNEMONIC_CPI) {
            // Two-register
        } // ... more elseifs ...
    }
}
```

**Refactoring Strategy:**

1. Create handler functions for instruction groups:
```c
static int handle_no_operand(struct prog_info *pi, int mnemonic)
static int handle_indirect(struct prog_info *pi, int mnemonic)
static int handle_bit_operation(struct prog_info *pi, int mnemonic)
static int handle_single_register(struct prog_info *pi, int mnemonic)
static int handle_branch(struct prog_info *pi, int mnemonic)
static int handle_call_jmp(struct prog_info *pi, int mnemonic)
static int handle_two_register(struct prog_info *pi, int mnemonic)
// etc.
```

2. Build a dispatch table in `instruction_list` or create separate table:
```c
typedef int (*handler_fn)(struct prog_info *pi, int mnemonic);

struct instruction_handler {
    const char *name;
    handler_fn handler;
    int opcode;  // or store in instruction_list
};
```

3. Simplify `parse_mnemonic()`:
```c
parse_mnemonic(struct prog_info *pi)
{
    int mnemonic = get_mnemonic_type(pi);
    if (mnemonic == -1) {
        // Handle macro/unknown
        return expand_macro_or_error(pi);
    }

    // Get handler for this instruction type
    handler_fn handler = get_handler(mnemonic);
    if (handler) {
        return handler(pi, mnemonic);
    }
    // Fallback
    return True;
}
```

**Reference Points in Code:**
- `instruction_list[]` - Array of instruction definitions (mnemonic.c around line 200+)
- `MNEMONIC_*` enums - Instruction type constants (lines 39-160)
- `get_mnemonic_type()` - Function to get instruction enum (line 675)

**Test Points:**
- Run `make check` (or `cd tests/regression && bash runtests.sh`)
- All 23 regression tests must pass
- Compare binary size before/after
- Performance should be same or better

---

### 3. Directive.c Similar Issue

**File:** `src/directiv.c` (980 lines)

**Problem Area:** Directive parsing similar if-elseif chains

**Main Function:** `parse_directive()`

**Directives to Handle:**
- `.ifdef`, `.ifndef`, `.if`, `.else`, `.endif` - Conditional assembly
- `.define`, `.undef` - Symbol definition
- `.include` - File inclusion
- `.org` - Origin/address setting
- `.db`, `.dw`, `.dd`, `.dq` - Data directives
- `.byte`, `.cseg`, `.dseg`, `.eseg` - Memory directives
- And ~15+ more

**Refactoring:** Similar function pointer approach as mnemonic.c

---

### 4. Known Bugs (Prioritized by Severity)

#### Bug #1: Recursive Macro SIGSEGV (CRITICAL)
**Severity:** HIGH - Causes crash
**Expected File:** `src/macro.c` (543 lines)
**Issue:** Stack overflow or infinite recursion when expanding recursive macros
**TODO Line:** "recursion - sigsegv"

**Investigation Points:**
- Check `expand_macro()` function
- Look for recursion depth limit
- Check `first_macro_call` stack management

#### Bug #2: LDS/STS I/O Addressing (HIGH)
**Severity:** HIGH - Wrong code generation
**Expected File:** `src/mnemonic.c` - likely around LDS/STS instruction handlers
**Issue:** These instructions should address I/O space but don't
**TODO Line:** "LDS and STS instructions don't address I/O"

#### Bug #3: ATmega8 CALL Instruction (MEDIUM)
**Severity:** MEDIUM - Device-specific issue
**Expected File:** `src/device.c` or `src/mnemonic.c`
**Issue:** CALL instruction encoding wrong for ATmega8
**TODO Line:** "ATmega8 CALL instruction is bogus"

#### Bug #4-7: Other Issues
- Forward reference conditional assembly (directiv.c)
- Expression parser edge case: "((str_start_%)<<1)" (expr.c)
- Listing file double-write remark (file.c or listing output)
- Inconsistent hex formatting (%x vs %X)

---

### 5. Device Flags Architecture Issue

**File:** `src/device.c` (254 lines)
**Include Files:** `includes/*.inc` (~150 device definition files)

**Current Problem:** Device flag assignments are error-prone

**Current Approach:**
```c
struct device {
    // ... other fields ...
    int instruction_flags;  // Boolean flags for capabilities
    // e.g., has_mul, has_call_3bytes, etc.
};
```

**Reference:** See how devices are initialized in device.c around line 100+

**Suggested Fix:** Use capability bitmap (20 bytes per device, 160 bits for different instructions)

---

### 6. Args.c Simplification

**File:** `src/args.c` (303 lines)

**Current:** Manual getopt loop

**Suggestion:** Consider adopting simpler pattern from suckless.org/tools/suckless-coding-style

**Lower Priority** - focus on mnemonic.c first

---

## Testing Strategy

### Run Regression Tests
```bash
# From project root
make check

# Or manually:
cd tests/regression
bash runtests.sh
```

### Verify Changes
```bash
# Build the project
make clean
make all

# Check binary size before/after refactoring
ls -lh src/avra

# Run regression tests
make check
```

### Create Unit Tests (Optional)
For each new handler function in mnemonic.c refactor, consider creating targeted tests.

---

## Suggested Implementation Order

### Day 1: C99 Modernization
1. Update misc.h - add `#include <stdbool.h>`, remove `#define True/False`
2. Replace all `True` → `true`, `False` → `false` in source files
3. Run regression tests to verify no breakage
4. Commit: "Modernize to C99: use stdbool.h instead of custom bool"

### Days 2-3: Mnemonic.c Refactor
1. Extract first handler function (e.g., `handle_no_operand()`)
2. Build handler dispatch mechanism
3. Gradually move if-elseif branches into handler functions
4. Test after each major change
5. Commit: "Refactor mnemonic.c: use function pointer dispatch instead of if-elseif chains"

### Day 4: Validation & Testing
1. Run full regression test suite
2. Compare binary before/after
3. Verify performance metrics
4. Code review and cleanup
5. Document changes
6. Final commit: "Complete mnemonic.c refactoring and C99 modernization"

---

## Files You'll Need to Understand

### Core Files:
- `src/mnemonic.c` - Instruction encoding (main target)
- `src/misc.h` - Type definitions (bool types here)
- `src/avra.h` - Main header with data structures
- `src/instruction_list[]` - Instruction definitions

### Supporting:
- `src/directiv.c` - Directive processing (similar pattern)
- `src/device.c` - Device definitions
- `src/macro.c` - Macro handling
- `src/expr.c` - Expression evaluation

### Build & Test:
- `src/makefiles/Makefile.linux` - Build rules
- `tests/regression/runtests.sh` - Test runner
- `Makefile` - Top-level make

---

## Key Commands

```bash
# Build
cd /home/w4gns/Projects/avra-fork
make clean
make all

# Run tests
make check

# Find specific code
grep -n "parse_mnemonic" src/mnemonic.c
grep -n "MNEMONIC_" src/mnemonic.c | head -30

# Count lines in a file
wc -l src/mnemonic.c

# Search for patterns
grep -n "else if.*mnemonic" src/mnemonic.c
```

---

## Success Criteria

✅ All 23+ regression tests pass
✅ Binary size remains similar (within 2%)
✅ No performance regression
✅ Code complexity reduced (lower cyclomatic complexity in parse_mnemonic)
✅ New code follows consistent style
✅ Changes committed with clear messages
✅ Can proceed to bug fixes with cleaner codebase

---

## Questions to Answer During Implementation

1. Should handler functions return `int` (opcode) or void (side-effect based)?
2. Should we create one static handler table or multiple files?
3. How to handle instruction-specific error messages in handlers?
4. Should we add handler function documentation?
5. Any performance differences between if-elseif vs function pointers on this codebase?

---

## References

- **TODO file:** Lists all known issues: [TODO](TODO)
- **Project docs:** [REFERENCE.md](docs/REFERENCE.md)
- **Test docs:** [tests/regression/README.md](tests/regression/README.md)
- **Recent work:** Commits 7fd8f92 and c7759a7 show recent improvements

---

**Last Updated:** 2025-10-16 | **Prepared for:** AVRA 1.4.2 on performance-optimizations branch
