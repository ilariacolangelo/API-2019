#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define ADDENT "addent"
#define DELENT "delent"
#define ADDREL "addrel"
#define DELREL "delrel"
#define REPORT "report"
#define END "end"

#define SIZEHASH 100
#define SIZETYPEREL 10
#define SIZEDEST 5
#define SIZEORIG 5

typedef struct rel { char id[1024]; }idRel;

typedef struct ent_orig {
    char name[1024];
    int len_array;
    idRel rel[SIZETYPEREL];
}ent_orig;

typedef struct ent_dest {
    char name[1024];
    int n_rel;
}ent_dest;

typedef struct ent{                                 // entity in hashmap
    char name[1024];
    int len_array;
    ent_orig orig[SIZEORIG];
    struct ent *next;
}entity;

typedef struct typeRel{                             //tipi relazioni
    char id_rel[1024];
    int len_array;
    ent_dest dest[SIZEDEST];
    struct typeRel *next;
}typeRel;


entity *hash[SIZEHASH] = {NULL};
typeRel *array_lex[64] = {NULL};
entity *cacheEnt = NULL;
typeRel *cacheRel = NULL;


int hashfunc(char username[]){                      //polynomial rolling hash function
    int len= strlen(username);
    int hashval=0;
    for(int i = 0;i<len;i++){
        hashval+=(int)username[i]*(long int)exp2(i);
    }
    return (int)hashval%SIZEHASH;
}

void read(char string[]){                           // scanf without " "
    char virgolette;
    int len;
    scanf("%c",&virgolette);
    scanf("%c",&virgolette);
    scanf("%s",string);
    len = strlen(string);
    string[len-1]='\0';
}

void addent(entity *hash[]){
    char username[1024];

    entity *item = NULL;
    entity *flagpoint = NULL;
    int pos;

    read(username);                          //scanf without " "

    pos = hashfunc(username);
    flagpoint = hash[pos];

    while(flagpoint!=NULL && flagpoint->next!=NULL && strcmp(username,flagpoint->name)!= 0) {
        flagpoint = flagpoint->next;
    }

    if(flagpoint== NULL || (flagpoint->next == NULL && strcmp(username,flagpoint->name)!= 0)) {
        if(cacheEnt == NULL) {
            item = malloc(sizeof(entity)); //allocate memory for entity-struct
        }else {
            item = cacheEnt;
            if (item->next != NULL) {
                cacheEnt = item->next;
            }else cacheEnt = NULL;
        }
        strcpy(item->name,username);
        item->next = NULL;

        if(flagpoint == NULL) hash[pos] = item;
        else flagpoint->next = item;

        printf("Entity:%s addent ok\n",hash[pos]->name);
    }else printf("Entity already tracked\n");
}

void addrel(){
    printf("addrel\n");
}

void delent(){
    printf("delent\n");
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

int main() {
    char action[10];

    do{

        scanf("%s", action);
        if(action[0]=='a') {

            if(action[3]=='e') {
                addent(hash);
            }else addrel();

        }else if(action[0]=='d') {

            if(action[3]=='e') {
                delent();
            }else delrel();

        }else if(action[0]=='r') {
            report();
        }else if(action[0]=='e') {
            end();
        }
    } while(action[0]!='e');

    printf("end of file\n");
}