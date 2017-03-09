#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
/* This function separates record when on record of routes have more than one equipments.
 * for example if we have airline_id=123 and equipments="456 789", it will become 123,456 and 123,789 in database.
 */
int main(int argc, char **argv){
    sqlite3 *db; //the database
    sqlite3_stmt *stmt; //the update statement
    int rc;
    
    char *zErrMsg = NULL;
    
    rc = sqlite3_open("openflights.sql", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    char *sql = "CREATE TABLE newRoutes(Airline text, Airline_ID int, Source_airport text, Source_airport_ID int, Destination_airport text, Destination_airport_ID int, Codeshare text, Stops int, Equipment text);";
    sqlite3_exec(db,sql,0,0,&zErrMsg);
    
    
    char *sql_stmt2 = "select * from routes;";
    rc = sqlite3_prepare_v2(db, sql_stmt2, -1, &stmt, 0);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        int cut = 0;
        int i=0;
        int col;
        char b[10][4];
        
        for(col = 0; col<sqlite3_column_count(stmt)-1; col++){}
        const unsigned char *check_length =sqlite3_column_text(stmt, col);
        int length = strlen(check_length);
        if(length < 6){
            const unsigned char *Airline = sqlite3_column_text(stmt, 0);
            int Airline_ID = sqlite3_column_int(stmt, 1);
            
            const unsigned char *source_airport = sqlite3_column_text(stmt, 2);
            int source_airport_ID = sqlite3_column_int(stmt, 3);
            const unsigned char *destination_airport = sqlite3_column_text(stmt, 4);
            int destination_airport_ID = sqlite3_column_int(stmt, 5);
            const unsigned char *codeshare = sqlite3_column_text(stmt, 6);
            int stops = sqlite3_column_int(stmt, 7);
            const unsigned char *eq = sqlite3_column_text(stmt, 8);
            if(Airline_ID <= 0){
                sql = sqlite3_mprintf("INSERT INTO newRoutes VALUES('%s','\\N','%s',%d,'%s',%d,'%s',%d,'%s');", Airline,source_airport,source_airport_ID,destination_airport,destination_airport_ID,codeshare,stops,eq);
                
                sqlite3_exec(db, sql, 0, 0, &zErrMsg);
                sqlite3_free(sql);
            }
            
            else{
                sql = sqlite3_mprintf("INSERT INTO newRoutes VALUES('%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s');", Airline, Airline_ID,source_airport,source_airport_ID,destination_airport,destination_airport_ID,codeshare,stops,eq);
                
                sqlite3_exec(db, sql, 0, 0, &zErrMsg);
                sqlite3_free(sql);
            }
        }
        
        if(length>6){
            char *p = strtok(sqlite3_column_text(stmt, col)," ");
            strcpy(b[i++],p);
            p = strtok(NULL," ");
            while(p){
                strcpy(b[i++],p);
                p = strtok(NULL," ");
                cut++;
            }
            
            for(;cut>=0;cut--){
                const unsigned char *Airline = sqlite3_column_text(stmt, 0);
                int Airline_ID = sqlite3_column_int(stmt, 1);
                const unsigned char *source_airport = sqlite3_column_text(stmt, 2);
                int source_airport_ID = sqlite3_column_int(stmt, 3);
                const unsigned char *destination_airport = sqlite3_column_text(stmt, 4);
                int destination_airport_ID = sqlite3_column_int(stmt, 5);
                const unsigned char *codeshare = sqlite3_column_text(stmt, 6);
                int stops = sqlite3_column_int(stmt, 7);
                char *eq = b[cut];
                
                if(Airline_ID <=0){
                    sql = sqlite3_mprintf("INSERT INTO newRoutes VALUES('%s','\\N','%s',%d,'%s',%d,'%s',%d,'%s');", Airline,source_airport,source_airport_ID,destination_airport,destination_airport_ID,codeshare,stops,eq);
                    
                    sqlite3_exec(db, sql, 0, 0, &zErrMsg);
                    sqlite3_free(sql);
                    
                }
                else{
                    
                    sql = sqlite3_mprintf("INSERT INTO newRoutes VALUES('%s',%d,'%s',%d,'%s',%d,'%s',%d,'%s');", Airline, Airline_ID,source_airport,source_airport_ID,destination_airport,destination_airport_ID,codeshare,stops,eq);
                    
                    sqlite3_exec(db, sql, 0, 0, &zErrMsg);
                    sqlite3_free(sql);
                }
            }
        }
    }
    
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
