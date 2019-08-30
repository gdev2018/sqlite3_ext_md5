#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int sqlite3Md5Init(sqlite3 *db);
int md5test(int value);
//unsigned char md5direct(int argc, sqlite3_value **argv[]);
long long getLongOffset(unsigned char array[], int offset);

//char md5direct(const char str);

#ifdef __cplusplus
}  /* extern "C" */
#endif  /* __cplusplus */
