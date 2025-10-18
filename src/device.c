/***********************************************************************
 *
 *  AVRA - Assembler for the Atmel AVR microcontroller series
 *
 *  Copyright (C) 1998-2020 The AVRA Authors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 *
 *  Authors of AVRA can be reached at:
 *     email: jonah@omegav.ntnu.no, tobiw@suprafluid.com
 *     www: https://github.com/Ro5bert/avra
 */


#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "avra.h"
#include "device.h"

/* C23 compile-time validation */
_Static_assert(sizeof(struct device) > 0, "Device structure must be non-empty");

#define DEV_VAR    "__DEVICE__"	/* Device var name */
#define FLASH_VAR  "__FLASH_SIZE__"	/* Flash size var name */
#define EEPROM_VAR "__EEPROM_SIZE__"	/* EEPROM size var name */
#define RAM_VAR    "__RAM_SIZE__"	/* RAM size var name */
#define DEV_PREFIX "__"		/* Device name prefix */
#define DEV_SUFFIX "__"		/* Device name suffix */
#define DEF_DEV_NAME "DEFAULT"	/* Default device name (without prefix/suffix) */
#define MAX_DEV_NAME 32		/* Max device name length */


/* Field Order:
 * name, flash size (words), RAM start, RAM size (bytes), EEPROM size (bytes),
 * flags */
/* IMPORTANT: THE FLASH SIZE IS IN WORDS, NOT BYTES. This has been a fairly
 * consistent source of bugs when new devices are added. */
struct device device_list[] = {
	/* Default device */
	{.name = NULL, .flash_size = 4194304, .ram_start = 0x60, .ram_size = 8388608, .eeprom_size = 65536, .flag = 0}, /* Total instructions: 137 */

	/* ATtiny Series */
	{.name = "ATtiny4", .flash_size = 256, .ram_start = 0x040, .ram_size = 32, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP|DF_AVR8L},
	{.name = "ATtiny5", .flash_size = 256, .ram_start = 0x040, .ram_size = 32, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP|DF_AVR8L},
	{.name = "ATtiny9", .flash_size = 512, .ram_start = 0x040, .ram_size = 32, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP|DF_AVR8L},
	{.name = "ATtiny10", .flash_size = 512, .ram_start = 0x040, .ram_size = 32, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP|DF_AVR8L},
	{.name = "ATtiny11", .flash_size = 512, .ram_start = 0x000, .ram_size = 0, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_TINY1X|DF_NO_XREG|DF_NO_YREG|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny12", .flash_size = 512, .ram_start = 0x000, .ram_size = 0, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_TINY1X|DF_NO_XREG|DF_NO_YREG|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny13", .flash_size = 512, .ram_start = 0x060, .ram_size = 64, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny13A", .flash_size = 512, .ram_start = 0x060, .ram_size = 64, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny15", .flash_size = 512, .ram_start = 0x000, .ram_size = 0, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_XREG|DF_NO_YREG|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP|DF_TINY1X},
	{.name = "ATtiny20", .flash_size = 1024, .ram_start = 0x040, .ram_size = 128, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_EIJMP|DF_NO_EICALL|DF_NO_MOVW|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_BREAK|DF_AVR8L},
	{.name = "ATtiny22", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny24", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny24A", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny25", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny26", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny28", .flash_size = 1024, .ram_start = 0x000, .ram_size = 0, .eeprom_size = 0, .flag = DF_NO_MUL|DF_NO_JMP|DF_TINY1X|DF_NO_XREG|DF_NO_YREG|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny44", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny44A", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny45", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny48", .flash_size = 2048, .ram_start = 0x100, .ram_size = 256, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny84", .flash_size = 4096, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny85", .flash_size = 4096, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny88", .flash_size = 4096, .ram_start = 0x100, .ram_size = 512, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny261A", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny461A", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny861A", .flash_size = 4096, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny2313", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny2313A", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "ATtiny4313", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},

	/* AT90 series */
	{.name = "AT90S1200", .flash_size = 512, .ram_start = 0x000, .ram_size = 0, .eeprom_size = 64, .flag = DF_NO_MUL|DF_NO_JMP|DF_TINY1X|DF_NO_XREG|DF_NO_YREG|DF_NO_LPM|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S2313", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S2323", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S2333", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S2343", .flash_size = 1024, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 128, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S4414", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S4433", .flash_size = 2048, .ram_start = 0x060, .ram_size = 128, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S4434", .flash_size = 2048, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 256, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S8515", .flash_size = 4096, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90C8534", .flash_size = 4096, .ram_start = 0x060, .ram_size = 256, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},
	{.name = "AT90S8535", .flash_size = 4096, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_MUL|DF_NO_JMP|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_MOVW|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},

	/* AT90USB series */
	/* AT90USB168 */
	/* AT90USB1287 */

	/* ATmega series */
	{.name = "ATmega8", .flash_size = 4096, .ram_start = 0x060, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_JMP|DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega8A", .flash_size = 4096, .ram_start = 0x060, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_JMP|DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega161", .flash_size = 8192, .ram_start = 0x060, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega162", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega163", .flash_size = 8192, .ram_start = 0x060, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega16", .flash_size = 8192, .ram_start = 0x060, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega323", .flash_size = 16384, .ram_start = 0x060, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega32", .flash_size = 16384, .ram_start = 0x060, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega603", .flash_size = 32768, .ram_start = 0x060, .ram_size = 4096, .eeprom_size = 2048, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_MUL|DF_NO_MOVW|DF_NO_LPM_X|DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_BREAK},
	{.name = "ATmega103", .flash_size = 65536, .ram_start = 0x060, .ram_size = 4096, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_MUL|DF_NO_MOVW|DF_NO_LPM_X|DF_NO_ELPM_X|DF_NO_SPM|DF_NO_ESPM|DF_NO_BREAK},
	{.name = "ATmega104", .flash_size = 65536, .ram_start = 0x060, .ram_size = 4096, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM}, /* Old name for mega128 */
	{.name = "ATmega128", .flash_size = 65536, .ram_start = 0x100, .ram_size = 4096, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM},
	{.name = "ATmega128A", .flash_size = 65536, .ram_start = 0x100, .ram_size = 4096, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM},
	{.name = "ATmega48", .flash_size = 2048, .ram_start = 0x100, .ram_size = 512, .eeprom_size = 256, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega48A", .flash_size = 2048, .ram_start = 0x100, .ram_size = 512, .eeprom_size = 256, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega48P", .flash_size = 2048, .ram_start = 0x100, .ram_size = 512, .eeprom_size = 256, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega48PA", .flash_size = 2048, .ram_start = 0x100, .ram_size = 512, .eeprom_size = 256, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega88", .flash_size = 4096, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega88A", .flash_size = 4096, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega88P", .flash_size = 4096, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega88PA", .flash_size = 4096, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega168", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega168A", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega168P", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega168PA", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega169", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega169A", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega169P", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega169PA", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega328", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega328P", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega328PB", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega32U4", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2560, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega8515", .flash_size = 8192, .ram_start = 0x060, .ram_size = 512, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega1280", .flash_size = 65536, .ram_start = 0x200, .ram_size = 8192, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM},
	{.name = "ATmega164P", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega164PA", .flash_size = 8192, .ram_start = 0x100, .ram_size = 1024, .eeprom_size = 512, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega324A", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega324P", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega324PA", .flash_size = 16384, .ram_start = 0x100, .ram_size = 2048, .eeprom_size = 1024, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega644", .flash_size = 32768, .ram_start = 0x100, .ram_size = 4096, .eeprom_size = 2048, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega644P", .flash_size = 32768, .ram_start = 0x100, .ram_size = 4096, .eeprom_size = 2096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega644PA", .flash_size = 32768, .ram_start = 0x100, .ram_size = 4096, .eeprom_size = 2096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ELPM|DF_NO_ESPM},
	{.name = "ATmega1284P", .flash_size = 65536, .ram_start = 0x100, .ram_size = 16384, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM},
	{.name = "ATmega1284PA", .flash_size = 65536, .ram_start = 0x100, .ram_size = 16384, .eeprom_size = 4096, .flag = DF_NO_EICALL|DF_NO_EIJMP|DF_NO_ESPM},
	{.name = "ATmega2560", .flash_size = 131072, .ram_start = 0x200, .ram_size = 8192, .eeprom_size = 4096, .flag = DF_NO_ESPM},
	{.name = "ATmega2561", .flash_size = 131072, .ram_start = 0x200, .ram_size = 8192, .eeprom_size = 4096, .flag = DF_NO_ESPM},
	{.name = "ATmega4809", .flash_size = 24000, .ram_start = 0x2800, .ram_size = 6000, .eeprom_size = 256, .flag = DF_NO_ELPM|DF_NO_ESPM|DF_NO_EICALL|DF_NO_EIJMP},

	/* Other */
	{.name = "AT94K", .flash_size = 8192, .ram_start = 0x060, .ram_size = 16384, .eeprom_size = 0, .flag = DF_NO_ELPM|DF_NO_SPM|DF_NO_ESPM|DF_NO_BREAK|DF_NO_EICALL|DF_NO_EIJMP},

	{.name = NULL, .flash_size = 0, .ram_start = 0, .ram_size = 0, .eeprom_size = 0, .flag = 0}
};

static int LastDevice=0;

/* Define vars for device in LastDevice. */
static void
def_dev(struct prog_info *pi)
{
	def_var(pi,DEV_VAR,LastDevice);
	def_var(pi,FLASH_VAR,device_list[LastDevice].flash_size);
	def_var(pi,EEPROM_VAR,device_list[LastDevice].eeprom_size);
	def_var(pi,RAM_VAR,device_list[LastDevice].ram_size);
}

struct device *get_device(struct prog_info *pi, char *name)
{
	int i = 1;
	struct device *result = NULL;

	LastDevice = 0;
	if (name == NULL) {
		def_dev(pi);
		return (&device_list[0]);
	}

	/* Linear search through device list - kept simple for reliability
	   The device_list may not be sorted, so binary search isn't always safe */
	while (device_list[i].name) {
		if (!nocase_strcmp(name, device_list[i].name)) {
			LastDevice = i;
			result = &device_list[i];
			break;
		}
		i++;
	}

	def_dev(pi);
	return result;
}

/* Pre-define devices. */
int
predef_dev(struct prog_info *pi)
{
	int i;
	char temp[MAX_DEV_NAME+1];
	def_dev(pi);
	for (i=0; (!i)||(device_list[i].name); i++) {
		strncpy(temp,DEV_PREFIX,MAX_DEV_NAME);
		if (!i) strncat(temp,DEF_DEV_NAME,MAX_DEV_NAME);
		else strncat(temp,device_list[i].name,MAX_DEV_NAME);
		strncat(temp,DEV_SUFFIX,MAX_DEV_NAME);
		/* Forward references allowed. But check, if everything is ok ... */
		if (pi->pass==PASS_1) { /* Pass 1 */
			if (test_constant(pi,temp,NULL)!=NULL) {
				fprintf(stderr,"Error: Can't define symbol %s twice. Please don't use predefined symbols !\n", temp);
				return (False);
			}
			if (def_const(pi, temp, i)==False)
				return (False);
		} else { /* Pass 2 */
			int j;
			if (get_constant(pi, temp, &j)==False) {  /* Defined in Pass 1 and now missing ? */
				fprintf(stderr,"Constant %s is missing in pass 2\n",temp);
				return (False);
			}
			if (i != j) {
				fprintf(stderr,"Constant %s changed value from %d in pass1 to %d in pass 2\n",temp,j,i);
				return (False);
			}
			/* OK. definition is unchanged */
		}
	}
	return (True);
}

void
list_devices(void)
{
	int i = 1;
	printf("Device name   | Flash size | RAM start | RAM size | EEPROM size |  Supported\n"
	       "              |  (words)   | (bytes)   | (bytes)  |   (bytes)   | instructions\n"
	       "--------------+------------+-----------+----------+-------------+--------------\n"
	       " (default)    |    %7ld |    0x%04lx |  %7ld |       %5ld |          %3d\n",
	       device_list[0].flash_size,
	       device_list[0].ram_start,
	       device_list[0].ram_size,
	       device_list[0].eeprom_size,
	       count_supported_instructions(device_list[0].flag));
	while (device_list[i].name) {
		printf(" %-12s |    %7ld |    0x%04lx |  %7ld |       %5ld |          %3d\n",
		       device_list[i].name,
		       device_list[i].flash_size,
		       device_list[i].ram_start,
		       device_list[i].ram_size,
		       device_list[i].eeprom_size,
		       count_supported_instructions(device_list[i].flag));
		i++;
	}
}

/* end of device.c */

