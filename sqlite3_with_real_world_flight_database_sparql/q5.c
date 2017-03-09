#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#define EARTH_RADIUS 6371.009
#define CONVERT (3.1415926 / 180)

void length_function(sqlite3_context* ctx, int nargs, sqlite3_value** values);

int main(int argc, char **argv){
    sqlite3 *db; //the database
    sqlite3_stmt *stmt; //the update statement
    int rc;
    char b[100][4];
    int i=0;
    
    rc = sqlite3_open("lab1.sql", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    sqlite3_create_function( db, "length_function", 4, SQLITE_UTF8, NULL, length_function, NULL, NULL);
    
    char *sql_stmt = "SELECT DISTINCT Equipment FROM( \
    SELECT DISTINCT r.Equipment, length_function(a1.Latitude, a1.Longitude, a2.Latitude, a2.Longitude) as length \
    FROM Airports a1, Airports a2, routes r, Airlines air \
    WHERE air.active = 'Y' AND a1.Airport_ID = r.Source_Airport_ID AND a2.Airport_ID = r.destination_airport_ID AND r.airline_ID = air.airline_ID \
    AND (air.IATA <> '/N' OR air.IATA <> '' OR air.ICAO <> '/N' OR air.ICAO <> '' OR air.Callsign <> '/N' OR air.Callsign <> '' OR air.country <> '/N' OR air.country <> '') AND length > 10000 \
    ORDER BY length) \
    GROUP BY Equipment;";
    
    /* char *sql_stmt = "select distinct r.Equipment \
     FROM Airports a1, Airports a2, routes r, airlines air \
     WHERE a1.Airport_ID = r.Source_Airport_ID AND a2.Airport_ID = r.destination_airport_ID and length_function(a1.Latitude, a1.Longitude, a2.Latitude, a2.Longitude)>10000 AND air.active = 'Y' AND air.airline_ID = r.airline_ID \
     group by r.Equipment;";*/
    
    
    
    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        int col;
        char *p = strtok(sqlite3_column_text(stmt, col)," ");
        strcpy(b[i++],p);
        p = strtok(NULL," ");
        
        while(p){
            strcpy(b[i++],p);
            p = strtok(NULL," ");
            
        }
    }
    
    int duplicate = 0;
    for(int j=0;j<i;j++){
        for(int x=j+1;x<i;x++){
            if(strcmp(b[j],b[x]) == 0) duplicate = 1;
        }
        if(duplicate == 0){
            printf("%s\n",b[j]);
            duplicate = 0;
        }
        duplicate = 0;
    }
    sqlite3_finalize(stmt); //always finalize a statement
    sqlite3_close(db);
    return 0;
    
    
}

// length_function reference by "https://rosettacode.org/wiki/Haversine_formula#C"
void length_function(sqlite3_context* ctx, int nargs, sqlite3_value** values){
    double dx, dy, dz;
    double source_latitude = sqlite3_value_double(values[0]);
    double source_longitude = sqlite3_value_double(values[1]);
    double destination_latitude = sqlite3_value_double(values[2]);
    double destination_longitude = sqlite3_value_double(values[3]);
    
    source_longitude -= destination_longitude;
    source_longitude *= CONVERT;
    source_latitude *= CONVERT;
    destination_latitude *= CONVERT;
    
    dx = cos(source_longitude) * cos(source_latitude) - cos(destination_latitude);
    dy = sin(source_longitude) * cos(source_latitude);
    dz = sin(source_latitude) - sin(destination_latitude);
    
    double length = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * EARTH_RADIUS;
    sqlite3_result_double(ctx,length);
    
}

