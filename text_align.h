#ifndef __TEXT_ALIGN_
#define __TEXT_ALIGN_

#include <sys/types.h>

int text_align(const char *originFileName,
               const char *alignedFileName,
               const int padToLen);
off_t fsize(const char *path);

#endif // __TEXT_ALIGN_
