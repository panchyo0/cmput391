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
    
    char *sql_qry ="select r.Source_airport,r.Destination_airport from airlines a1,routes r where a1.active='Y'and a1.Airline_ID=r.Airline_ID and (a1.IATA <> '/N' or a1.ICAO <> '/N' or a1.Callsign <> '/N' or a1.country <> '/N') except select r1.Source_airport,r1.Destination_airport from airlines a,routes r1,routes r2 where r1.Source_airport_ID== r2.Destination_airport_ID and a.active='Y'and a.Airline_ID=r1.Airline_ID and a.Airline_ID=r2.Airline_ID and r1.Destination_airport_ID ==r2.Source_airport_ID except select r1.Source_airport,r1.Destination_airport from routes r1,airports ap1,airports ap2 where r1.Source_airport_ID=ap1.Airport_ID and r1.Destination_airport_ID=ap2.Airport_ID and ap1.country==ap2.country;";
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
