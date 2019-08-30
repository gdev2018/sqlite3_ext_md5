#include <cstddef>
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int sqlite3Md5Init(sqlite3 *db);
//long long getLongOffset(unsigned char array[], int offset);
long long getLongOffset(const unsigned char *hex, int offset);
char *bin2hex(const unsigned char *bin, size_t len, int lower);
size_t hexs2bin(const char *hex, unsigned char **out);

//char md5direct(const char str);

#ifdef __cplusplus
}  /* extern "C" */
#endif  /* __cplusplus */
