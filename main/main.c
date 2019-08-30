#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SIZEHASH 10000

typedef struct head_in_hash {
    struct headReport *p;
    struct head_in_hash *next;
}head_in_hash;

typedef struct orig_in_hash {
    struct origRel *p;
    struct orig_in_hash *next;
}orig_in_hash;

typedef struct origRel {
    struct entity *name;
    struct headReport *dest;
    struct origRel *prec;
    struct origRel *next;
}origRel;

typedef struct headReport{                                 // entity in hashmap
    int n_rel;
    struct entity *name;
    origRel *first_orig;
    struct headReport *prec;
    struct headReport *next;
}headReport;

typedef struct entity{                                 // entity in hashmap
    char name[1024];
    head_in_hash *head_list;
    orig_in_hash *orig_list;
    struct entity *next;
}entity;

typedef struct typeRel{                             //tipi relazioni
    char id_rel[1024];
    headReport *head_list;
    struct typeRel *next;
}typeRel;


entity *hash[SIZEHASH] = {NULL};
typeRel *array_lex[78] = {NULL};
entity *cacheEnt = NULL;
headReport *cacheHead = NULL;
origRel *cacheOrig = NULL;
orig_in_hash *cacheHashOrig = NULL;
head_in_hash *cacheHashHead = NULL;



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

void findRel(char id_rel[], typeRel **p, typeRel **prec) {
    int pos=(int)id_rel[0]- 45;
    (*prec)=NULL;
    (*p)=array_lex[pos];
    if ((*p)!= NULL){
        while ((*p)->next!=NULL && strcmp((*p)->id_rel,id_rel)<0){
            (*prec)=(*p);
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

void addrel() {
    char orig[1024];
    char dest[1024];
    char rel[1024];
    int pos;

    entity *p_orig;
    entity *p_dest;
    typeRel *p_rel;
    typeRel *prec_rel;
    typeRel *item_rel = NULL;
    headReport *item_head = NULL;
    headReport *temp_head = NULL;
    headReport *index_head = NULL;
    origRel *item_orig = NULL;
    origRel *temp_orig = NULL;
    orig_in_hash *item_hash_orig = NULL;
    head_in_hash *item_hash_head = NULL;

    read(orig);
    read(dest);
    read(rel);

    findInHash(dest, &p_dest);      //cerco in hash dest e orig
    findInHash(orig, &p_orig);

    if (p_dest!= NULL && p_orig!=NULL) {    //entità presenti in hash

        findRel(rel, &p_rel, &prec_rel);    //cerco typeRel
        printf("rel found %s\n",p_rel->id_rel);

        if(p_rel!=NULL){    //typeRel già presente
            printf("da modificare \n");
            temp_head = p_rel->head_list;
            while (temp_head!=NULL && temp_head->next!=NULL && temp_head->name != p_dest){          // cerca il dest nella lista di headReport
                temp_head = temp_head->next;
            }
            if(temp_head!=NULL && temp_head->name == p_dest) {         //trovato dest
                temp_orig = temp_head->first_orig;
                while (temp_orig!=NULL && temp_orig->next!=NULL && temp_orig->name != p_orig){          // cerca orig nella lista di origEnt
                    temp_orig= temp_orig->next;
                }
                if(temp_orig != NULL && temp_orig->name == p_orig) {    //trovato orig
                  printf("relazione già esistente\n");
                    return;
                }
            }else {
                if(cacheHead == NULL) {                     //mi creo elemento per headReport controllando la cacheHead
                    item_head = malloc(sizeof(headReport));
                }else {
                    item_head = cacheHead;
                    cacheHead = item_head->next;
                }

                item_head->n_rel = 0;                       //riempo e posiziono la headReport
                item_head->next = NULL;
                item_head->name = p_dest;

                if(temp_head == NULL) {
                    p_rel->head_list = item_head;
                    item_head->prec = NULL;
                }
                else {                              //temp_head punta alla coda della lista
                    temp_head->next = item_head;
                    item_head->prec = temp_head;
                }

                if(cacheHashHead == NULL) {                     //mi creo elemento per head_in_hash controllando la cache
                    item_hash_head = malloc(sizeof(head_in_hash));
                }else {
                    item_hash_head = cacheHashHead;
                    cacheHashHead = item_hash_head->next;
                }

                item_hash_head->p = item_head;                  //aggancio entity alla struttura delle rel
                item_hash_head->next=p_dest->head_list;
                p_dest->head_list = item_hash_head;

                temp_head = item_head; //temp punta al dest
            }

            //aggiungo orig
            if(cacheOrig == NULL) {                     //mi creo elemento per origRel controllando la cacheOrig
                item_orig = malloc(sizeof(origRel));
            }else {
                item_orig = cacheOrig;
                cacheOrig = item_orig->next;
            }

            item_orig->dest = temp_head;                //riempo origRel
            item_orig->name = p_orig;
            item_orig->prec = NULL;
            item_orig->next = temp_head->first_orig;    // aggiunto in testa
            temp_head->first_orig = item_orig;
            item_orig->next->prec = item_orig;

            if(cacheHashOrig == NULL) {                     //mi creo elemento per orig_in_hash controllando la cache
                item_hash_orig = malloc(sizeof(orig_in_hash));
            }else {
                item_hash_orig = cacheHashOrig;
                cacheHashOrig = item_hash_orig->next;
            }

            item_hash_orig->p = item_orig;                  //aggancio entity alla struttura delle rel
            item_hash_orig->next = p_orig->orig_list;
            p_orig->orig_list = item_hash_orig;

            temp_head->n_rel++;

            //RIORDINAMENTO LISTA REPORT
            index_head = temp_head;
            while(index_head->prec != NULL && (temp_head->n_rel>index_head->prec->n_rel|| (temp_head->n_rel==index_head->prec->n_rel && strcmp(temp_head->name->name,index_head->prec->name->name)<0))) {
                index_head = index_head->prec;
            }

            if(index_head->prec != NULL) { //aggiungo in testa
                temp_head->prec->next = NULL;
                temp_head->prec = NULL;
                temp_head->next = index_head;
                index_head->prec = temp_head;
                p_rel->head_list = temp_head;

            }else if(index_head != temp_head) { //aggiungo in mezzo
                temp_head->prec->next = NULL;
                temp_head->prec = index_head->prec;
                index_head->prec->next = temp_head;
                temp_head->next = index_head;
                index_head->prec = temp_head;
            }
            // LISTA REPORT MODIFICATA

            printf("modificata \n");
        }else {     //creo nuovo typeRel
            item_rel = malloc(sizeof(typeRel));         //mi creo elemento per typeRel

            if(cacheHead == NULL) {                     //mi creo elemento per headReport controllando la cacheHead
                item_head = malloc(sizeof(headReport));
            }else {
                item_head = cacheHead;
                cacheHead = item_head->next;
            }

            if(cacheOrig == NULL) {                     //mi creo elemento per origRel controllando la cacheOrig
                item_orig = malloc(sizeof(origRel));
            }else {
                item_orig = cacheOrig;
                cacheOrig = item_orig->next;
            }

            strcpy(item_rel->id_rel,rel);//riempo e posiziono typeRel
            item_rel->head_list = item_head;
            item_rel->next = NULL;

            findRel(rel,&p_rel,&prec_rel);
            if (prec_rel == NULL) { //aggiungi in testa
                pos=(int)rel[0]- 45;
                if(array_lex[pos] != NULL) item_rel->next = array_lex[pos];
                array_lex[pos] = item_rel;

            }else {//aggiungi in mezzo o coda
                item_rel->next = prec_rel->next;
                prec_rel->next = item_rel;
            }

            item_head->n_rel = 1;                       //riempo headReport
            item_head->next = NULL;
            item_head->prec = NULL;
            item_head->name = p_dest;
            item_head->first_orig = item_orig;

            item_orig->dest = item_head;                //riempo origRel
            item_orig->prec = NULL;
            item_orig->next = NULL;
            item_orig->name = p_orig;

            if(cacheHashHead == NULL) {                     //mi creo elemento per head_in_hash controllando la cache
                item_hash_head = malloc(sizeof(head_in_hash));
            }else {
                item_hash_head = cacheHashHead;
                cacheHashHead = item_hash_head->next;
            }

            if(cacheHashOrig == NULL) {                     //mi creo elemento per orig_in_hash controllando la cache
                item_hash_orig = malloc(sizeof(orig_in_hash));
            }else {
                item_hash_orig = cacheHashOrig;
                cacheHashOrig = item_hash_orig->next;
            }

            item_hash_head->p = item_head;                  //aggancio entity alla struttura delle rel
            item_hash_head->next=p_dest->head_list;
            p_dest->head_list = item_hash_head;

            item_hash_orig->p = item_orig;                  //
            item_hash_orig->next = p_orig->orig_list;
            p_orig->orig_list = item_hash_orig;

            printf("rel aggiunta\n");
        }

    }else {
        printf("entità non monitorate\n");
    }
}

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