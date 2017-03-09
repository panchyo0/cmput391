#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char **argv){
    sqlite3 *db; //the database
    sqlite3_stmt *stmt; //the update statement
    
    int rc;
    
    rc = sqlite3_open("openflights.sql", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    char *sql_stmt = "select distinct name from (SELECT a.name, count(r.airline_ID) FROM airports a, routes r, airports a2 Where (r.source_airport_ID = a.Airport_ID and a.country like '%Canada%') and (r.source_airport_ID = a2.airport_ID and r.destination_airport_ID = a.airport_ID And a.country <> a2.country) Group by a.name Union SELECT a.name, count(r.airline_ID) FROM airports a, routes r, airports a2 where r.destination_airport_ID = a.Airport_ID and a.country like '%Canada%' and (r.source_airport_ID = a2.airport_ID and r.destination_airport_ID = a.airport_ID And a.country <> a2.country) Group by a.name Order by count(r.airline_ID) desc) Limit 10";
    
    
    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        int col;
        for(col = 0; col<sqlite3_column_count(stmt)-1; col++){
            printf("%s|", sqlite3_column_text(stmt, col));
        }
        printf("%s", sqlite3_column_text(stmt, col));
        printf("\n");
    }
    sqlite3_finalize(stmt); //always finalize a statement
}

