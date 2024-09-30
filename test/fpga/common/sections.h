#ifndef __SECTION_MACROS_H
#define __SECTION_MACROS_H

#define __STRING(x)     #x

#ifndef __not_in_flash
#define __not_in_flash(group) __attribute__((section(".time_critical." group)))
#endif

#ifndef __not_in_flash_func
#define __not_in_flash_func(func_name) __not_in_flash(__STRING(func_name)) func_name
#endif

#ifndef __time_critical_func
#define __time_critical_func(func_name) __not_in_flash_func(func_name)
#endif

#ifndef __no_inline_not_in_flash_func
#define __no_inline_not_in_flash_func(func_name) __noinline __not_in_flash_func(func_name)
#endif

#endif
