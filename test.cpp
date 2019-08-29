#include <sqlite3.h>
#include <string>

#include <iostream>
//#include "md5.h"

using namespace std;

int main() {
    int *a = new int;


    sqlite3 *tmpDB;
    sqlite3_open_v2(":memory", &tmpDB, SQLITE_OPEN_READWRITE, 0);
    sqlite3_stmt *tmpStmt = nullptr;

    sqlite3_prepare_v2(tmpDB, "SELECT ?;", 9, &tmpStmt, nullptr);
    sqlite3_bind_text(tmpStmt, 0, "paul", 4, SQLITE_TRANSIENT);
    int res = sqlite3_step(tmpStmt);

    std::string s(reinterpret_cast<const char*>(sqlite3_value_text(sqlite3_column_value(tmpStmt, 0))));
    sqlite3_finalize(tmpStmt);

    //    cout << md5direct("Hello World\n");
    cin.get();  //Для того, чтобы сразу не закрывалось
    //return 0;
}
