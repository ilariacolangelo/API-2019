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

typedef struct username { char x[1024]; } username;

typedef struct ent{                                 // entity in hashmap
    char name[1024];
    struct ent *next;
}hash_entity;

typedef struct rel{                             // teste tipi relazioni
    char id_rel[1024];
    int len_array;
    user *first_user;
    struct rel *next;
}head_rel;

typedef struct user{                                //user della singola relazione
    char name[1024];
    int n_rel;
    username name_list[1];
}user;

int hashfunc(char username[]){                      //polynomial rolling hash function
    int len= strlen(username);
    long int hashval=0;
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

hash_entity setInput(char username[]){              //create hash   entity
    hash_entity ret;
    strcpy(ret.name,username);
    ret.next= NULL;
    return ret;
}

void addItem(hash_entity *item, hash_entity elem){     //link pointer with memory allocated to entity
    strcpy(item->name,elem.name);
    item->next = NULL;
}

int isInHash(char *username, hash_entity **hash){          // username is tracked yet?
    int pos;
    hash_entity *flagpoint;
    pos = hashfunc(username);

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

int isInRel(char *namerel, head_rel *curr) {                     //curr start pointing to the head of rel_list
    int cmp;
    if (curr!=NULL) {
        cmp = strcmp(curr->id_rel,namerel);
        while (cmp < 0 && curr->next != NULL){
            curr = curr->next;
            cmp = strcmp(curr->id_rel,namerel);
        }
        if (cmp == 0)return 1;
    }
    return 0;
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

        pos = hashfunc(item->name);         // pos entity in hashmap


        if (hash[pos] != NULL) {            //if pos is occupied for collision put in next
            hash_entity *curr;
            curr = hash[pos];
            while (curr->next != NULL) {
                curr = curr->next;
            }
            curr->next = item;
        } else hash[pos] = item;            //if pos is free
        printf("Username:%s\nPos:%d\naddent ok\n",hash[pos]->name, pos);
    }else printf("Username already tracked!\n");



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

int main() {
    char action[10];
    hash_entity *hash[SIZEHASH] = {NULL};
    head_rel *head;



    do{

        scanf("%s", action);
        if(strcmp(action,ADDENT)==0){
            addent(hash);
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


