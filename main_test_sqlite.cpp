//#include <stdio.h>

#include <sqlite3.h>
#include <md5.h>

#include <iostream>				// в Си: #include <stdio.h>
#include <cstring>

using namespace std;


static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    printf("%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//        std::cout << (argv[i]);
    }

    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
//    //--------- test getLongOffset by unsigned char
//    unsigned char digest[] = "1bc29b36f623ba82aaf6724fd3b16718";
    unsigned char digest[] = "86ae973193b63cf8239bb7cdb3adcc13";    // returns 17 digits
    long long md5long;
    md5long = abs(getLongOffset(digest, 0) ^ getLongOffset(digest, 8));
    printf("md5long: %lld\n", md5long);
    return 0;

//    //--------- test getLongOffset by char *a
////    unsigned char digest[] = "1bc29b36f623ba82aaf6724fd3b16718";
//    const char    *a = "1bc29b36f623ba82aaf6724fd3b16718";
//    long long md5long;
//    md5long = (getLongOffset((unsigned char *)a, 0) ^ getLongOffset((unsigned char *)a, 8));
//    printf("md5long: %lld\n", md5long);
////    return 0;

//    //--------- test hex-bin-hex-getLongOffset
//    // 1-hex2bin
//    const char    *a = "1bc29b36f623ba82aaf6724fd3b16718";
//    printf("a = %s\n", a);
//    unsigned char *bin;
//    size_t         binlen;
//    binlen = hexs2bin(a, &bin);
//    printf("bin = %.*s\n", (int)binlen, (char *)bin);
//    // 2-bin2hex
//    char          *hex;
//    hex = bin2hex((unsigned char *)bin, binlen);
//    printf("hex = %s\n", hex);
//    printf("binlen = %d\n", binlen);
//    printf("strlen((char *)bin)) = %d\n", strlen((char *)bin));
//    // 3-getLongOffset by char *hex
//    long long md5long;
//    md5long = (getLongOffset((unsigned char *)hex, 0) ^ getLongOffset((unsigned char *)hex, 8));
//    printf("md5long by (unsigned char *)hex: %lld\n", md5long);
//    md5long = (getLongOffset((unsigned char *)a, 0) ^ getLongOffset((unsigned char *)a, 8));
//    printf("md5long by *a: %lld\n", md5long);
//    // diff a and hex was in 0123456789abcdef or 0123456789ABCDEF in bin2hex function
//    free(hex);
//    free(bin);
//    return 0;

//    //--------- test hex2bin
//    unsigned char digest[] = "1bc29b36f623ba82aaf6724fd3b16718";
//    const char    *a = "1bc29b36f623ba82aaf6724fd3b16718";
//    char          *hex;
//    unsigned char *bin;
//    size_t         binlen;
//
//    binlen = hexs2bin(a, &bin);
//    printf("bin = %.*s\n", (int)binlen, (char *)bin);
//
//    printf("binlen = %d\n", binlen);
//
//    hex = bin2hex(bin, binlen);
//    printf("hex = %s\n", hex);
//
//    free(bin);
//    free(hex);
//    return 0;


//    //--------- test bin2hex
//    const char    *a = "1bc29b36f623ba82aaf6724fd3b16718";
//    char          *hex;
//    unsigned char *bin;
//    size_t         binlen;
//
//    hex = bin2hex((unsigned char *)a, strlen(a));
//    printf("%s\n", hex);
//
//    binlen = hexs2bin(hex, &bin);
//    printf("%.*s\n", (int)binlen, (char *)bin);
//
//    free(bin);
//    free(hex);
//    return 0;


    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    const char* data = "Callback function called";

    // both working. dll must be in the exe-file folder
//    rc = sqlite3_open("v:\\Projects\\_C_projects\\sqlite3_ext_md5\\test.db", &db);
    rc = sqlite3_open("..\\test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        printf("Opened database successfully\n");
    }

    sqlite3_enable_load_extension(db, 1);
    // both working. dll must be in the exe-file folder or in System32
//    sqlite3_load_extension(db,"v:\\Projects\\_C_projects\\sqlite3_ext_md5\\cmake-build-debug\\sqlite3_mod_extfunc3.dll","sqlite3_extension_init", 0);
//    rc = sqlite3_load_extension(db,"..\\sqlite3_mod_extfunc3.dll","sqlite3_extension_init",0);
    rc = sqlite3_load_extension(db,"..\\libsqlite3_ext_md5.dll","sqlite3_extension_init",0);

    /* Create SQL statement */
//    sql = "SELECT s_Step, median(l_id), md5long(s_Step) from t_Steps";
//    sql = "SELECT s_Step, hex(md5('md5')), hex(md5long('md5')) from t_Steps";
//    sql = "SELECT s_Step, hex(md5('md5')), hex2long(s_Step), hex2long(lower(hex(md5('md5')))), md5long('md5') from t_Steps";
    sql = "SELECT hex2long(s_Step) from t_Steps";
    sql = "SELECT md5long(s_Step) FROM t_Steps;";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);




//    sqlite3 *tmpDB;
//    sqlite3_open_v2(":memory", &tmpDB, SQLITE_OPEN_READWRITE, 0);
//    sqlite3_stmt *tmpStmt = nullptr;
//
//    sqlite3_prepare_v2(tmpDB, "SELECT ?;", 9, &tmpStmt, nullptr);
//    sqlite3_bind_text(tmpStmt, 0, "paul", 4, SQLITE_TRANSIENT);
//    int res = sqlite3_step(tmpStmt);
//
//    std::string s(reinterpret_cast<const char*>(sqlite3_value_text(sqlite3_column_value(tmpStmt, 0))));
//    sqlite3_finalize(tmpStmt);




    //    cout << md5direct("Hello World\n");
//    std::cin.get();  //Для того, чтобы сразу не закрывалось
    return 0;
}