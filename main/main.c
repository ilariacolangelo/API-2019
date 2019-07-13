#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ADDENT "addent"
#define DELENT "delent"
#define ADDREL "addrel"
#define DELREL "delrel"
#define REPORT "report"
#define END "end"

void addent();
void delent();
void addrel();
void delrel();
void report();
void end();

int main() {
    char action[10];
    //caso in cui ho file con solo END?
    do{

        scanf("%s", action);
        if(strcmp(action,ADDENT)==0){
            addent();
        }else if(strcmp(action,DELENT)==0){
            delent();
        }else if(strcmp(action,ADDREL)==0){
            addrel();
        }else if(strcmp(action,DELREL)==0){
            delrel();
        }else if(strcmp(action,REPORT)==0){
            report();
        }else if(strcmp(action,END)==0){
            end();
        }

    } while(strcmp(action,END)!=0);

    printf("end of file\n");
}


void addent(){
   printf("addent\n");
}

void delent(){
    printf("delent\n");
}

void addrel(){
    printf("addrel\n");
}

void delrel(){
    printf("delrel\n");
}

void report(){
    printf("report\n");
}

void end(){
    printf("end\n");
}