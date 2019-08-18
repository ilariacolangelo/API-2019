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

#define SIZEHASH 100000
#define SIZETYPEREL 1000
#define SIZEUSER 300
#define SIZELISTUSER 200

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

hash_entity *hash[SIZEHASH] = {NULL};
head_rel *hashRel[SIZETYPEREL] = {NULL};
head_rel *head;

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

hash_entity setInput(char username[]){              //create hash   entity
    hash_entity ret;
    strcpy(ret.name,username);
    ret.next= NULL;
    return ret;
}


void setRel(char rel[], char orig[], char dest[],head_rel*item){                                  //create rel entity
    strcpy(item->id_rel,rel);
    item->len_array=1;
    strcpy(item->rel_users[0].name, dest);
    item->rel_users[0].n_rel=1;
    strcpy(item->rel_users[0].name_list[0].x,orig);
    item->next=NULL;
}

void addItem(hash_entity *item, hash_entity elem){     //link pointer with memory allocated to entity
    strcpy(item->name,elem.name);
    item->next = NULL;
}

void addRelType(head_rel *p, head_rel* elem){
    strcpy(p->id_rel,elem->id_rel);
    p->len_array=elem->len_array;
    p->rel_users[0]=elem->rel_users[0];
    p->next=elem->next;
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
            free(temp);
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

int isBefore(char username1[],char username2[]) {   //lexicographic check
    if (strcmp(username1,username2)<=0)
        return 1;
    else
        return 0;
}

void addent(hash_entity *hash[]){
    char username[1024];
    hash_entity current;
    hash_entity *item;
    hash_entity *flagpoint;
    int pos;

    read(username);                          //scanf without " "

    if (isInHash(username,hash)==0) {

        current = setInput(username);
        item = malloc(sizeof(hash_entity));  //allocate memory for entity-struct
        addItem(item, current);

        pos = hashfunc(item->name,SIZEHASH);         // pos entity in hashmap


        if (hash[pos] != NULL) {            //if pos is occupied for collision put in next
            hash_entity *curr;
            curr = hash[pos];
            while (curr->next != NULL) {
                curr = curr->next;
            }
            curr->next = item;
        } else hash[pos] = item;            //if pos is free
        printf("Username:%s addent ok\n",hash[pos]->name);
    }else printf("Username already tracked!\n");

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

    head_rel *item=NULL;
    head_rel *pointer=NULL;
    head_rel *create=NULL;
    head_rel *temp=NULL;

    if(isInHash(orig,hash)==1 && isInHash(dest,hash)==1) {
        printf("NUOVA RELAZIONE: %s\n",rel);
        flagRel=isInRel(rel,hashRel);
        printf("flagRel %d\n",flagRel);
        if (flagRel==1) { //modifica struttura
            posHash = hashfunc(rel,SIZETYPEREL);
            pointer = hashRel[posHash];
            while(strcmp(pointer->id_rel,rel)!=0) {
                pointer = pointer->next;
            }
            //pointer punta all'elemento typerel giusto
            //adesso cerco l'user dest
            int i=0;
            int cmpDest = strcmp(pointer->rel_users[i].name,dest);
            while(i<pointer->len_array && cmpDest!=0) {
                i++;
                cmpDest = strcmp(pointer->rel_users[i].name,dest);
            }
            if(cmpDest == 0) {//dest è già presente
                int j = 0;
                int cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x,orig);

                while(j<pointer->rel_users[i].n_rel && cmpOrig!=0) {
                    j++;
                    cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x,orig);
                }
                if(cmpOrig==0) {
                    printf("Relazione già esistente! \n");
                }else {
                    strcpy(pointer->rel_users[i].name_list[j+1].x,orig);
                    pointer->rel_users[i].n_rel++;
                    printf("nuovo user aggiunto alla listuser di dest\n");
                }

            }else {//aggiungo dest all'array degli user e orig in pos 0 nella lista user di dest appena creato
                strcpy(pointer->rel_users[i+1].name,dest);
                pointer->rel_users[i+1].n_rel = 1;
                strcpy(pointer->rel_users[i+1].name_list[0].x,orig);
                printf("nuovo user aggiunto al typeRel\n");
            }
            printf("modificata struttura\n");
        }else { //aggiungi nuova rel
            item  = malloc(sizeof(head_rel));
            setRel(rel,orig,dest,item);
            //create = malloc(sizeof(head_rel));
            //addRelType(create,item);
            posHash = hashfunc(rel,SIZETYPEREL);
            if(hashRel[posHash]== NULL){
                hashRel[posHash] = item;
            }else{ //collision
                head_rel *point = hashRel[posHash];
                while(point->next!=NULL){
                    point = point->next;
                }
                point->next = item;
            }

            create = NULL;
            printf("aggiunto nuovo typeRel\n");
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

    if (isInHash(username,hash)==1) {
        pos = hashfunc(username,SIZEHASH);
        flagpoint = hash[pos];
        while(flagpoint->next!=NULL && strcmp(username,flagpoint->name)!= 0){
            prec = flagpoint;
            flagpoint = flagpoint->next;
        }
        //flagpoint punta all'elemento da eliminare
        if(prec!=NULL) {
            if (flagpoint->next == NULL) { //ultimo elem di lista di collisioni
                prec->next = NULL;
            } else {//elemento in mezzo alla lista di collisioni
                prec->next = flagpoint->next;
            }
        }
        free(flagpoint);

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
    printf("delent done\n");
}

void delrel(hash_entity *hash[], head_rel *hashRel[]){
    char orig[1024];
    char dest[1024];
    char rel[1024];
    read(orig);
    read(dest);
    read(rel);

    int posHash;

    head_rel *pointer=NULL;
    head_rel *prec=NULL;

    if(isInHash(orig,hash)==1 && isInHash(dest,hash)==1 && isInRel(rel,hashRel)==1) {
        //modifica struttura
        posHash = hashfunc(rel,SIZETYPEREL);
        pointer = hashRel[posHash];
        while(strcmp(pointer->id_rel,rel)!=0) {
            prec = pointer;
            pointer = pointer->next;
        }
        //pointer punta all'elemento typerel giusto e prec punta all'elemento che viene prima nella collisione altrimenti è NULL
        //adesso cerco l'user dest
        int i=0;
        int cmpDest = strcmp(pointer->rel_users[i].name,dest);
        while(i<pointer->len_array && cmpDest!=0) {
            i++;
            cmpDest = strcmp(pointer->rel_users[i].name,dest);
        }
        if(cmpDest == 0) {//dest è presente
            int j = 0;
            int cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x,orig);

            while(j<pointer->rel_users[i].n_rel && cmpOrig!=0) {
                j++;
                cmpOrig = strcmp(pointer->rel_users[i].name_list[j].x,orig);
            }
            if(cmpOrig==0) {//orig è presente
                //modificare struttura eliminando l'orig da userlist di dest relazione
                pointer->rel_users[i].n_rel--;
                while(j<(pointer->rel_users[i].n_rel)){
                    strcpy(pointer->rel_users[i].name_list[j].x,pointer->rel_users[i].name_list[j+1].x);
                    j++;
                }
                strcpy(pointer->rel_users[i].name_list[pointer->rel_users[i].n_rel].x,"\0");

                if(pointer->rel_users[i].n_rel==0) {//dest senza più relazioni
                    pointer->len_array--;
                    while(i<(pointer->len_array)){
                        pointer->rel_users[i] = pointer->rel_users[i+1];
                        i++;
                    }
                    strcpy(pointer->rel_users[pointer->len_array].name,"\0");
                    pointer->rel_users[pointer->len_array].n_rel=0;
                }

                if (pointer->len_array==0){//non esistono più relazioni di quel tipo
                    if(prec!=NULL) {
                        if (pointer->next == NULL) { //ultimo elem di lista di collisioni
                            prec->next = NULL;
                        } else {//elemento in mezzo alla lista di collisioni
                            prec->next = pointer->next;
                        }
                    }
                    free(pointer);

                }
            }else {
                printf("orig: relazione non esistente\n");
            }
        }else {
            printf("dest: relazione non esistente\n");
        }
    }else {
        printf("relazione non esistente\n");
    }
    printf("delrel done\n");
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
            }else addrel(hash,hashRel);

        }else if(action[0]=='d') {

            if(action[3]=='e') {
                delent(hash, hashRel);
            }else delrel(hash,hashRel);

        }else if(action[0]=='r') {
            report();
        }else if(action[0]=='e') {
            end();
        }

    } while(action[0]!='e');

    printf("end of file\n");
}