#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int sqlite3Md5Init(sqlite3 *db);
long long getLongOffset(unsigned char array[], int offset);

//char md5direct(const char str);

#ifdef __cplusplus
}  /* extern "C" */
#endif  /* __cplusplus */
