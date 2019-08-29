#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SIZEHASH 10000
#define SIZETYPEREL 200
#define SIZEDEST 10000
#define SIZEORIG 10000

typedef struct origEnt {
    struct entity *name;
    struct origEnt *next;
}origEnt;

typedef struct headReport{                                 // entity in hashmap
    int n_rel;
    struct entity *name;
    origEnt *first_orig;
    struct headReport *next;
}headReport;

typedef struct ent{                                 // entity in hashmap
    char name[1024];
    headReport *head_list;
    origEnt *orig_list;
    struct ent *next;
}entity;

typedef struct typeRel{                             //tipi relazioni
    char id_rel[1024];
    headReport *rep_list;
    struct typeRel *next;
}typeRel;


entity *hash[SIZEHASH] = {NULL};
typeRel *array_lex[78] = {NULL};
entity *cacheEnt = NULL;


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

void findInHash(char username[],entity **p) {
    int pos;
    pos = hashfunc(username);
    (*p) = hash[pos];
    if ((*p)!= NULL){
        while ((*p)->next!=NULL && strcmp((*p)->name,username)!=0){
            (*p) = (*p)->next;
        }
        if(strcmp((*p)->name,username)!=0) {
            (*p) = NULL;
        }
    }
}

void findRel(char id_rel[], typeRel **p) {
    int pos=(int)id_rel[0]- 45;
    (*p)=array_lex[pos];
    if ((*p)!= NULL){
        while ((*p)->next!=NULL && strcmp((*p)->id_rel,id_rel)<0){
            (*p) = (*p)->next;
        }
        if(strcmp((*p)->id_rel,id_rel)!=0) {
            (*p) = NULL;
        }
    }
}

void addent(){
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

    if (flagpoint == NULL) {
        if(cacheEnt == NULL) {
            item = malloc(sizeof(entity)); //allocate memory for entity-struct
        }else {
            item = cacheEnt;
            //if (item->next != NULL) {
            cacheEnt = item->next;
            //}else cacheEnt = NULL;
        }
        strcpy(item->name,username);
        item->head_list = NULL;
        item->orig_list = NULL;
        item->next = NULL;

        hash[pos] = item;
        printf("aggiunta %s\n",username);
    }else if(flagpoint->next == NULL && strcmp(username,flagpoint->name)!= 0) {
        if(cacheEnt == NULL) {
            item = malloc(sizeof(entity)); //allocate memory for entity-struct
        }else {
            item = cacheEnt;
            //if (item->next != NULL) {
            cacheEnt = item->next;
            //}else cacheEnt = NULL;
        }
        strcpy(item->name,username);
        item->head_list = NULL;
        item->orig_list = NULL;
        item->next = NULL;

        flagpoint->next = item;
        printf("aggiunta %s\n",username);
    }else printf("presente %s\n",username);
}

void addrel() {}

void delent(){}

void delrel() {}

void report(){}

void end(){
    printf("end\n");
}

int main() {
    char action[10];
    int count = 0;
    entity *pKA;
    typeRel *p;
    do{
        scanf("%s", action);
        //count++;
        //printf("%d)\n",count);
        /*if(count == 277) {
            findInHash("Jonathan_Archer",&pKA);
            if(pKA!= NULL) {
                for (int i = 0; i < 5; i++) {
                    printf("name: %s rel:", pKA->odest[i].name);
                    for (int j = 0; j < pKA->odest[i].len_array; j++) {
                        printf(" %s &", pKA->odest[i].rel[j].id);
                    }
                    printf("\n");
                }
            }
        }*/
        if(action[0]=='a') {

            if(action[3]=='e') {
                addent();
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
        /*if(count == 217) {
            for (int i = 0; i < 78; i++) {
                p = array_lex[i];
                while (p != NULL) {
                    printf("%s\n", p->id_rel);
                    for (int j = 0; j < p->len_array; j++) {
                        printf("%d %s %d\n", j, p->dest[j].name, p->dest[j].n_rel);
                    }
                    p = p->next;
                }
            }
        }*/
    } while(action[0]!='e');


    printf("end of file\n");
}