//
//  main.c
//  store_graph.c
//
//  Created by 庞祺 on 2017/3/4.
//  Copyright © 2017年 panchy. All rights reserved.
//


#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#define max_length 1000

int check_type_error(char *subject_or_pre_or_object);
int check_string_match_error(char *subject_or_pre_or_object);
int check_symbol_error(char subject[],char predicate[],char object[]);
int check_prefix_error(char *subject_or_pre_or_object);

int check_lenghth(FILE *file);
char* get_full_url(char *subject_or_pre_or_object);
char* insert(char source[],char destination[],int length);
char* separate_before_colon(char *string,char *symbol);
char* separate_after_colon(char *string,char *symbol);
int StringFind(const char *pSrc, const char *pDst);
int check_line(char str[128], sqlite3 *db, sqlite3_stmt *stmt, char *ErrMsg);

int matchLetter(char * s);
int matchNum(char * s);


int check_prefix = 0;
int has_error=0;

int check_que = 0;
int count=0;

char final[max_length];



struct ARRAY_STRUCT{
    char prefix[20];
    char url[100];
}array_struct[200];




int main(int argc, char *argv[]) {
    int inputfile = 1;
    int outputfile = 2;
    FILE *input;
    FILE *output;
    FILE *input_check;
    
    memset(array_struct, 0, sizeof(struct ARRAY_STRUCT));
    
    input = fopen(argv[inputfile],"r");
    input_check = fopen(argv[inputfile],"r");
    
    if(input ==NULL)
    {
        printf("Input file \"%s\" does not exist \n", argv[inputfile]);
        return -1;
    }
    output = fopen(argv[outputfile],"w");
    
    
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *ErrMsg=0;
    
    int rc;
    
    rc=sqlite3_open(argv[outputfile],&db);
    if (rc) {
        fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
    }
    
    char *sql_store_graph="create table store_graph(subject text, prediction text, object text);";
    rc=sqlite3_exec(db, sql_store_graph, 0, 0, &ErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "new_airport Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    
    char str[128];
    
    int line_of_file=check_lenghth(input_check);
    for (int i=0; i<=line_of_file; i++) {
        if (fgets(str, 128, input)!=NULL){
            check_line(str,db,stmt,ErrMsg);
            if (has_error==1) {
                remove(argv[outputfile]);
                break;
            }else continue;
        }
    }
    

    fclose(input);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

int check_line(char str[128],sqlite3 *db,sqlite3_stmt *stmt,char *ErrMsg){
    
    
    int i=0;
    char b[100][100];

    char* delim="' '|'\t'|'\n'";
    char* p=strtok(str,delim);
    
    while(p!=NULL){
        strcpy(b[i++],p);
        p=strtok(NULL,delim);
    }
    int pa = i;
    
    
    if((strcmp(b[0], "@prefix")) == 0){
        
        
        check_prefix = 1;
        strcpy(array_struct[count].url, b[pa-2]);
        strcpy(array_struct[count].prefix, b[pa-3]);
        
        count ++;
        
    }
    
    
    if((strcmp(b[0], "@prefix")) != 0) {
        check_que = 1;
        char *subject = b[0];
        char *pre = b[1];
        char *object = b[2];
        
        char pref[max_length];
        char sub[max_length];
        char obj[max_length];
        
        
        
        if (check_prefix_error(subject)==-2 && check_symbol_error(subject,pre,object)==0) {
            
            has_error=1;
            printf("Can not find prefix: %s, Please check your file\n",separate_before_colon(subject,":"));
            
        }else if(check_symbol_error(subject,pre,object)==-1){
            has_error=1;
            printf("Missing subject, predicate or object\n");
            
        }else if(check_string_match_error(pre)==-3){
            has_error=1;
            printf("Missing \" for string, Please check your file\n");
        }
        else if(check_string_match_error(object)==-3){
            has_error=1;
            printf("Missing \" for string, Please check your file\n");
            
        }else if(check_type_error(object)==-4){
            has_error=1;
            printf("Unsatisfy date type, Please check your file\n");
        }
        else{
            strcpy(sub,get_full_url(subject));
            printf("sub = %s\n", sub);
        
            strcpy(pref,get_full_url(pre));
            printf("pre = %s\n", pref);
        
            printf("obj=%s\n",get_full_url(object));
            strcpy(obj,get_full_url(object));
            //printf("obj=%s\n",obj);
        
        
            char* sql = sqlite3_mprintf("INSERT INTO store_graph VALUES('%s','%s','%s');",sub,pref,obj);
            sqlite3_exec(db, sql, 0, 0, &ErrMsg);
            sqlite3_free(sql);
            printf("store success!\n");
        }
    }
    return 0;
    
    
}
//check type error
int check_type_error(char *subject_or_pre_or_object){
    char *str=NULL;
    char *data=NULL;
    
    
    printf("subji=%s\n",subject_or_pre_or_object);
    if (StringFind(subject_or_pre_or_object,"xsd")!=-1) {
        
        str=separate_after_colon(subject_or_pre_or_object,":");
        
        if (strcmp(str,"string")==0) {
            data=separate_before_colon(subject_or_pre_or_object,"^^");
            printf("data=%s\n",data);
            if (StringFind(data,"\"")!=-1) {
                return 0;
            }else return -4;

        }else if(strcmp(str,"integer")==0){
            data=separate_before_colon(subject_or_pre_or_object,"^^");
            if ((strlen(data)-2)==matchNum(data)) {
                return 0;
            }else return -4;

        }else if(strcmp(str,"float")==0){
            data=separate_before_colon(subject_or_pre_or_object,"^^");
            if ((strlen(data)-2)==(matchNum(data)+1) && StringFind(data,".")!=-1 ) {
                return 0;
            }else return -4;

        }else if(strcmp(str,"decimal")==0){
            return 0;
        }else if(strcmp(str,"date")==0){
            data=separate_before_colon(subject_or_pre_or_object,"^^");
            if (matchNum(data)==4 || matchNum(data)==8 ) {
                
                return 0;
            }else return -4;

        }
    }
    return 0;
}

//check \" error
int check_string_match_error(char *subject_or_pre_or_object){
    
    
    int che=0;
    
    for (int i=0; i<=strlen(subject_or_pre_or_object); i++) {
        if (subject_or_pre_or_object[i]=='"') {
            che++;
        }
    }
    if (che==2 || che==0 || che==4) {
        return 0;
    }else{
        return -3;
    }
    
    
//    int check_string_match=0;
//    char str[max_length];
//    //printf("subji=%s\n",subject_or_pre_or_object);
//    if (StringFind(subject_or_pre_or_object,"\"")!=-1) {
//        strcpy(str,separate_after_colon(subject_or_pre_or_object,"\""));
//        printf("str=%s\n",str);
//        if(StringFind(str,"\"")!=-1){
//            printf("stringfind=%d\n",StringFind(str,"\""));
//            return 0;
//        }else{
//            printf("stringfind=%d\n",StringFind(str,"\""));
//            return -3;
//        }
//    }else{
//        return 0;
//    }
    
}
//check , ; error
int check_symbol_error(char subject[],char predicate[],char object[]){

    if (strlen(subject)==1 || strlen(predicate)==1 || strlen(object)==1) {

        return -1;
    }else{
        return 0;
    }
}

//check prefix error
int check_prefix_error(char *subject_or_pre_or_object){
    int check_prefix=0;
    for (int i=0; i<=count; i++) {
        if (StringFind(subject_or_pre_or_object,"xsd")!=-1 || StringFind(array_struct[i].prefix,separate_before_colon(subject_or_pre_or_object,":"))!=-1) {
            check_prefix=1;
        }
    }
    if (check_prefix==0) {
        return -2;
    }else{
        return 0;
    }
}


//return string with full URL <http>
char* get_full_url(char *subject_or_pre_or_object){
    char *str=NULL;
    char *pref=NULL;
    int position,position_1;
    char temp2[100];
    //int check_prefix=0;
    
    //pref=separate_before_colon(subject_or_pre_or_object,":");//同时调用 会覆盖 pointer 地址
//    printf("count=%d",count);
    for (int i=0; i<=count; i++) {
        if (StringFind(subject_or_pre_or_object,"xsd")==-1 && StringFind(subject_or_pre_or_object,"\"")==-1 && StringFind(subject_or_pre_or_object,"<http")==-1) {
            pref=separate_before_colon(subject_or_pre_or_object,":");
        
            if (StringFind(array_struct[i].prefix, pref)!=-1 && StringFind(pref,"xsd")==-1) {
        
                position=StringFind(array_struct[i].url, ">");
                str=insert(array_struct[i].url,separate_after_colon(subject_or_pre_or_object,":"), position-1);
               
            }
        }else if(StringFind(array_struct[i].prefix, "xsd")!=-1 && StringFind(subject_or_pre_or_object,"xsd")!=-1){
        
            position=StringFind(subject_or_pre_or_object,"xsd");
    
            strcpy(temp2,insert(separate_before_colon(subject_or_pre_or_object,"xsd"),array_struct[i].url,position-1));
            //printf("pref=%s\n",temp2);
            position_1=StringFind(temp2, ">");
            
            str=insert(temp2,separate_after_colon(subject_or_pre_or_object,":"),position_1-1);
        
            return str;
            
        }else if(StringFind(subject_or_pre_or_object,"\"")!=-1 || StringFind(subject_or_pre_or_object,"<http")!=-1){
            str=subject_or_pre_or_object;
        }
            //str=subject_or_pre_or_object;
        
    }

    return str;
}

//return string separate before symbol
char* separate_before_colon(char *string,char *symbol){
    
    int position,length;
    char before[100]={0};
    char *str=NULL;

    position=StringFind(string,symbol);

    for (int i=0; i<position; i++) {
        before[i]=string[i];

    }

    str=before;
    return str;
}

//return string separate after colon
char* separate_after_colon(char *string,char *symbol){
    
    int position,length;
    char after[100]={0};
    char *str=NULL;
    
    position=StringFind(string,symbol);
    length=strlen(string)-position-1;
    for (int i=0;i<=length;i++) {
        after[i]=string[position+1];
        position++;
    }
    str=after;
    return str;
}


//insert destination into source string at position
char* insert(char source[],char destination[],int length)
{
    
    char temp[max_length];
    char buf[max_length];
    unsigned long ln;
    int temp2;
    char *first_part;

    memset(final,0,max_length*sizeof(char));
    memset(buf,0,max_length*sizeof(char));
    
    temp2=length;
    ln=strlen(source)-length;
    
    for(int i=0;i<=ln;i++){
        temp[i]=source[length+1];
        length++;
    }
    for (int i=0; i<=temp2; i++) {
        buf[i]=source[i];
    }
    first_part=strcat(buf,destination);
    strcpy(final,strcat(first_part,temp));
    
    return final;
}

//find string in string, return the first start location or -1 if can not find
int StringFind(const char *pSrc, const char *pDst)
{
    int i, j;
    for (i=0; pSrc[i]!='\0'; i++)
    {
        if(pSrc[i]!=pDst[0])
            continue;
        j = 0;
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')
        {
            j++;
            if(pDst[j]!=pSrc[i+j])
                break;
        }
        if(pDst[j]=='\0')
            return i;
    }
    return -1;
}
//return the lins of file
int check_lenghth(FILE *file){
    
    int line=0;
    char ch;
    while(!feof(file))
        if((ch=fgetc(file))=='\n')line++;
    return line;
    
}
// return the number of letters
int matchLetter(char * s)
{
    int number=0;
    for (int i=0; i<=strlen(s);i++ ) {
        if ((s[i]>='a' && s[i]<='z')||(s[i]='A' && s[i]<='Z')) {
            number++;
        }
    }
    return number;
    
}

//return the number of interge
int matchNum(char * s)
{
    int number=0;
    for (int i=0; i<=strlen(s);i++ ) {
        if (s[i]>=48 && s[i]<=57) {
            number++;
        }
    }
    return number;
}
