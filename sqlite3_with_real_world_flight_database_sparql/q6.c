#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

void print_result(sqlite3_stmt *stmt);

int main (int argc,char **argv){
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    int rc;
    
    rc=sqlite3_open("openflights.sql",&db);
    if (rc) {
        fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    char *sql_qry ="with recursive influenced (Destination_airport) as(\
    select Destination_airport\
    from routes\
    where Source_Airport='YEG'\
    union\
    select r.Destination_airport\
    from influenced infl,routes r\
    where infl.Destination_airport=r.Source_Airport)\
    select Destination_airport,name,Altitude from influenced,airports\
    where IATA=Destination_airport\
    ORDER BY Altitude desc\
    LIMIT 1;";
    rc=sqlite3_prepare_v2(db, sql_qry, -1, &stmt, 0);
    if (rc!=SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    print_result(stmt);
    sqlite3_finalize(stmt); //always finalize a statement
    
    sqlite3_close(db);
    return 0;
    
    
}

void print_result(sqlite3_stmt *stmt){
    int rc;
    
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int col;
        for(col=0; col<sqlite3_column_count(stmt)-1; col++) {
            printf("%s|", sqlite3_column_text(stmt, col));
        }
        printf("%s", sqlite3_column_text(stmt, col));
        printf("\n");
    }
}
