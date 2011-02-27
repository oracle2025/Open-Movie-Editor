#include <string.h>

#include "strlcpy.h"

size_t strlcpy(char *dst, const char *src, size_t dstsize)
{
	if (strlen(src) < dstsize) {
		strcpy(dst,src);
	} else {
		strncpy(dst,src,dstsize-1);
		dst[dstsize-1] = '\0';
	}
	return strlen(src);
}

