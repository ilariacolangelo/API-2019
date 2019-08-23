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
typeRel *array_lex[78] = {NULL};
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

void findInHash(char username[],entity **p) {
    int pos;
    pos = hashfunc(username);
    *p = hash[pos];
    if (*p!= NULL){
        while (*p->next!=NULL && strcmp(*p->name,username)!=0){
            *p = *p->next;
        }
        if(strcmp(*p->name,username)!=0) {
            *p = NULL;
        }
    }
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
        item->len_array=0;
        item->next = NULL;

        if(flagpoint == NULL) hash[pos] = item;
        else flagpoint->next = item;

        printf("Entity:%s addent ok\n",hash[pos]->name);
    }else printf("Entity already tracked\n");
}

void addrel(){
    int pos,i,j,w,x,y;
    int exist=0;
    char orig[1024];
    char dest[1024];
    char rel[1024];
    typeRel *item;
    typeRel *pointer;
    typeRel *prec = NULL;
    entity *p_orig = NULL;
    entity *p_dest = NULL;
    read(orig);
    read(dest);
    read(rel);

    findInHash(orig,&p_orig);
    findInHash(dest,&p_dest);

    if (p_dest !=NULL && p_orig!=NULL) {

        pos = (int) rel[0] - 45; //ASCII value of first char admitted is 45
        pointer = array_lex[pos];

        while (pointer != NULL && pointer->next != NULL && strcmp(pointer->id_rel, rel) < 0) {
            prec = pointer;
            pointer = pointer->next;
        }

        //check in hash of entity if this rel already exists
        //update hash ent
        for(x=0; x<p_orig->len_array && strcmp(p_orig->orig[x].name,dest)!=0;x++){}
        if(strcmp(p_orig->orig[x].name,dest)==0){
            for(y=0; y<p_orig->orig[x].len_array && strcmp(p_orig->orig[x].rel[y].id,rel)!=0;y++){}

            if(strcmp(p_orig->orig[x].rel[y].id,rel)==0) {
                exist=1;
            }else {//add new rel to orig[x]
                strcpy(p_orig->orig[x].rel[y].id,rel);
                p_orig->orig[x].len_array++;
            }
        }else {//add new entity to orig[]
            strcpy(p_orig->orig[x].name,dest);
            p_orig->len_array++;
            strcpy(p_orig->orig[x].rel[0].id,rel);
            p_orig->orig[x].len_array++;
        }

        if (pointer != NULL && strcmp(pointer->id_rel, rel) == 0) { //modify structure
            if (exist == 1) {
                printf("relazione già esistente\n");
            }else {
                //modifica array dest
                for (i = 0; i < pointer->len_array && strcmp(pointer->dest[i].name, dest) != 0; i++) {} //find dest in array in typerel
                if (strcmp(pointer->dest[i].name, dest) == 0) {// dest found
                    pointer->dest[i].n_rel++;
                } else {
                    strcpy(pointer->dest[i].name, dest);
                    pointer->dest[i].n_rel = 1;
                    pointer->len_array++;
                }

                //ordina array dest
                j = i;
                while (j >= 0 && (pointer->dest[j--].n_rel < pointer->dest[i].n_rel || (pointer->dest[j--].n_rel == pointer->dest[i].n_rel && strcmp(pointer->dest[j--].name, pointer->dest[i].name) > 0))) {
                    j--;
                }

                ent_dest temp;
                temp.n_rel = pointer->dest[i].n_rel;
                strcpy(temp.name, pointer->dest[i].name);

                for (w = i; w > j + 1; w--) {
                    pointer->dest[w] = pointer->dest[w - 1];
                }
                pointer->dest[w] = temp;
            }

        } else { //create new typeRel
            printf("NUOVA: %s\n", rel);      //add new typerel

            item = malloc(sizeof(typeRel));

            strcpy(item->id_rel, rel);
            item->len_array = 1;
            strcpy(item->dest[0].name, dest);
            item->dest[0].n_rel = 1;
            item->next = NULL;
            //add this rel in entity list orig

            if (prec == NULL) array_lex[pos] = item;
            else {
                if (strcmp(pointer->id_rel, rel) > 0) {item->next = pointer;}
                pointer->next = item;
            }
            printf("aggiunto nuovo typeRel\n");
        }
    }else printf("entità non monitorate\n");

}

void delent(){
    int i,j,z;
    int pos;
    entity *prec_u=NULL;
    entity *user;
    typeRel *pointer;
    char username[1024];
    read(username);

    pos = hashfunc(username);
    user = hash[pos];
    if (user!= NULL) {
        while (user->next != NULL && strcmp(user->name, username) != 0) {
            prec_u = user;
            user = user->next;
        }
        if (strcmp(user->name, username) == 0) {
            if(prec_u != NULL){
                if(user->next!=NULL) {
                    prec_u->next=user->next;
                    user->next = NULL;
                }else  prec_u->next=NULL;
            }else {
                if (user->next != NULL) {
                   hash[pos]=user->next;
                }else hash[pos]=NULL;
            }

            if(cacheEnt!=NULL)user->next=cacheEnt;
            cacheEnt = user;
            user = NULL;

            for (i = 0; i < 78; i++) {
                pointer = array_lex[i];
                while (pointer != NULL) {
                    for (j = 0; j < pointer->len_array && strcmp(pointer->dest[j].name, username) != 0; j++) {}
                    if (strcmp(pointer->dest[j].name, username) == 0) {
                        pointer->len_array--;
                        for (z = j; z < pointer->len_array; z++) {
                            pointer->dest[z] = pointer->dest[z + 1];
                        }
                    }
                    pointer = pointer->next;
                }
            }
        }else printf("entità non monitorata\n");
    }else printf("entità non monitorata\n");
}

void delrel(){
    printf("delrel\n");
}

void report(){
    int i,j;
    typeRel *pointer;
    int flag_space=0;

    for (i = 0; i < 78; i++) {
        pointer = array_lex[i];
        while (pointer != NULL) {
            if(pointer->len_array>0) {
                if(flag_space!=0){
                    printf(" ");
                }
                flag_space=1;
                printf("\"%s\" \"%s\"", pointer->id_rel, pointer->dest[0].name);
                for (j = 0; j < pointer->len_array && pointer->dest[j + 1].n_rel == pointer->dest[j].n_rel; j++) {
                    printf(" \"%s\"", pointer->dest[j + 1].name);
                }
                printf(" %d;", pointer->dest[0].n_rel);
            }
            pointer = pointer->next;
        }
    }
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