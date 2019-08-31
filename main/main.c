#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define SIZEHASH 10000

typedef struct head_in_hash {
    struct headReport *p;
    struct head_in_hash *next;
    struct head_in_hash *prec;
}head_in_hash;

typedef struct orig_in_hash {
    struct origRel *p;
    struct orig_in_hash *next;
    struct orig_in_hash *prec;
}orig_in_hash;

typedef struct origRel {
    struct entity *name;
    struct headReport *dest;
    struct origRel *prec;
    struct origRel *next;
}origRel;

typedef struct headReport{                                 // entity in hashmap
    struct entity *name;
    int n_rel;
    struct typeRel *rel;
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

        if(p_rel!=NULL) {    //typeRel già presente
            printf("da modificare \n");
            temp_head = p_rel->head_list;
            while (temp_head != NULL && temp_head->next != NULL &&
                   temp_head->name != p_dest) {          // cerca il dest nella lista di headReport
                temp_head = temp_head->next;
            }

            if (temp_head != NULL && temp_head->name == p_dest) {         //trovato dest
                temp_orig = temp_head->first_orig;
                while (temp_orig != NULL && temp_orig->next != NULL && temp_orig->name != p_orig) {          // cerca orig nella lista di origEnt
                    temp_orig = temp_orig->next;
                }
                if (temp_orig != NULL && temp_orig->name == p_orig) {    //trovato orig
                    printf("relazione già esistente\n");
                    return;
                }
            } else {

                if (cacheHead == NULL) {                     //mi creo elemento per headReport controllando la cacheHead
                    item_head = malloc(sizeof(headReport));
                } else {
                    item_head = cacheHead;
                    cacheHead = item_head->next;
                }

                item_head->n_rel = 0;                       //riempo e posiziono la headReport
                item_head->next = NULL;
                item_head->name = p_dest;
                item_head->rel = p_rel;

                if (temp_head == NULL) {
                    p_rel->head_list = item_head;
                    item_head->prec = NULL;
                } else {                              //temp_head punta alla coda della lista
                    temp_head->next = item_head;
                    item_head->prec = temp_head;
                }

                if (cacheHashHead ==NULL) {                     //mi creo elemento per head_in_hash controllando la cache
                    item_hash_head = malloc(sizeof(head_in_hash));
                } else {
                    item_hash_head = cacheHashHead;
                    cacheHashHead = item_hash_head->next;
                }

                item_hash_head->p = item_head;                  //aggancio entity alla struttura delle rel
                item_hash_head->next = p_dest->head_list;
                item_hash_head->prec = NULL;
                if(p_dest->head_list!=NULL) p_dest->head_list->prec = item_hash_head;
                p_dest->head_list = item_hash_head;

                temp_head = item_head; //temp punta al dest

            }

            //aggiungo orig
            if (cacheOrig == NULL) {                     //mi creo elemento per origRel controllando la cacheOrig
                item_orig = malloc(sizeof(origRel));
            } else {
                item_orig = cacheOrig;
                cacheOrig = item_orig->next;
            }

            item_orig->dest = temp_head;            //riempo origRel
            item_orig->name = p_orig;
            item_orig->prec = NULL;
            item_orig->next = temp_head->first_orig;    // aggiunto in testa
            if(temp_head->first_orig!=NULL) temp_head->first_orig->prec = item_orig;
            temp_head->first_orig = item_orig;

            if (cacheHashOrig == NULL) {                     //mi creo elemento per orig_in_hash controllando la cache
                item_hash_orig = malloc(sizeof(orig_in_hash));
            } else {
                item_hash_orig = cacheHashOrig;
                cacheHashOrig = item_hash_orig->next;
            }

            item_hash_orig->p = item_orig;                  //aggancio entity alla struttura delle rel
            item_hash_orig->next = p_orig->orig_list;
            item_hash_orig->prec = NULL;
            if(p_orig->orig_list!=NULL) p_orig->orig_list->prec = item_hash_orig;
            p_orig->orig_list = item_hash_orig;

            temp_head->n_rel++;

            //RIORDINAMENTO LISTA REPORT
            index_head = temp_head;
            while (index_head->prec != NULL && (temp_head->n_rel > index_head->prec->n_rel ||
                                                (temp_head->n_rel == index_head->prec->n_rel &&
                                                 strcmp(temp_head->name->name, index_head->prec->name->name) < 0))) {
                index_head = index_head->prec;
            }
            if(index_head != temp_head){ //Se devo spostarlo
                if (index_head->prec == NULL) { //aggiungo in testa
                    temp_head->prec->next = NULL;
                    temp_head->prec = NULL;
                    temp_head->next = index_head;
                    index_head->prec = temp_head;
                    p_rel->head_list = temp_head;
                }else { //aggiungo in mezzo
                    temp_head->prec->next = NULL;
                    temp_head->prec = index_head->prec;
                    index_head->prec->next = temp_head;
                    temp_head->next = index_head;
                    index_head->prec = temp_head;
                }
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

            strcpy(item_rel->id_rel,rel);               //riempo e posiziono typeRel
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
            item_head->rel = item_rel;
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

            item_hash_head->p = item_head;
            item_hash_head->next=p_dest->head_list;
            item_hash_head->prec = NULL;
            p_dest->head_list = item_hash_head;

            item_hash_orig->p = item_orig;                  //
            item_hash_orig->next = p_orig->orig_list;
            item_hash_orig->prec = NULL;
            p_orig->orig_list = item_hash_orig;

            printf("rel aggiunta\n");
        }

    }else {
        printf("entità non monitorate\n");
    }
}

void delent(){
    char username[1024];
    entity *p_user;
    head_in_hash *tempHeadInHash = NULL;
    head_in_hash *precHeadInHash = NULL;
    orig_in_hash *tempOrigInHash = NULL;
    orig_in_hash *precTempOrigInHash = NULL;
    origRel *tempOrig = NULL;
    origRel *precOrig = NULL;
    headReport *tempHead = NULL;
    headReport *index_head = NULL;
    read(username);

    findInHash(username,&p_user);

    if(p_user != NULL){
        printf("1\n");
        //elimino user tutte le volte che è stato orig
        tempOrigInHash = p_user->orig_list;
        while(tempOrigInHash!= NULL) {
            precTempOrigInHash = tempOrigInHash;

            tempOrig = tempOrigInHash->p;
            tempHead = tempOrig->dest;

            if (tempOrig->prec != NULL) {
                tempOrig->prec->next = tempOrig->next;
            } else {
                tempOrig->dest->first_orig = tempOrig->next; //cambia testa della lista
            }
            printf("2\n");
            if (tempOrig->next != NULL) {
                tempOrig->next->prec = tempOrig->prec;
            }

            tempOrig->prec = NULL;         //sposta orig in cache
            tempOrig->dest = NULL;
            tempOrig->name = NULL;
            tempOrig->next = cacheOrig;
            cacheOrig = tempOrig;

            printf("3\n");
            //decrementa di una relazione dest
            tempHead->n_rel--;
            if (tempHead->n_rel > 0) { //riordina dest

                index_head = tempHead;
                while (index_head->next != NULL && (index_head->next->n_rel > tempHead->n_rel ||
                                                    (index_head->next->n_rel == tempHead->n_rel &&
                                                     strcmp(tempHead->name->name, index_head->name->name) > 0))) {
                    index_head = index_head->next;
                }

                if (index_head != tempHead) { //va spostato
                    printf("4\n");
                    //estrazione
                    if (tempHead->prec != NULL) {
                        tempHead->prec->next = tempHead->next;
                    } else {
                        tempHead->rel->head_list = tempHead->next; //cambia testa della lista
                    }
                    tempHead->next->prec = tempHead->prec;

                    //riposizionamento
                    tempHead->prec = index_head;
                    tempHead->next = index_head->next;
                    if (index_head->next != NULL) index_head->next->prec = tempHead;
                    index_head->next = tempHead;

                }
            }
            printf("5\n");
            tempOrigInHash->prec = NULL;
            tempOrigInHash->p = NULL;
            tempOrigInHash = tempOrigInHash->next;
        }
        if(precTempOrigInHash!=NULL) {
            precTempOrigInHash->next = cacheHashOrig;
            cacheHashOrig = p_user->orig_list;
        }
        tempOrigInHash = NULL;
        tempOrig = NULL;
        tempHead = NULL;

        printf("6\n");
        //elimino user tutte le volte che è stato dest
        tempHeadInHash = p_user->head_list;
        while (tempHeadInHash!=NULL){
            precHeadInHash = tempHeadInHash;
            tempHead = tempHeadInHash->p;

            if(tempHead->prec!=NULL){
                tempHead->prec->next = tempHead->next;
            }else {
                tempHead->rel->head_list = tempHead->next; //cambia testa della lista
            }

            if(tempHead->next!=NULL){
                tempHead->next->prec = tempHead->prec;
            }

            printf("7\n");
            //svuoto la head da tutti i suoi orig
            tempOrig = tempHead->first_orig;
            while(tempOrig!=NULL){
                printf("7.1\n");
               precOrig = tempOrig;
               tempOrigInHash = tempOrig->name->orig_list;
                printf("7.1.1\n");
               while(tempOrigInHash->p != tempOrig){
                   printf("7.1.2\n");
                   tempOrigInHash = tempOrigInHash->next;
                   printf("7.1.3\n");
               }
                printf("7.1.4\n");
                //elimino orig_in_hash da entity
                if(tempOrigInHash->prec!=NULL){
                    printf("7.1.5\n");
                    //printf("%s\n",tempOrigInHash->prec->next->p->name->name);
                    (tempOrigInHash->prec)->next = tempOrigInHash->next;
                    printf("7.1.6\n");

                }else {
                    printf("7.1.7\n");
                    tempOrig->name->orig_list = tempOrigInHash->next; //cambia testa della lista
                    printf("7.1.8\n");
                }
                printf("7.2\n");
                if(tempOrigInHash->next!=NULL){
                    tempOrigInHash->next->prec = tempOrigInHash->prec;
                }

                printf("8\n");
                tempOrigInHash->prec = NULL;         //sposta in cache
                tempOrigInHash->p=NULL;
                tempOrigInHash->next = cacheHashOrig;
                cacheHashOrig = tempOrigInHash;

               tempOrig->name = NULL;
               tempOrig->prec = NULL;
               tempOrig->dest = NULL;

               tempOrig = tempOrig->next;
            }

            printf("9\n");
            if(precOrig != NULL){
                precOrig->next = cacheOrig;
                cacheOrig = tempHead->first_orig;
            }

            tempHead->prec = NULL;
            tempHead->rel = NULL;
            tempHead->name = NULL;
            tempHead->next = cacheHead;
            cacheHead = tempHead;

            tempOrigInHash = NULL;
            tempOrig = NULL;
            precOrig = NULL;
            tempHead = NULL;

            tempHeadInHash->prec = NULL;
            tempHeadInHash->p = NULL;
            tempHeadInHash = tempHeadInHash->next;
        }
        printf("10\n");
        if(precHeadInHash!=NULL){
            precHeadInHash->next = cacheHashHead;
            cacheHashHead = p_user->head_list;
        }


    }else {
        printf("entità non monitorata\n");
    }

}

void delrel() {
    char orig[1024];
    char dest[1024];
    char rel[1024];

    entity *p_dest;
    entity *p_orig;
    head_in_hash *tempHeadInHash;
    orig_in_hash *tempOrigInHash;
    headReport *index_head;
    origRel *tempOrig;
    headReport *tempHead;
    typeRel *p_rel;
    typeRel *prec_rel;

    read(orig);
    read(dest);
    read(rel);

    findRel(rel,&p_rel,&prec_rel);
    findInHash(dest,&p_dest);
    findInHash(orig,&p_orig);

    if(p_dest!=NULL && p_orig!=NULL && p_rel!=NULL) { //esistono orig dest e rel
        tempHeadInHash = p_dest->head_list;
        while(tempHeadInHash!=NULL && tempHeadInHash->p->rel!=p_rel ) { //cerco dest nella entity
            tempHeadInHash = tempHeadInHash->next;
        }
        if(tempHeadInHash == NULL) {        //dest non esiste in quella typerel
            printf("relazione non esistente\n");
        }else {
            tempOrig = tempHeadInHash->p->first_orig;
            while(tempOrig != NULL && tempOrig->name!=p_orig){
                tempOrig = tempOrig->next;
            }
            if(tempOrig == NULL){ //dest esiste  ma non esiste orig in quella typerel
                printf("relanzione non esistente\n");
            }else {//MODIFICA STRUTTURA

                tempHead = tempHeadInHash->p;

                //cerca orig_in_hash in p_orig
                tempOrigInHash = p_orig->orig_list;
                while(tempOrigInHash!=NULL && tempOrigInHash->p != tempOrig ) {
                    tempOrigInHash = tempOrigInHash->next;
                }

                //elimina da entity head_in_hash && orig_in_hash
                if(tempOrigInHash->prec!=NULL){
                    tempOrigInHash->prec->next = tempOrigInHash->next;
                }else {
                    p_orig->orig_list = tempOrigInHash->next; //cambia testa della lista
                }

                if(tempOrigInHash->next!=NULL){
                    tempOrigInHash->next->prec = tempOrigInHash->prec;
                }

                tempOrigInHash->prec = NULL;         //sposta in cache
                tempOrigInHash->p=NULL;
                tempOrigInHash->next = cacheHashOrig;
                cacheHashOrig = tempOrigInHash;



                if(tempHeadInHash->prec!=NULL){
                    tempHeadInHash->prec->next = tempHeadInHash->next;
                }else {
                    p_dest->head_list = tempHeadInHash->next; //cambia testa della lista
                }

                if(tempHeadInHash->next!=NULL){
                    tempHeadInHash->next->prec = tempHeadInHash->prec;
                }

                tempHeadInHash->prec = NULL;         //sposta cache
                tempHeadInHash->p=NULL;
                tempHeadInHash->next = cacheHashHead;
                cacheHashHead = tempHeadInHash;



                //elimina origEnt nella struttura delle rel
                if(tempOrig->prec!=NULL){
                    tempOrig->prec->next = tempOrig->next;
                }else {
                    tempOrig->dest->first_orig = tempOrig->next; //cambia testa della lista
                }

                if(tempOrig->next!=NULL){
                    tempOrig->next->prec = tempOrig->prec;
                }

                tempOrig->prec = NULL;         //sposta orig in cache
                tempOrig->dest = NULL;
                tempOrig->name = NULL;
                tempOrig->next = cacheOrig;
                cacheOrig = tempOrig;

                //decrementa di una relazione dest
                tempHead->n_rel--;
                if(tempHead->n_rel>0){ //riordina dest

                    index_head = tempHead;
                    while (index_head->next != NULL && (index_head->next->n_rel>tempHead->n_rel || (index_head->next->n_rel==tempHead->n_rel && strcmp(tempHead->name->name,index_head->name->name)>0)) ) {
                        index_head = index_head->next;
                    }

                    if(index_head != tempHead) { //va spostato

                        //estrazione
                        if(tempHead->prec!=NULL){
                            tempHead->prec->next = tempHead->next;
                        }else {
                            tempHead->rel->head_list = tempHead->next; //cambia testa della lista
                        }
                        tempHead->next->prec = tempHead->prec;

                        //riposizionamento
                        tempHead->prec=index_head;
                        tempHead->next=index_head->next;
                        if(index_head->next != NULL) index_head->next->prec = tempHead;
                        index_head->next = tempHead;

                    }

                }else {//elimina dalla lista il dest
                    if(tempHead->prec!=NULL){
                        tempHead->prec->next = tempHead->next;
                    }else {
                        tempHead->rel->head_list = tempHead->next; //cambia testa della lista
                    }

                    if(tempHead->next!=NULL){
                        tempHead->next->prec = tempHead->prec;
                    }

                    tempHead->prec = NULL;
                    tempHead->rel = NULL;
                    tempHead->name = NULL;
                    tempHead->next = cacheHead;
                    cacheHead = tempHead;

                }
                printf("DELREL modifica struttura\n");
            }
        }
    }else { //non esiste typerel
        printf("relazione non esistente\n");
    }

}

void report(){
    typeRel *tempRel = NULL;
    headReport *tempHead = NULL;
    int flagfirst = 0;
    int flagspace;
    int n_rel = -1;

    for(int i=0 ; i<78; i++) {
        flagspace = 0;
        tempRel = array_lex[i];
        while (tempRel!=NULL){
            if(tempRel->head_list!=NULL) {
                if(flagspace!=0) printf(" ");
                printf("\"%s\" ",tempRel->id_rel);
                tempHead = tempRel->head_list;
                n_rel = tempHead->n_rel;
                printf("\"%s\" ",tempHead->name->name);
                while (tempHead->next!= NULL && tempHead->next->n_rel == n_rel) {
                    tempHead = tempHead->next;
                    printf("\"%s\" ",tempHead->name->name);
                }
                printf("%d;",n_rel);
                flagspace = 1;
            }
            tempRel =tempRel->next;
        }
    }
    printf("\n");
}

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