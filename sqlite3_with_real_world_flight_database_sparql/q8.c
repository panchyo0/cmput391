#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

void print_result(sqlite3_stmt *stmt);

int main(int argc, char **argv){
    sqlite3 *db; 
    sqlite3_stmt    *stmt_q;
    int rc;
    
    rc = sqlite3_open("openflights.sql", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    char *sql_qry ="Select distinct a1.name\
    From airports a1,airports a2, airlines air, routes r\
    Where air.airline_ID = r.airline_ID AND (air.IATA <> '/N' OR air.IATA <> '' OR air.ICAO <> '/N' OR air.ICAO <> '' OR air.Callsign <> '/N' OR air.Callsign <> '' OR air.country <> '/N' OR air.country <> '') AND  r.destination_airport_ID = a1.airport_ID and r.source_airport_ID = a2.airport_ID\
    except\
    select * from(\
    with recursive influenced (Destination_airport) as(\
    select Destination_airport\
    from routes\
    where Source_Airport=? \
    union\
    select r.Destination_airport\
    from influenced infl,routes r\
    where infl.Destination_airport=r.Source_Airport)\
    select name from influenced,airports\
    where IATA=Destination_airport);";
    
    rc = sqlite3_prepare_v2(db, sql_qry, -1, &stmt_q, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    /* iterate through different ids */
    char input_id[10];
    while(input_id[0] != 'q'){
        printf("enter id: ");
        fgets(input_id, 100, stdin);
        
        sqlite3_bind_text(stmt_q, 1,input_id,3,NULL);
        
        printf("****************************************************************\n");
        print_result(stmt_q);
        printf("****************************************************************\n");
        
        sqlite3_reset(stmt_q);
        sqlite3_clear_bindings(stmt_q);
        
    };
    
    sqlite3_finalize(stmt_q);
    
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
