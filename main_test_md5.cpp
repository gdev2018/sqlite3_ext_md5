#include <stdio.h>
#include <iostream>
#include <sqlite3.h>

#include "md5.h"


static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
}

int main(int argc, char *argv[]) {
//    int x, *p, **q;
//    x = 10;
//    p = &x;
//    q = &p; // указатель на указатель
//    printf ("%d", **q );
//    printf ("%d", *argv[0] );

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
        fprintf(stderr, "Opened database successfully\n");
    }



    /* Create SQL statement */
    sql = "SELECT s_Step, median(l_id) from t_Steps";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);



    int x = md5test(10);

    sqlite3_str *pAccum;
    pAccum = (sqlite3_str*)sqlite3_aggregate_context(context, 0);
    if( pAccum ){
        if( pAccum->accError==SQLITE_TOOBIG ){
            sqlite3_result_error_toobig(context);
        }else if( pAccum->accError==SQLITE_NOMEM ){
            sqlite3_result_error_nomem(context);
        }else{
            const char *zText = sqlite3_str_value(pAccum);
            sqlite3_result_text(context, zText, -1, SQLITE_TRANSIENT);
        }
    }


    sqlite3_value *a = sqlite3_int64_value (1);
    sqlite3_value *b = sqlite3_str_value (0);
    sqlite3_value *c = sqlite3_str_new (db);

    sqlite3_value_free (a);
    sqlite3_value_free (b);
    sqlite3_value_free (c);


    char *a = "a";
    char *b = "a";
    char *c = "a";

//    input_char[]  = "md5";
    pVal = sqlite3ValueNew(0);
    sqlite3ValueSetStr(pVal, -1, zSql, SQLITE_UTF16NATIVE, SQLITE_STATIC);


    sqlite3_value *pVal = 0;        /* New value */
    struct ValueNewStat4Ctx *pCtx;   /* Second argument for valueNew() */
    pVal = valueNew(db, pCtx);

    unsigned char y =  md5direct(4, *input[]);
    printf("%d", y);


    //    cout << md5direct("Hello World\n");
    std::cin.get();  //Для того, чтобы сразу не закрывалось
    return 0;
}