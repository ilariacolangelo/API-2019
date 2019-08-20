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
#define SIZEUSER 5
#define SIZELISTUSER 5

typedef struct username { char x[1024]; } username;

typedef struct ent{                                 // entity in hashmap
    char name[1024];
    struct ent *next;
}hash_entity;

typedef struct user{                                //user della singola relazione
    char name[1024];
    int n_rel;
    username name_list[SIZELISTUSER];
}user;

typedef struct rel{                             // teste tipi relazioni
    char id_rel[1024];
    int len_array;
    user rel_users[SIZEUSER];
    struct rel *next;
}head_rel;

typedef struct repTypeRel {
    char id_rel[1024];
    int max;
    username maxuser[SIZEUSER];
}repTypeRel;

hash_entity *hash[SIZEHASH] = {NULL};
hash_entity *cacheEnt = NULL;
head_rel *hashRel[SIZETYPEREL] = {NULL};
head_rel *cacheRel = NULL;


int hashfunc(char username[], int size){                      //polynomial rolling hash function
    int len= strlen(username);
    int hashval=0;
    for(int i = 0;i<len;i++){
        hashval+=(int)username[i]*(long int)exp2(i);
    }
    return (int)hashval%size;
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

void findUserInRel(char *username, head_rel *temp, head_rel *prec, head_rel **hashRel) {
    int i=0;
    int z=0;
    int w;

    int cmpList;
    int cmp;

    while(i<temp->len_array) {//scansiona tutto l'array di user

        cmp = strcmp(temp->rel_users[i].name,username);
        if(cmp == 0) {//username è presente-> elimina elemento da user array
            z = i;
            temp->len_array--;
            while(z<(temp->len_array)){
                temp->rel_users[z] = temp->rel_users[z+1];
                z++;
            }
            strcpy(temp->rel_users[temp->len_array].name,"\0");
            temp->rel_users[temp->len_array].n_rel=0;
            i--;
        }else { //scava nella userlist dell'username in questione per trovare username da eliminare
            z=0;
            cmpList = strcmp(temp->rel_users[i].name_list[z].x,username);
            while(z<temp->rel_users[i].n_rel && cmpList!=0) {
                z++;
                cmpList = strcmp(temp->rel_users[i].name_list[z].x,username);
            }

            if(cmpList==0) { //trovato username in list da eliminare
                temp->rel_users[i].n_rel--;
                while(z<(temp->rel_users[i].n_rel)){
                    strcpy(temp->rel_users[i].name_list[z].x,temp->rel_users[i].name_list[z+1].x);
                    z++;
                }
                strcpy(temp->rel_users[i].name_list[temp->rel_users[i].n_rel].x,"\0");

                if(temp->rel_users[i].n_rel==0) {//User senza più relazioni
                    temp->len_array--;
                    w = i;
                    while(w<(temp->len_array)){
                        temp->rel_users[w] = temp->rel_users[w+1];
                        w++;
                    }
                    strcpy(temp->rel_users[temp->len_array].name,"\0");
                    temp->rel_users[temp->len_array].n_rel=0;
                }
            }
        }

        if (temp->len_array==0){//non esistono più relazioni di quel tipo
            if(prec!=NULL) {
                if (temp->next == NULL) { //ultimo elem di lista di collisioni
                    prec->next = NULL;
                } else {//elemento in mezzo alla lista di collisioni
                    prec->next = temp->next;
                }
            }
            strcpy(temp->id_rel, "\0");
            temp->len_array = 0;
            temp->next = NULL;
            //array di rel_users è già vuoto
            head_rel *pointcache = cacheRel;
            if(pointcache!=NULL){
                while (pointcache->next!=NULL) {
                    pointcache = pointcache->next;
                }
                pointcache->next = temp;
            }else cacheRel = temp;

            if (prec != NULL) temp=prec;
        }

        i++;
    }
}

int isInHash(char *username, hash_entity **hash){          // username is tracked yet?
    int pos;
    hash_entity *flagpoint;
    pos = hashfunc(username,SIZEHASH);
    if(hash[pos]==NULL){
        return 0;
    }else{
        flagpoint = hash[pos];
        while(flagpoint->next!=NULL && strcmp(username,flagpoint->name)!= 0){
            flagpoint = flagpoint->next;
        }
        if(strcmp(username,flagpoint->name)== 0)
            return 1;
        else{
            return 0;
        }
    }
}

int isInRel(char *namerel, head_rel **hash) {                     //curr start pointing to the head of rel_list
    int pos;
    head_rel *flagpoint;
    pos = hashfunc(namerel,SIZETYPEREL);

    if(hash[pos]==NULL){
        return 0;
    }else{
        flagpoint = hash[pos];
        while(flagpoint->next!=NULL && strcmp(namerel,flagpoint->id_rel)!= 0){
            flagpoint = flagpoint->next;
        }
        if(strcmp(namerel,flagpoint->id_rel)== 0)
            return 1;
        else{
            return 0;
        }
    }
}

void addent(hash_entity *hash[]){
    char username[1024];
    hash_entity current;
    hash_entity *item = NULL;
    hash_entity *flagpoint = NULL;
    int pos;

    read(username);                          //scanf without " "

    pos = hashfunc(username,SIZEHASH);
    if(hash[pos]==NULL){
        if(cacheEnt == NULL) {
            item = malloc(sizeof(hash_entity)); //allocate memory for entity-struct
        }else {
           item = cacheEnt;
           if (item->next != NULL) {
               cacheEnt = item->next;
           }else cacheEnt = NULL;
        }
        strcpy(item->name,username);
        item->next = NULL;
        hash[pos] = item;
        printf("Entity:%s addent ok\n",hash[pos]->name);
    }else {
        flagpoint = hash[pos];
        while(flagpoint->next!=NULL && strcmp(username,flagpoint->name)!= 0){
            flagpoint = flagpoint->next;
        }
        if(strcmp(username,flagpoint->name)!= 0) {
            if(cacheEnt == NULL) {
                item = malloc(sizeof(hash_entity)); //allocate memory for entity-struct
            }else {
                item = cacheEnt;
                if (item->next != NULL) {
                    cacheEnt = item->next;
                }else cacheEnt = NULL;
            }
            strcpy(item->name, username);
            item->next = NULL;
            flagpoint->next = item;
            printf("Entity:%s addent ok\n",flagpoint->next->name);
        }else printf("Entity already tracked\n");
    }
}

void addrel(hash_entity *hash[], head_rel *hashRel[]) {
    char orig[1024];
    char dest[1024];
    char rel[1024];
    read(orig);
    read(dest);
    read(rel);

    int flagRel = 0;
    int posHash;
    int i,j;

    head_rel *item=NULL;
    head_rel *pointer=NULL;
    head_rel *prec=NULL;
    head_rel *temp=NULL;

    if(isInHash(orig,hash)==1 && isInHash(dest,hash)==1) {
        printf("RELAZIONE: %s\n",rel);
        posHash = hashfunc(rel,SIZETYPEREL);
        printf("posHash %d\n",posHash);
        pointer = hashRel[posHash];

        while (pointer != NULL && pointer->next != NULL && strcmp(rel,pointer->id_rel)!= 0) {
            pointer = pointer->next;
        }
        if (pointer== NULL || (pointer->next == NULL && strcmp(rel,pointer->id_rel)!= 0)) {
            printf("NUOVA: %s\n",rel);      //add new typerel

            if(cacheRel == NULL) {
                item  = malloc(sizeof(head_rel));
            }else {
                item = cacheRel;
                if (item->next != NULL) {
                    cacheRel = item->next;
                }else cacheRel = NULL;
            }
            strcpy(item->id_rel,rel);
            item->len_array=1;
            strcpy(item->rel_users[0].name, dest);
            item->rel_users[0].n_rel=1;
            strcpy(item->rel_users[0].name_list[0].x,orig);
            item->next=NULL;

            if(pointer == NULL) hashRel[posHash] = item;
            else pointer->next = item;

            printf("aggiunto nuovo typeRel\n");
        }else {
            //pointer punta all'elemento da modificare
            //adesso cerco l'user dest
            int cmpDest = 1;
            for (i=0; i < pointer->len_array && cmpDest != 0;i++) {
                cmpDest = strcmp(pointer->rel_users[i].name, dest);
            }
            if (cmpDest == 0) {//dest è già presente
                int cmpOrig =1;
                for(j=0; j < pointer->rel_users[i].n_rel && cmpOrig != 0; j++) {
                    cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x, orig);
                }
                if (cmpOrig == 0) {
                    printf("Relazione già esistente! \n");
                }else {
                    strcpy(pointer->rel_users[i].name_list[j].x, orig);
                    pointer->rel_users[i].n_rel++;
                    printf("nuovo user aggiunto alla listuser di dest\n");
                }

            }else {//aggiungo dest all'array degli user e orig in pos 0 nella lista user di dest appena creato
                strcpy(pointer->rel_users[i].name, dest);
                pointer->len_array++;
                pointer->rel_users[i].n_rel = 1;
                strcpy(pointer->rel_users[i].name_list[0].x, orig);
                printf("nuovo user aggiunto al typeRel\n");
            }
            printf("modificata struttura\n");
        }
    }else printf("entità non monitorate\n");
}


void delent(hash_entity *hash[], head_rel *hashRel[]) {
    char username[1024];
    int pos;

    hash_entity *flagpoint;
    hash_entity *prec = NULL;
    head_rel *temp = NULL;
    head_rel *tempPrec = NULL;

    read(username);
    pos = hashfunc(username,SIZEHASH);

    if(hash[pos]!=NULL){
        flagpoint = hash[pos];
        while(flagpoint->next!=NULL && strcmp(username,flagpoint->name)!= 0){
            prec =flagpoint;
            flagpoint = flagpoint->next;
        }
        if(strcmp(username,flagpoint->name)== 0) {
            //flagpoint punta all'elemento da eliminare
            if(prec!=NULL) {
                if (flagpoint->next == NULL) { //ultimo elem di lista di collisioni
                    prec->next = NULL;
                } else {//elemento in mezzo alla lista di collisioni
                    prec->next = flagpoint->next;
                }
            }else hash[pos] = NULL;
            strcpy(flagpoint->name, "\0");
            flagpoint->next = NULL;

            hash_entity *pointEntCache = cacheEnt;
            if(pointEntCache!=NULL) flagpoint->next = cacheEnt;
            cacheEnt = flagpoint;

            //scansiono tutta la hashRel
            for(int i=0; i<SIZETYPEREL;i++) {
                if (hashRel[i]!=NULL) { //esiste typeRel in pos i
                    temp = hashRel[i];
                    findUserInRel(username,temp,tempPrec,hashRel); //Qui tempPrec è NULL
                    while (temp->next!=NULL) {
                        tempPrec = temp;
                        temp = temp->next;
                        findUserInRel(username,temp,tempPrec,hashRel);
                    }
                }
            }
        }else printf("Username does not exist!\n");

    }else printf("Username does not exist!\n");
    printf("delEnt done\n");
}

void delrel(hash_entity *hash[], head_rel *hashRel[]) {
    char orig[1024];
    char dest[1024];
    char rel[1024];
    read(orig);
    read(dest);
    read(rel);

    int posHash;
    int cmpRel;

    head_rel *pointer = NULL;
    head_rel *prec = NULL;

    if (isInHash(orig, hash) == 1 && isInHash(dest, hash) == 1) {
        //modifica struttura
        posHash = hashfunc(rel, SIZETYPEREL);                //QI modifica inserendo is in Rel
        pointer = hashRel[posHash];
        if (pointer != NULL) {

            cmpRel = strcmp(pointer->id_rel, rel);
            while (cmpRel != 0 && pointer->next != NULL) {
                prec = pointer;
                pointer = pointer->next;
            }
            if (cmpRel == 0) {
                //pointer punta all'elemento typerel giusto e prec punta all'elemento che viene prima nella collisione altrimenti è NULL
                //adesso cerco l'user dest
                int i = 0;
                int cmpDest = strcmp(pointer->rel_users[i].name, dest);
                while (i < pointer->len_array && cmpDest != 0) {
                    i++;
                    cmpDest = strcmp(pointer->rel_users[i].name, dest);
                }
                if (cmpDest == 0) {//dest è presente
                    int j = 0;
                    int cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x, orig);

                    while (j < pointer->rel_users[i].n_rel && cmpOrig != 0) {
                        j++;
                        cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x, orig);
                    }
                    if (cmpOrig == 0) {//orig è presente
                        //modificare struttura eliminando l'orig da userlist di dest relazione
                        pointer->rel_users[i].n_rel--;
                        while (j < (pointer->rel_users[i].n_rel)) {
                            strcpy(pointer->rel_users[i].name_list[j].x, pointer->rel_users[i].name_list[j + 1].x);
                            j++;
                        }
                        strcpy(pointer->rel_users[i].name_list[pointer->rel_users[i].n_rel].x, "\0");

                        if (pointer->rel_users[i].n_rel == 0) {//dest senza più relazioni
                            pointer->len_array--;
                            while (i < (pointer->len_array)) {
                                pointer->rel_users[i] = pointer->rel_users[i + 1];
                                i++;
                            }
                            strcpy(pointer->rel_users[pointer->len_array].name, "\0");
                            pointer->rel_users[pointer->len_array].n_rel = 0;
                        }

                    }else {
                        printf("orig: relazione non esistente\n");
                    }
                }else {
                    printf("dest: relazione non esistente\n");
                }
            }
        }else {
            printf("relazione non esistente\n");
        }
        printf("delrel done\n");
    }
}

void report(head_rel *hashRel[]){
    int i,j,z,w,y,h;
    int sizeUserArray;
    int sizeArray=0;
    head_rel *temp = NULL;
    head_rel *tempPrec = NULL;
    repTypeRel reportArray[SIZETYPEREL];

    for(i=0; i<SIZETYPEREL;i++) {
        if (hashRel[i]!=NULL) { //esiste typeRel in pos i
            temp = hashRel[i];
            //blocco ripetuto nel while START
            j=0;
            if(sizeArray==0){
                strcpy(reportArray[j].id_rel,temp->id_rel);
                sizeArray++;
            }else {
                while(j<sizeArray && strcmp(reportArray[j].id_rel,temp->id_rel)<0) {//ordine alfabetico del typerel
                    j++;
                }
                if(strcmp(reportArray[j].id_rel,temp->id_rel)>0) { // j=pos in array dove inserire la typeRel
                    for(z=sizeArray; z>j; z--) {
                        reportArray[z-1] = reportArray [z];
                    }
                    strcpy(reportArray[j].id_rel,temp->id_rel);
                    sizeArray++;
                }
            }
            for(w = 0; w<temp->len_array; w++){ //ricerca del max del typerel in pos j
                reportArray[j].max = 0;
                if(temp->rel_users[w].n_rel>reportArray[j].max) {
                    reportArray[j].max = temp->rel_users[w].n_rel;
                }
            }

            sizeUserArray = 0;
            for(w = 0; w<temp->len_array; w++) { //ricerca di user con max ed inserimento in array
                if (temp->rel_users[w].n_rel == reportArray[j].max) {
                    if(sizeUserArray == 0){
                        strcpy(reportArray[j].maxuser[0].x,temp->rel_users[w].name);
                        sizeUserArray++;
                    }else {
                        y=0;
                        while(y<sizeArray && strcmp(reportArray[j].maxuser[y].x, temp->rel_users[w].name)<0) {//ordine alfabetico del typerel
                            y++;
                        }
                        if(strcmp(reportArray[j].maxuser[y].x, temp->rel_users[w].name)>0) {
                            for(h=sizeUserArray; h>y; h--) {
                                reportArray[j].maxuser[h-1]= reportArray[j].maxuser[h];
                            }
                            strcpy(reportArray[j].maxuser[y].x,temp->rel_users[w].name);
                            sizeUserArray++;
                        }
                    }
                }
            }
            //blocco ripetuto nel while FINISH

            while (temp->next!=NULL) {
                tempPrec = temp;
                temp = temp->next;

                //da 444 a 490 uguale
                j=0;
                if(sizeArray==0){
                    strcpy(reportArray[j].id_rel,temp->id_rel);
                    sizeArray++;
                }else {
                    while(j<sizeArray && strcmp(reportArray[j].id_rel,temp->id_rel)<0) {//ordine alfabetico del typerel
                        j++;
                    }
                    if(strcmp(reportArray[j].id_rel,temp->id_rel)>0) { // j=pos in array dove inserire la typeRel
                        for(z=sizeArray; z>j; z--) {
                            reportArray[z-1] = reportArray [z];
                        }
                        strcpy(reportArray[j].id_rel,temp->id_rel);
                        sizeArray++;
                    }
                }
                for(w = 0; w<temp->len_array; w++){ //ricerca del max del typerel in pos j
                    reportArray[j].max = 0;
                    if(temp->rel_users[w].n_rel>reportArray[j].max) {
                        reportArray[j].max = temp->rel_users[w].n_rel;
                    }
                }

                sizeUserArray = 0;
                for(w = 0; w<temp->len_array; w++) { //ricerca di user con max ed inserimento in array
                    if (temp->rel_users[w].n_rel == reportArray[j].max) {
                        if(sizeUserArray == 0){
                            strcpy(reportArray[j].maxuser[0].x,temp->rel_users[w].name);
                            sizeUserArray++;
                        }else {
                            y=0;
                            while(y<sizeArray && strcmp(reportArray[j].maxuser[y].x, temp->rel_users[w].name)<0) {//ordine alfabetico del typerel
                                y++;
                            }
                            if(strcmp(reportArray[j].maxuser[y].x, temp->rel_users[w].name)>0) {
                                for(h=sizeUserArray; h>y; h--) {
                                    reportArray[j].maxuser[h-1]= reportArray[j].maxuser[h];
                                }
                                strcpy(reportArray[j].maxuser[y].x,temp->rel_users[w].name);
                                sizeUserArray++;
                            }
                        }
                    }
                }
                //fine blocco uguale
            }
        }
    }
    //costruzione dell'output
    printf("report\n");
}

void end(){
    printf("end\n");
}

int main() {
    int d,s,t;
    head_rel *tem;
    char action[10];


    do{

        scanf("%s", action);
        if(action[0]=='a') {

            if(action[3]=='e') {
                addent(hash);
            }else addrel(hash,hashRel);

        }else if(action[0]=='d') {

            if(action[3]=='e') {
                delent(hash, hashRel);
            }else delrel(hash,hashRel);

        }else if(action[0]=='r') {
            report(hashRel);
        }else if(action[0]=='e') {
            end();
        }

        for(t=0; t<SIZEHASH; t++){
            if (hash[t]== NULL) {
                printf("-");
            }else printf(" %s ",hash[t]->name);
        }
        printf("\n");
        for(t=0; t<SIZETYPEREL; t++) {
            if (hashRel[t]== NULL) {
                printf("-");
            }else {
                tem = hashRel[t];
                while (tem->next!=NULL) {
                    printf(" %s lenarray %d ->",tem->id_rel, tem->len_array);
                    tem = tem->next;
                }
                printf(" %s lenarray %d ->X",tem->id_rel, tem->len_array);
                /*for (s=0;s<SIZEUSER;s++) {
                    if(strcmp(hashRel[t]->rel_users[s].name,"\0")!=0) {
                        printf("%s->listUser: ", hashRel[t]->rel_users[s].name);
                        for (d=0; d<SIZELISTUSER;d++) {
                            if(strcmp(hashRel[t]->rel_users[s].name_list[d].x,"\0")!=0)
                                printf("%s", hashRel[t]->rel_users[s].name_list[d].x);
                            else
                                printf("@");
                        }
                        printf(" ");
                    }
                    else
                        printf("+");
                }*/
                printf(" ");
            }
        }
        printf("\n");
    } while(action[0]!='e');

    printf("end of file\n");
}