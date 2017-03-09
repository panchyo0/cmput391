#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/* This is a function to normaliz our database(openflighst.sql).After excution, we will get a database call
 * normalized.sql which will satisfly 1NF,2NF and 3NF. The way we do that will describle in readMe.
 */

void print_result(sqlite3_stmt *stmt);

int main(int argc, char **argv){
    sqlite3 *db2; //the database
    sqlite3_stmt    *stmt_q;
    char *ErrMsg=0;
    int rc;
    
    rc=sqlite3_open("normalized.sql", &db2);
    if( rc){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return(1);
    }
    
    
    char *sql_new_airport="create table airport(Airport_ID int,Name text,City text,Country text,IATA text,ICAO text,Latitude double,Longitude double,Altitude double,Timezone double,DST text,Tz_database_time_zone text,primary key(Airport_ID));";
    rc=sqlite3_exec(db2, sql_new_airport, 0, 0, &ErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "new_airport Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    char *sql_airline="create table airline( Airline_ID int,Name text,Alias text,IATA text,ICAO text,Callsign text,Country text,Active text,primary key(Airline_ID));";
    rc=sqlite3_exec(db2, sql_airline, 0, 0, &ErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "airline Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    
    char *sql_route="create table route(Airline_ID int,Source_airport text,Destination_airport text,Codeshare text,Stops int,Equipment text,FOREIGN KEY (Airline_ID) REFERENCES airline(Airline_ID));";
    rc=sqlite3_exec(db2, sql_route, 0, 0, &ErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "route Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    char *sql_UnAirline="create table unRecordAirline(Airline text,Airline_ID text,Source_airport_ID int,Destination_airport_ID int,Codeshare text,Stops int,Equipment text, FOREIGN KEY (Source_airport_ID) REFERENCES airport(Airline_ID));";
    rc=sqlite3_exec(db2, sql_UnAirline, 0, 0, &ErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "UnAirline Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }

    char *sql_attach="attach database 'openflights.sql' as source;";
    rc=sqlite3_exec(db2, sql_attach, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "attach Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    char *sql_trans_R="insert into route select airline_id,Source_airport,Destination_airport,Codeshare,Stops ,Equipment from source.newRoutes;";
    rc=sqlite3_exec(db2,sql_trans_R, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "trans_R Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    char *sql_trans_airport="insert into airport select Airport_ID ,Name ,City ,Country ,IATA ,ICAO,Latitude,Longitude,Altitude,Timezone,DST ,Tz_database_time_zone from source.airports;";
    rc=sqlite3_exec(db2,sql_trans_airport, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "trans_airport Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    char *sql_trans_airline="insert into airline select air.Airline_ID ,air.Name ,air.Alias ,air.IATA ,air.ICAO ,air.Callsign ,air.Country ,air.Active from source.airlines air;";
    
    
    rc=sqlite3_exec(db2,sql_trans_airline, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "trans_airline Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    char *sql_trans_Unairline="insert into unRecordAirline select Airline,Airline_ID ,Source_airport_ID,Destination_airport_ID,Codeshare ,Stops ,Equipment  from source.newRoutes r where r.Airline_ID='\\N';";
    
    
    rc=sqlite3_exec(db2,sql_trans_Unairline, 0, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "trans_Unairline Preparation failed: %s\n", sqlite3_errmsg(db2));
        sqlite3_close(db2);
        return 1;
    }
    
    
    
    sqlite3_finalize(stmt_q);
    sqlite3_close(db2);
    return 0;
}


