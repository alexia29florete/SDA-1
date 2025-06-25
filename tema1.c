/* FLORETE Alexia-Maria - 315CC */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct page
{
    int id;
    char url[50];
    char *description;
} page;

typedef struct node 
{
    page *page_data;
    struct node *next;
} node;

typedef struct stack 
{
    node *top;
} stack;

typedef struct tab
{
    int id;
    page *currentPage;
    stack *backwardStack;
    stack *forwardStack;
} tab;

typedef struct tabNode
{
    tab *tab;
    struct tabNode *next, *prev;
} tabNode;

typedef struct tabList
{
    struct tabNode *santinela;
} tabList;

typedef struct browser
{
    tab *current;
    tabList *list;
} browser;

//adaug informatiile specifice paginii initiale cu id = 0
page *pagina_initiala()
{
    page *pagina_0 = (page *)malloc(sizeof(page));
    if (!pagina_0)
    {
        return NULL;
    }
    pagina_0->id = 0;
    strcpy(pagina_0->url, "https://acs.pub.ro/");
    pagina_0->description = malloc(strlen("Computer Science") + 1);
    strcpy(pagina_0->description, "Computer Science");
    return pagina_0;
}

tab *creare_tab(int id, page *pagina_0)
{
    tab *new_tab = malloc(sizeof(tab));
    if (!new_tab)
    {
        return NULL;
    }
    new_tab->id = id;
    new_tab->currentPage = pagina_0;

    new_tab->backwardStack = (stack*)malloc(sizeof(stack));
    new_tab->forwardStack = (stack*)malloc(sizeof(stack));
    if (!new_tab->backwardStack || !new_tab->forwardStack) 
    {
        free(new_tab->backwardStack);
        free(new_tab->forwardStack);
        free(new_tab);
        return NULL;
    }

    //stiva este goala
    new_tab->backwardStack->top = NULL;
    new_tab->forwardStack->top = NULL;

    return new_tab;
}

//initializez lista dublu inlantuita circulara cu santinela
tabList *initializare_tablist()
{
    tabList *list = (tabList *)malloc(sizeof(tabList));
    if (!list) 
    {
        return NULL;
    }
    list->santinela = (tabNode *)malloc(sizeof(tabNode));
    if (!list->santinela) 
    {
        free(list); 
        return NULL;
    }
    list->santinela->next = list->santinela->prev = list->santinela;
    return list;
}

void push(stack *stiva, page *pages)
{
    node *new_element = (node *)malloc(sizeof(node));
    if (!new_element)
    {
        return;
    }
    new_element->page_data = pages;
    new_element->next = stiva->top;
    stiva->top = new_element;
}

page *pop(stack *stiva)
{
    if (stiva->top == NULL)
    {
        return NULL;
    }

    node *curent = stiva->top;
    page *pagina = curent->page_data;
    stiva->top = stiva->top->next;
    free(curent);
    return pagina;
}

void inversare_stiva(stack *stiva) 
{
    node *prev = NULL;
    node *current = stiva->top;
    node *next = NULL;

    while (current != NULL) 
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    stiva->top = prev;
}

void free_stack_with_pages(stack *stiva) 
{
    node *curent = stiva->top;
    while (curent) 
    {
        node *temp = curent;
        if (temp->page_data && temp->page_data->id == 0) 
        {
            free(temp->page_data->description);
            free(temp->page_data);
        }
        curent = curent->next;
        free(temp);
    }
    free(stiva);
}

//adaug la sfasitul listei dublu inlantuite circulara cu santinela elemente noi
void adaugare_tab(tabList *list, tab *new_tab) 
{
    tabNode *new_node = (tabNode *)malloc(sizeof(tabNode));
    if (!new_node) 
    {
        return;
    }
    new_node->tab = new_tab;
    tabNode *curent = list->santinela->prev;
    new_node->next = list->santinela;
    new_node->prev = curent;
    curent->next = new_node;
    list->santinela->prev = new_node;
}

void close_tab(FILE *output_file, browser *Browser)
{
    if (Browser->current == NULL)
    {
        return;
    }

    //verific daca tab-ul curent are id = 0
    if (Browser->current->id == 0)
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }

    tabNode *curent = Browser->list->santinela->next;

    //caut tab-ul curent in lista de taburi
    while (curent != Browser->list->santinela && curent->tab != Browser->current)
    {
        curent = curent->next;
    }

    //daca tab-ul nu a fost gasit
    if (curent == Browser->list->santinela)
    {
        return;
    }

    //actualizez tab-ul curent (cel aflat in stanga celui sters)
    tabNode *tab_stanga = curent->prev;
    if (tab_stanga != Browser->list->santinela)
    {
        Browser->current = tab_stanga->tab;
    }
    else
    {
        Browser->current = NULL;
    }

    //sterg tabul curent
    curent->prev->next = curent->next;
    curent->next->prev = curent->prev;

    //eliberez memoria 
    if (curent->tab->currentPage != NULL && curent->tab->currentPage->id == 0) 
    {
        free(curent->tab->currentPage->description);
        free(curent->tab->currentPage);
    }
    free_stack_with_pages(curent->tab->backwardStack);
    free_stack_with_pages(curent->tab->forwardStack);
    free(curent->tab);
    free(curent);
}

void open_tab_id(FILE *output_file, browser *Browser, int id)
{
    tabNode *curent = Browser->list->santinela->next;

    //caut tabul cu id-ul cerut
    while(curent != Browser->list->santinela)
    {
        if (curent->tab->id == id)
        {
            Browser->current = curent->tab;
            break;
        }
        curent = curent->next;
    }

    if (curent == Browser->list->santinela)
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }
}

void next(browser *Browser)
{
    tabNode *curent = Browser->list->santinela->next;

    //caut tab-ul curent
    while(curent != Browser->list->santinela && curent->tab != Browser->current)
    {
        curent = curent->next;
    }

    //setez tab-ul urmatorul(cel din dreapta) ca tab curent
    if (curent != Browser->list->santinela)
    {
        tabNode *next_node = curent->next;
        //curent->next poate deveni santinela si nu vreau asta
        if (next_node == Browser->list->santinela)
        {
            next_node = next_node->next;
        }
        Browser->current = next_node->tab;
    }
}

void prev(browser *Browser)
{
    tabNode *curent = Browser->list->santinela->next;

    //caut tab-ul curent
    while(curent != Browser->list->santinela && curent->tab != Browser->current)
    {
        curent = curent->next;
    }

    //setez tab-ul anterior (cel din stanga) ca tab curent
    if (curent != Browser->list->santinela)
    {
        tabNode *prev_node = curent->prev;
        //curent->prev poate deveni santinela si nu vreau asta
        if (prev_node == Browser->list->santinela)
        {
            prev_node = prev_node->prev;
        }
        Browser->current = prev_node->tab;
    }
}

void page_id(FILE *output_file, browser *Browser, page *pages, int id, int nr_pagini)
{
    page *pagina_curenta = NULL;
    //caut pagina cu id-ul cerut in vectorul de pagini
    for(int i = 0; i < nr_pagini; i++)
    {
        if (pages[i].id == id) 
        {
            pagina_curenta = &pages[i]; //&pages[i] este adresa structurii page de la indexul i
            break;
        }
    }

    if (!pagina_curenta)
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }

    //adaug pagina curenta in stiva backward
    push(Browser->current->backwardStack, Browser->current->currentPage);

    Browser->current->currentPage = pagina_curenta;
}

void backward(FILE *output_file, browser *Browser)
{
    //daca stiva este goala
    if (Browser->current->backwardStack->top == NULL)
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }

    //adaug pagina curenta in stiva forward
    push(Browser->current->forwardStack, Browser->current->currentPage);

    //pagina curenta devine ultimul elemet adaugat in stiva de backward
    Browser->current->currentPage = pop(Browser->current->backwardStack);
}

void forward(FILE *output_file, browser *Browser)
{
    //daca stiva este goala
    if (Browser->current->forwardStack->top == NULL)
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }

    //adaug pagina curenta in stiva backward
    push(Browser->current->backwardStack, Browser->current->currentPage);

    //pagina curenta devine ultimul elemet adaugat in stiva de forward
    Browser->current->currentPage = pop(Browser->current->forwardStack);
}

void print(FILE *output_file, browser *Browser)
{
    tabNode *curent = Browser->list->santinela->next;

    //caut nodul din lista care contine tabul curent
    while (curent != Browser->list->santinela && curent->tab != Browser->current) 
    {
        curent = curent->next;
    }
    if (curent == Browser->list->santinela) 
    {
        return;
    }

    tabNode *parcurgere = curent;
    //plec de la nodul curent, ma duc in dreapta pana cand ma intorc la el
    int este_singur = 1;
    do 
    {
        if (!este_singur) 
        {
            fprintf(output_file, " ");
        }
        fprintf(output_file, "%d", parcurgere->tab->id);
        este_singur = 0;
        parcurgere = parcurgere->next;
        if (parcurgere == Browser->list->santinela)
        {
            parcurgere = parcurgere->next;
        }
    } while(parcurgere != curent);
    fprintf(output_file, "\n%s\n", Browser->current->currentPage->description);
}

void print_history(FILE *output_file, browser *Browser, int id)
{
    tabNode *curent = Browser->list->santinela->next;

    //caut tab-ul cu id-ul specificat
    while (curent != Browser->list->santinela && curent->tab->id != id) 
    {
        curent = curent->next;
    }

    if (curent == Browser->list->santinela) 
    {
        fprintf(output_file, "403 Forbidden\n");
        return;
    }

    //afisez url-urile din forward de la prima adaugata (asa ca inversez stiva de forward)
    inversare_stiva(curent->tab->forwardStack);
    
    node *temp_forward = curent->tab->forwardStack->top;
    while(temp_forward != NULL)
    {
        fprintf(output_file, "%s\n", temp_forward->page_data->url);
        temp_forward = temp_forward->next;
    }
    inversare_stiva(curent->tab->forwardStack);

    //afisez pagina curenta
    fprintf(output_file, "%s\n", curent->tab->currentPage->url);

    //afisez url-urile din backward de la ultima adaugata
    node *temp_backward = curent->tab->backwardStack->top;
    while(temp_backward != NULL)
    {
        fprintf(output_file, "%s\n", temp_backward->page_data->url);
        temp_backward = temp_backward->next;
    }
}

void eliberare_memorie(int numar_pagini, browser *Browser, page *pages, tabList *lista_taburi)
{
    for (int i = 0; i < numar_pagini; i++) 
    {
        free(pages[i].description);
    }
    
    tabNode *curent = lista_taburi->santinela->next;
    if (curent != lista_taburi->santinela) 
    {
        do 
        {
            tabNode *next = curent->next;
            if (curent->tab->currentPage && curent->tab->currentPage->id == 0) 
            {
                free(curent->tab->currentPage->description);
                free(curent->tab->currentPage);
            }

            free_stack_with_pages(curent->tab->backwardStack);
            free_stack_with_pages(curent->tab->forwardStack);
            free(curent->tab);
            free(curent);

            curent = next;
        } while (curent != lista_taburi->santinela);
    }

    free(lista_taburi->santinela);
    free(lista_taburi);
    free(Browser);
    free(pages);
}

int main()
{
    tabList *lista_taburi = initializare_tablist();
    browser *Browser = (browser *)malloc(sizeof(browser));
    Browser->list = lista_taburi;
    Browser->current = NULL;

    FILE *input_file = fopen("tema1.in", "r");
    FILE *output_file = fopen("tema1.out", "w");

    int numar_pagini;
    fscanf(input_file, "%d", &numar_pagini);
    fgetc(input_file);

    page *pages = (page *)malloc(numar_pagini * sizeof(page));
    for(int i = 0; i < numar_pagini; i++)
    {
        fscanf(input_file, "%d", &pages[i].id);
        fgetc(input_file);
        fgets(pages[i].url, 50, input_file);
        int lungime = strlen(pages[i].url);
        if (pages[i].url[lungime - 1] == '\n') 
        {
			pages[i].url[lungime - 1] = '\0';
			lungime--;
		}

        char description[100];
        fgets(description, 100, input_file);
        int lungime_descriere = strlen(description);
        if (description[lungime_descriere - 1] == '\n')
        {
            description[lungime_descriere - 1] = '\0';
            lungime_descriere--;
        }

        pages[i].description = (char *)malloc((lungime_descriere + 1) * sizeof(char)); //lungime_descriere + 1 pt a pune '\0'
        strcpy(pages[i].description, description);
    }

    int id_generator = 0;
    tab *primul_tab = creare_tab(id_generator, pagina_initiala());
    adaugare_tab(lista_taburi, primul_tab);
    Browser->current = primul_tab;

    int nr_operatii;
    fscanf(input_file, "%d", &nr_operatii);
    fgetc(input_file);
    for(int i = 0; i < nr_operatii; i++)
    {
        char operatia[20];
        while (fscanf(input_file, "%s", operatia) == 1) 
        {
            if (strcmp(operatia, "NEW_TAB") == 0)
            {
                //adaug taburi
                int new_id = ++id_generator;

                tab *nou_tab = creare_tab(new_id, pagina_initiala());
                adaugare_tab(lista_taburi, nou_tab);
                Browser->current = nou_tab;
            }
            else if (strcmp(operatia, "CLOSE") == 0)
            {
                //inchid taburile, le sterg din lista de taburi si din browser
                close_tab(output_file, Browser);

            }
            else if (strcmp(operatia, "OPEN") == 0)
            {
                int id;
                fscanf(input_file, "%d", &id);
                open_tab_id(output_file, Browser, id); 
            }
            else if (strcmp(operatia, "NEXT") == 0)
            {
                next(Browser);
            }
            else if (strcmp(operatia, "PREV") == 0)
            {
                prev(Browser);
            }
            else if (strcmp(operatia, "PAGE") == 0)
            {
                int id;
                fscanf(input_file, "%d", &id);
                page_id(output_file, Browser, pages, id, numar_pagini);
            }
            else if (strcmp(operatia, "PRINT") == 0)
            {
                //afisez
                print(output_file, Browser);
            }
            else if (strcmp(operatia, "BACKWARD") == 0)
            {
                backward(output_file, Browser);
            }
            else if (strcmp(operatia, "FORWARD") == 0)
            {
                forward(output_file, Browser);
            }
            else if (strcmp(operatia, "PRINT_HISTORY") == 0)
            {
                int id;
                fscanf(input_file, "%d", &id);
                print_history(output_file, Browser, id);
            }
        }
    }

    fclose(input_file);
    fclose(output_file);

    eliberare_memorie(numar_pagini, Browser, pages, lista_taburi);
    return 0;
}