//#include <stdio.h>

#include <sqlite3.h>
#include <md5.h>

#include <iostream>				// в Си: #include <stdio.h>
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
//    unsigned char digest[] = "1bc29b36f623ba82aaf6724fd3b16718";
//    long long md5long;
//    md5long = (getLongOffset(digest, 0) ^ getLongOffset(digest, 8));
////    std::cout << (md5long);
//    printf("md5long: %lld", md5long);




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
    sql = "SELECT s_Step, hex(md5('md5')), md5long('md5') from t_Steps";

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