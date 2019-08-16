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
    long int hashval=0;
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

void delent(){
    printf("delent\n");
}

void addrel(hash_entity *hash[], head_rel *hashRel[]){
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
            //adesso certo l'user dest
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
                    printf("relazione già esistente! \n");
                }else {
                    strcpy(pointer->rel_users[i].name_list[j+1].x,orig);
                    pointer->rel_users[i].n_rel++;
                    printf("nuova relazione aggiunta alla struttura\n");
                }

            }else {//aggiungo dest all'array degli user e orig in pos 0 nella lista user di dest appena creato
                strcpy(pointer->rel_users[i+1].name,dest);
                pointer->rel_users[i+1].n_rel = 1;
                strcpy(pointer->rel_users[i+1].name_list[0].x,orig);
            }
            printf("modifica struttura\n");
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
            printf("aggiunta!\n");
        }

    }else printf("entità non monitorate\n");

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
        if(strcmp(action,ADDENT)==0){
            addent(hash);
        }else if(strcmp(action,DELENT)==0){
            delent();
        }else if(strcmp(action,ADDREL)==0){
            addrel(hash,hashRel);
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