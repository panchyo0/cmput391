# h04_team9 lab1-challenge

Created by:
Qi Pang (qpang) &
Xin Huang (xhuang2)

In this task, we use our database which created for other 8 tasks. called 'openflights.sql' and it already exists in our update file.

Instructions:
1. run separate_equipments.c  (It takes time! please wait around 1.5 mins)
run: 

     gcc -g separate_equipments.c -o separate_equipments sqlite3.c -lpthread -ldl
     
     ./separate_equipments

2. run normaliz.c
run: 

     gcc -g normaliz.c -o normaliz sqlite3.c -lpthread -ldl
     
     ./normaliz


Description:
1. We fuond that in table routes, some of the equipments contains more than one equipment in one single line
we want to separate them. So we write a C program called separate_equipment.c. Such that after we run this function, every column will become a unseparateable data which will satisfy 1NF
EX: we have 2K,1338,BOG,2709,UIO,2688,,0,319 320, in equipment column we have 319 and 320 seems like we have a sub-table here. we want 
          
        2K,1338,BOG,2709,UIO,2688,,0,319     and
                            
        2K,1338,BOG,2709,UIO,2688,,0,320.
        
 we created another table in old database(openfligthts.sql), called newRoutes which contains the 'fixed' data'.

2. We need primary key and foreign key as constraints and reasonable relationship for each table to satisfy 2NF and 3NF.

3. We want to simplify the routes table, since we have airline_id, we do not need airline because the values are not unique in this table, and they match each other. which means we only need one of them. (if we have airline_ID we can find airline in airlines table). same idea for source_airport - source_airport_ID and destination_airport - destination_airport_ID. So we remove airline, source_airport and destination_airport.

4. In routes table, there are some airlines does not have Airline_ID and their IATA also does not in Airlines table. we assume these airlines are not registered. so we create a new table called unRecordAirline which contains these airlines. And we put source_airport_ID as a foreign key to connect airport table.
    Such that we separate 'two' kinds of airlines.
    In this table, since airline_ID are all \N, so we also keep Airline.

To sum up:
we create 4 tables:
1. airport:

    airport(Airport_ID int,
            Name text,
            City text,
            Country text,
            IATA text,
            ICAO text,
            Latitude double,
            Longitude double,
            Altitude double,
            Timezone double,
            DST text,
            Tz_database_time_zone text,
            primary key(Airport_ID)

2.airline:

    airline(Airline_ID int,
            Name text,
            Alias text,
            IATA text,
            ICAO text,
            Callsign text,
            Country text,
            Active text,
            primary key(Airline_ID))

3.route:

    route(Airline_ID int,
          Source_airport text,
          Destination_airport text,
          Codeshare text,
          Stops int,
          Equipment text,
          FOREIGN KEY (Airline_ID) REFERENCES airline(Airline_ID))

4.unRecordAirline:

    unRecordAirline(Airline text,
                    Airline_ID text,
                    Source_airport_ID int,
                    Destination_airport_ID int,
                    Codeshare text,
                    Stops int,
                    Equipment text, 
                    FOREIGN KEY (Source_airport_ID) REFERENCES airport(Airline_ID))

Then we get the data from old database(openflights.sql) and generate a new database called 'normalized.sql'








