#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>

typedef struct Client Client;
typedef struct Nod Nod;
typedef struct Coada Coada;
typedef struct HashTable HashTable;

struct Client {
    char* nume;           // pointer 1
    char* prenume;        // pointer 2
    char* email;          // pointer 3
    int varsta;
    float buget;
    long telefon;
    int fidelitate;       // scor de fidelitate
};

struct Nod {
    Client info;
    Nod* next;
};

struct Coada {
    Nod* prim;
    Nod* ultim;
};

struct HashTable {
    int dimensiune;
    Nod** vector;
};

void afisareClient(Client client) {
    printf("\nClientul %s %s, email: %s, varsta: %d, buget: %.2f lei, telefon: %ld, fidelitate: %d",
        client.nume, client.prenume, client.email, client.varsta, client.buget, client.telefon, client.fidelitate);
}

Client citireClient(FILE* fileStream) {
    Client client;
    char buffer[100];

    fgets(buffer, 100, fileStream);
    char* nume = strtok(buffer, "\n");
    client.nume = (char*)malloc(sizeof(char) * (strlen(nume) + 1));
    strcpy(client.nume, nume);

    fgets(buffer, 100, fileStream);
    char* prenume = strtok(buffer, "\n");
    client.prenume = (char*)malloc(sizeof(char) * (strlen(prenume) + 1));
    strcpy(client.prenume, prenume);

    fgets(buffer, 100, fileStream);
    char* email = strtok(buffer, "\n");
    client.email = (char*)malloc(sizeof(char) * (strlen(email) + 1));
    strcpy(client.email, email);

    fgets(buffer, 10, fileStream);
    client.varsta = atoi(buffer);

    fgets(buffer, 15, fileStream);
    client.buget = atof(buffer);

    fgets(buffer, 15, fileStream);
    client.telefon = atol(buffer);

    fgets(buffer, 10, fileStream);
    client.fidelitate = atoi(buffer);

    return client;
}

Client copyClient(Client sursa) {
    Client destinatie;

    destinatie.nume = (char*)malloc(sizeof(char) * (strlen(sursa.nume) + 1));
    strcpy(destinatie.nume, sursa.nume);

    destinatie.prenume = (char*)malloc(sizeof(char) * (strlen(sursa.prenume) + 1));
    strcpy(destinatie.prenume, sursa.prenume);

    destinatie.email = (char*)malloc(sizeof(char) * (strlen(sursa.email) + 1));
    strcpy(destinatie.email, sursa.email);

    destinatie.varsta = sursa.varsta;
    destinatie.buget = sursa.buget;
    destinatie.telefon = sursa.telefon;
    destinatie.fidelitate = sursa.fidelitate;

    return destinatie;
}

void dezalocareClient(Client* client) {
    free(client->nume);
    free(client->prenume);
    free(client->email);
}

Coada initCoada() {
    Coada coada;
    coada.prim = NULL;
    coada.ultim = NULL;
    return coada;
}

int esteCoadaGoala(Coada coada) {
    return coada.prim == NULL;
}

void inserareCoada(Coada* coada, Client client) {
    Nod* nou = (Nod*)malloc(sizeof(Nod));
    nou->info = copyClient(client);
    nou->next = NULL;

    if (esteCoadaGoala(*coada)) {
        coada->prim = nou;
        coada->ultim = nou;
    }
    else {
        coada->ultim->next = nou;
        coada->ultim = nou;
    }
}

Client extragereCoada(Coada* coada) {
    if (!esteCoadaGoala(*coada)) {
        Client client = copyClient(coada->prim->info);

        Nod* temp = coada->prim;
        coada->prim = coada->prim->next;

        dezalocareClient(&temp->info);
        free(temp);

        if (coada->prim == NULL) {
            coada->ultim = NULL;
        }

        return client;
    }

    Client clientGol;
    clientGol.nume = NULL;
    clientGol.prenume = NULL;
    clientGol.email = NULL;
    return clientGol;
}

void citireCoada(FILE* file, Coada* coada) {
    if (file == NULL) {
        return;
    }

    while (!feof(file)) {
        Client client = citireClient(file);
        inserareCoada(coada, client);
        dezalocareClient(&client);
    }
}

void afisareCoada(Coada coada) {
    printf("\n=== Afisare Coada ===");
    Nod* temp = coada.prim;
    while (temp != NULL) {
        afisareClient(temp->info);
        temp = temp->next;
    }
}

Client ultimClientBugetMare(Coada* coada) {
    Coada coadaTemp = initCoada();
    Client ultimClient;
    ultimClient.nume = NULL;
    ultimClient.prenume = NULL;
    ultimClient.email = NULL;

    while (!esteCoadaGoala(*coada)) {
        Client client = extragereCoada(coada);

        if (client.buget > 1000) {
            if (ultimClient.nume != NULL) {
                dezalocareClient(&ultimClient);
            }
            ultimClient = copyClient(client);
        }

        inserareCoada(&coadaTemp, client);
        dezalocareClient(&client);
    }

    while (!esteCoadaGoala(coadaTemp)) {
        Client client = extragereCoada(&coadaTemp);
        inserareCoada(coada, client);
        dezalocareClient(&client);
    }

    return ultimClient;
}

int hashFunction(Client client, int dimensiune) {
    int suma = 0;
    for (int i = 0; i < strlen(client.nume); i++) {
        suma += client.nume[i];
    }
    return suma % dimensiune;
}

HashTable initHashTable(int dimensiune) {
    HashTable tabela;
    tabela.dimensiune = dimensiune;
    tabela.vector = (Nod**)malloc(sizeof(Nod*) * dimensiune);

    for (int i = 0; i < dimensiune; i++) {
        tabela.vector[i] = NULL;
    }

    return tabela;
}

void inserareLaSfarsit(Nod** cap, Client client) {
    Nod* nou = (Nod*)malloc(sizeof(Nod));
    nou->info = copyClient(client);
    nou->next = NULL;

    if (*cap == NULL) {
        *cap = nou;
    }
    else {
        Nod* temp = *cap;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = nou;
    }
}

void inserareInHashTable(HashTable tabela, Client client) {
    if (tabela.dimensiune > 0) {
        int pozitie = hashFunction(client, tabela.dimensiune);
        if (pozitie >= 0 && pozitie < tabela.dimensiune) {
            inserareLaSfarsit(&(tabela.vector[pozitie]), client);
        }
    }
}

HashTable conversieCoada2HashTable(Coada* coada, int dimensiune) {
    HashTable tabela = initHashTable(dimensiune);
    Coada coadaTemp = initCoada();

    while (!esteCoadaGoala(*coada)) {
        Client client = extragereCoada(coada);
        inserareInHashTable(tabela, client);
        inserareCoada(&coadaTemp, client);
        dezalocareClient(&client);
    }

    while (!esteCoadaGoala(coadaTemp)) {
        Client client = extragereCoada(&coadaTemp);
        inserareCoada(coada, client);
        dezalocareClient(&client);
    }

    return tabela;
}

Client* getClientiByNume(HashTable tabela, char* nume, int* nrClienti) {
    *nrClienti = 0;
    Client* vectorClienti = NULL;

    for (int i = 0; i < tabela.dimensiune; i++) {
        Nod* temp = tabela.vector[i];
        while (temp != NULL) {
            if (strcmp(temp->info.nume, nume) == 0) {
                (*nrClienti)++;
                vectorClienti = (Client*)realloc(vectorClienti, (*nrClienti) * sizeof(Client));
                vectorClienti[(*nrClienti) - 1] = copyClient(temp->info);
            }
            temp = temp->next;
        }
    }

    return vectorClienti;
}

void afisareHashTable(HashTable tabela) {
    printf("\n=== Afisare Tabela de Dispersie ===");
    for (int i = 0; i < tabela.dimensiune; i++) {
        printf("\nPozitia: %d", i);
        Nod* temp = tabela.vector[i];
        while (temp != NULL) {
            afisareClient(temp->info);
            temp = temp->next;
        }
    }
}

void afisareVector(Client* vectorClienti, int nrClienti) {
    printf("\n=== Afisare Vector Clienti ===");
    for (int i = 0; i < nrClienti; i++) {
        afisareClient(vectorClienti[i]);
    }
}

void stergereClientiFidelitateMica(HashTable* tabela) {
    for (int i = 0; i < tabela->dimensiune; i++) {
        Nod* anterior = NULL;
        Nod* curent = tabela->vector[i];

        while (curent != NULL) {
            if (curent->info.fidelitate < 50) {
                if (anterior == NULL) {
                    tabela->vector[i] = curent->next;
                    dezalocareClient(&curent->info);
                    free(curent);
                    curent = tabela->vector[i];
                }
                else {
                    anterior->next = curent->next;
                    dezalocareClient(&curent->info);
                    free(curent);
                    curent = anterior->next;
                }
            }
            else {
                anterior = curent;
                curent = curent->next;
            }
        }
    }
}

void dezalocareNod(Nod** nod) {
    dezalocareClient(&(*nod)->info);
    free(*nod);
    *nod = NULL;
}

void dezalocareHashTable(HashTable* tabela) {
    for (int i = 0; i < tabela->dimensiune; i++) {
        while (tabela->vector[i] != NULL) {
            Nod* temp = tabela->vector[i];
            tabela->vector[i] = tabela->vector[i]->next;
            dezalocareNod(&temp);
        }
    }
    free(tabela->vector);
}

void dezalocareCoada(Coada* coada) {
    while (!esteCoadaGoala(*coada)) {
        Client client = extragereCoada(coada);
        dezalocareClient(&client);
    }
}

void dezalocareVector(Client* vector, int dimensiune) {
    for (int i = 0; i < dimensiune; i++) {
        dezalocareClient(&vector[i]);
    }
    free(vector);
}

void main() {
    printf("=== Program implementare structuri de date ===\n");

    FILE* file = fopen("C:/Users/oaset/source/repos/SDD_Seminar/SDD_Seminar/clienti.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului!\n");
        return;
    }

    Coada coada = initCoada();
    citireCoada(file, &coada);
    fclose(file);

    afisareCoada(coada);

    Client ultimClient = ultimClientBugetMare(&coada);
    if (ultimClient.nume != NULL) {
        printf("\n\n=== Ultimul client cu buget peste 1000 lei ===");
        afisareClient(ultimClient);
        dezalocareClient(&ultimClient);
    }
    else {
        printf("\n\nNu exista clienti cu buget peste 1000 lei!");
    }

    HashTable tabela = conversieCoada2HashTable(&coada, 10);

    afisareHashTable(tabela);

    int nrClienti = 0;
    char numeTarget[30] = "Popescu";
    Client* vectorClienti = getClientiByNume(tabela, numeTarget, &nrClienti);

    printf("\n\n=== Clienti cu numele %s ===", numeTarget);
    if (nrClienti > 0) {
        afisareVector(vectorClienti, nrClienti);
    }
    else {
        printf("\nNu exista clienti cu numele %s!", numeTarget);
    }

    printf("\n\n=== Stergere clienti cu fidelitate < 50 ===");
    stergereClientiFidelitateMica(&tabela);

    printf("\n\n=== Tabela dupa stergere ===");
    afisareHashTable(tabela);

    dezalocareVector(vectorClienti, nrClienti);
    dezalocareHashTable(&tabela);
    dezalocareCoada(&coada);

    printf("\n\nProgram finalizat cu succes!\n");

    
}
