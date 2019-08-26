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

typedef struct ent_odest {
    char name[1024];
    int len_array;
    idRel rel[SIZETYPEREL];
}ent_odest;

typedef struct ent_dest {
    char name[1024];
    int n_rel;
}ent_dest;

typedef struct ent{                                 // entity in hashmap
    char name[1024];
    int len_array;
    ent_odest odest[SIZEORIG];
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
        if(strcmp((*p)->name,username)!=0) {
            (*p) = NULL;
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
    char orig[1024];
    char dest[1024];
    char rel[1024];

    int pos;
    int cmpDest,cmpRel,cmpTRel;
    int i=0;
    int j=0;
    int z=0;
    int exist = 0;

    typeRel *prec=NULL;
    typeRel *pointer=NULL;
    typeRel *item=NULL;

    entity *p_orig = NULL;
    entity *p_dest = NULL;
    read(orig);
    read(dest);
    read(rel);

    findInHash(orig,&p_orig);
    findInHash(dest,&p_dest);

    if (p_dest !=NULL && p_orig!=NULL) {

        cmpDest = strcmp(p_orig->odest[i].name,dest);
        for(i=0; (i<p_orig->len_array && cmpDest!=0); i++) {
            cmpDest = strcmp(p_orig->odest[i].name,dest);
        }
        if(cmpDest == 0) {

            cmpRel = strcmp(p_orig->odest[i].rel[j].id,rel);
            for(j = 0; (j<p_orig->odest[i].len_array && cmpRel != 0);j++) {
                cmpRel = strcmp(p_orig->odest[i].rel[j].id,rel);
            }
            if(cmpRel == 0) {
                exist = 1;
                printf("relazione già esistente\n");
            }
        }else {
            strcpy(p_orig->odest[i].name,dest);
            p_orig->len_array++;
        }
        if(exist == 0) {
            strcpy(p_orig->odest[i].rel[j].id,rel);
            p_orig->odest[i].len_array++;

            //analyze typeRel
            printf("\nREL: %s\n",rel);
            pos = (int) rel[0] - 45; //ASCII value of first char admitted is 45
            pointer=array_lex[pos];

            while (pointer != NULL && pointer->next != NULL && strcmp(pointer->id_rel,rel)<0) {
                prec = pointer;
                pointer = pointer->next;
            }

            if(pointer == NULL || strcmp(pointer->id_rel,rel)!=0){
                //new
                item  = malloc(sizeof(typeRel));
                strcpy(item->id_rel,rel);
                item->len_array=1;
                strcpy(item->dest[0].name,dest);
                item->dest[0].n_rel=1;
                item->next=NULL;

                if (pointer == NULL) {
                    array_lex[pos]=item;
                }else {
                    if(strcmp(pointer->id_rel,rel)<0){ //aggiunta in coda
                        pointer->next = item;
                    }else {
                        item->next = pointer;
                        if(prec == NULL) { //aggiunta in testa
                            array_lex[pos]=item;
                        }else { //aggiunta nel mezzo
                            prec->next=item;
                        }
                    }
                }
                printf("nuova rel\n");
            }else {
                if(pointer->len_array>0){
                    cmpTRel = strcmp(pointer->dest[z].name,dest);
                    for(z=0;z<pointer->len_array && cmpTRel!=0;z++) {
                        cmpTRel = strcmp(pointer->dest[z].name,dest);
                    }
                    if(cmpTRel==0){
                        pointer->dest[z].n_rel++;
                    }
                }
                pointer->dest[z].n_rel=1;
                strcpy(pointer->dest[z].name,dest);
                pointer->len_array++;

                int index = z;
                while(index>0 && (pointer->dest[z].n_rel>pointer->dest[index-1].n_rel || (pointer->dest[z].n_rel==pointer->dest[index-1].n_rel && strcmp(pointer->dest[z].name,pointer->dest[index-1].name)<0))) {
                    index--;
                }
                ent_dest temp_ent = pointer->dest[z];
                while(z!=index){
                    pointer->dest[z]=pointer->dest[z-1];
                    z--;
                }
                pointer->dest[index]=temp_ent;

                printf("modificata rel\n");
            }

        }


}else printf("entità non monitorate\n");

}

void delent(){
    int i,j,z;
    int pos;
    int cmpUser;
    entity *prec_u=NULL;
    entity *user;
    typeRel *pointer;
    typeRel *thisRel
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

            for(int u=0;u<user->len_array;u++) {
                for (int w = 0; w<user->odest[u].len_array;w++) {
                    findRel(user->odest[u].rel[w].id,thisRel);
                    if(thisRel!= NULL){
                        //trova nel dest array di thisrel odest[u].name, decrementa e riordina

                    }
                }
            }
            if(cacheEnt!=NULL) {
                user->next=cacheEnt;
            }
            cacheEnt = user;
            user = NULL;

            for (i = 0; i < 78; i++) {
                pointer = array_lex[i];
                while (pointer != NULL) {
                    printf("1 %s\n",pointer->id_rel);
                    cmpUser = strcmp(pointer->dest[0].name, username);
                    for (j = 0; j < pointer->len_array && cmpUser!= 0; j++) {
                        printf("#\n");
                        cmpUser = strcmp(pointer->dest[j].name, username);
                    }
                    if (cmpUser == 0) {
                        printf("2\n");
                        pointer->len_array--;
                        for (z = j; z < pointer->len_array; z++) {
                            printf("3\n");
                            pointer->dest[z] = pointer->dest[z + 1];
                        }
                    }
                    pointer = pointer->next;
                }
            }
            printf("entità %s eliminata\n",username);
        }else printf("entità non monitorata\n");
    }else printf("entità non monitorata\n");
}

void delrel(){
    printf("delrel\n");
}

void report(){
    int flag_none=0;
    int i,j;
    typeRel *pointer;
    int flag_space=0;

    for (i = 0; i < 78; i++) {
        pointer = array_lex[i];
        while (pointer != NULL) {
            if(pointer->len_array>0) {
                flag_none = 1;
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
    if (flag_none == 0) {
        printf("none");
    }
    printf("\n");
}

void end(){
    printf("end\n");
}

int main() {
    char action[10];
    typeRel *p;
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