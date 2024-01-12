#define RAYGUI_IMPLEMENTATION

#if defined(_MSC_VER)

// Note: the `#define`s only work if sizeof(buffer) is a meaningful value,
// a.k.a, buffer is declared as `char buffer[SIZE]`, instead of just a `char*`.
// Here, we rely on this behavior:
// char  buffer[size]; (uintptr_t)&buffer == (uintptr_t)buffer;
// char* buffer;       (uintptr_t)&buffer != (uintptr_t)buffer;

#define IS_ARRAY(arr) ((uintptr_t)(void*)&(arr) == (uintptr_t)(arr))

#include <stdio.h>
#define sprintf(buffer, ...) sprintf_s(buffer, (IS_ARRAY(buffer) ? sizeof(buffer) : -1), __VA_ARGS__)

// fopen/fopen_s   doesn't have trivial fix
// sscanf/sscanf_s doesn't even have a moderate fix.
// So, just disable the warning.

#pragma warning(disable: 4267) // size_t to int conversion.
#pragma warning(disable: 4996) // fopen/fopen_s sscanf/sscanf_s

#endif

#include "raygui.h"
