#ifndef MISSY_KERNEL_EXCEPTION_H
#define MISSY_KERNEL_EXCEPTION_H

#ifdef _cplusplus
extern "C" {
#endif

void kernel_panic (const char *err);

#ifdef _cplusplus
}
#endif

#endif
