#include <stdlib.h>
#include <stdio.h>
#include <graphviz/cgraph.h>
#include <string.h>
#include "mapa.h"

#define PASSOS 1  //numero de passos para simular 

typedef struct 
{
    Agrec_t h; 
    int x;
    int raiz;
    int marca_fronteira; //aqui cada vez que for pintar o mapa tem que zerar
    int estado; //para busca em profundidade
}mydata;  //records do grafo, x seria a cor do mapa
int tamanho_raiz;

Agraph_t* CopiaGrafo(Agraph_t *g)
{
    Agraph_t* copia;
    copia = agopen("G1", Agundirected, NULL);
    Agnode_t *n;
    Agnode_t *u, *v;
    Agedge_t *e,*e1;

    mydata *p, *aux;

    for (n = agfstnode(g); n; n = agnxtnode(g,n))
    {
        aux = (mydata*)aggetrec(n,"mydata",1); //aggetrec pega os dados do node
        v = agnode(copia,agnameof(n),TRUE);
        p=(mydata*) agbindrec(v,"mydata",sizeof(mydata),FALSE);  //adiciona a cor no vertice 
        p->x = aux->x;
        p->raiz = aux->raiz;
        p->marca_fronteira = aux->marca_fronteira;
        p->estado = aux->estado;
        for (e = agfstout(g,n); e; e = agnxtout(g,e))
        {
            aux = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
            u = agnode(copia,agnameof(e->node),TRUE);
            p=(mydata*) agbindrec(u,"mydata",sizeof(mydata),FALSE);  //adiciona a cor no vertice 
            p->x = aux->x;
            p->raiz = aux->raiz;
            p->marca_fronteira = aux->marca_fronteira;
            p->estado = aux->estado;

            e1 = agedge(copia,v,u,NULL,TRUE);
        }
    }
    return copia;
}

void pinta_mapa_grafo(Agraph_t* g, int cor)
{
    mydata *data1;
    Agnode_t* node;

    for (node = agfstnode(g); node; node = agnxtnode(g,node)) //for que passa por tds vertices
    {
        data1 = (mydata*)aggetrec(node,"mydata",1);
        
        if (data1->raiz == 1) 
        {
            data1->x = cor;
        }
        data1->marca_fronteira = 0;
    }
}

//funcao que calcula a fronteira da raiz
int calcula_fronteira(Agraph_t* g)
{
    mydata *data1, *data2;
    Agnode_t* node;
    Agedge_t *e;

    int fronteira_tamanho = 0;

    for (node = agfstnode(g); node; node = agnxtnode(g,node)) //for que passa por tds vertices
    {
        data1 = (mydata*)aggetrec(node,"mydata",1);

        for (e = agfstedge(g,node); e; e = agnxtedge(g,e,node))  //visita todos os vizinhos do vertice
        {
            data2 = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
            if((data1->raiz == 1) && (data2->raiz != 1) && (data2->marca_fronteira != 1)) //conta a fronteira
            {
                data2->marca_fronteira = 1;
                fronteira_tamanho++;
            } 
        }
    }
    return fronteira_tamanho;
}

//Nessa funcao vem como primeiro parametro o primeiro vertice, que é o primeira cor do mapa, que é a raiz
//e a partir desse vertice, usando uma busca em profundidade só nos vertices que tem a mesma cor, marcamos a raiz
//que é a cor "principal" do mapa
void marca_raiz_recursivo(Agraph_t* g,Agnode_t* node,mydata *data1)
{
    mydata *data2;
    Agedge_t *e;

    data1->estado = 1;  //marca vertice como visitado

    for(e = agfstedge(g,node); e; e = agnxtedge(g,e,node)) //visita todos os vertices vizinhos
    {
        data2 = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
        if((data1->raiz == 1) && (data2->x == data1->x ) && (data2->estado == 0)) //marca a area q esta pintada, que chamamos de raiz
        {
            data2->raiz = 1;
            tamanho_raiz ++;
            marca_raiz_recursivo(g,e->node,data2); //faz busca recursiva
        }
    }
}

//funcao que marca todos os vertices como nao vizitados e começa a recursividade para marcar a raiz
void marca_raiz(Agraph_t* g)
{
    mydata *data1;
    Agnode_t* node;
    tamanho_raiz = 1;

    //marca todos vertices como nao vizitados, estado = 0
    for (node = agfstnode(g); node; node = agnxtnode(g,node)) //for que passa por tds vertices
    {
        data1 = (mydata*)aggetrec(node,"mydata",1);
        data1->estado = 0;
    }

    node = agnode(g,"0",FALSE);   //pega primeira cor
    data1 = (mydata*)aggetrec(node,"mydata",1);

    marca_raiz_recursivo(g,node,data1); //manda o primeiro nodo que ja sabemos que é raiz e só faz uma busca em profundidade nesse nodo

    //return tamanho_raiz;
}

//busca por cores alem da fronteira
void busca_cores_recursivo(Agraph_t* g, Agnode_t* node, mydata *data1, int* vetor_cores_fronteira)
{
    mydata *data2;
    Agedge_t *e;

    for(e = agfstedge(g,node); e; e = agnxtedge(g,e,node)) //visita todos os vertices vizinhos
    {
        data2 = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
        if((data2->x == data1->x ) && (data2->estado != 2)) 
        {
            data2->estado = 2;
            vetor_cores_fronteira[data2->x]++;
            busca_cores_recursivo(g,e->node,data2, vetor_cores_fronteira); //faz busca recursiva
        }
    }
}

void simula_passo(Agraph_t* g, int ncores, int cor_simula, int* resultado)
{
    Agraph_t *copia_grafo;
    copia_grafo = CopiaGrafo(g);

    int i, fab;
    int cor;
    int numero_cores;
    int *vetor_cores_fronteira;

    mydata *data1, *data2;
    Agnode_t* node;
    Agedge_t *e;
    int aux = 1;
    int aux2 = 0;

    pinta_mapa_grafo(copia_grafo,cor_simula);

    vetor_cores_fronteira = (int*) malloc((ncores + 1) * sizeof(int));
    node = agnode(copia_grafo,"0",FALSE);   //pega primeira cor
    data1 = (mydata*)aggetrec(node,"mydata",1);
    cor = data1->x;
    
    marca_raiz(copia_grafo); 

    while(aux <= PASSOS)  //enquanto a fronteira for maior que zero significa que ainda nao completou o mapa
    {
        for(i = 1; i <= ncores; i++) //zera vetor com as cores para fazer novamente
            vetor_cores_fronteira[i] = 0;

        for (node = agfstnode(copia_grafo); node; node = agnxtnode(copia_grafo,node)) //for que passa por tds vertices
        {
            data1 = (mydata*)aggetrec(node,"mydata",1);
            for (e = agfstedge(copia_grafo,node); e; e = agnxtedge(copia_grafo,e,node))  //visita todos os vizinhos do vertice
            {
                data2 = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
                if((data1->raiz == 1) && (data2->raiz != 1) && (data2->estado != 2)) //
                {
                    data2->estado = 2;  //reutilizamos uma das variaveis da struct para marcar que esta cor ja contou
                    vetor_cores_fronteira[data2->x]++; //aqui o indice é a cor e dentro é o contador, cor = 3, vetor_cores_fronteira[3] = 2 por exemplo
                    //aqui faz uma busca em profundidade para ver se tem mais cores iguais alem da borda
                    busca_cores_recursivo(copia_grafo,e->node,data2,vetor_cores_fronteira);
                } 
            }
        }

        numero_cores = 0;
        for(fab = 1; fab <= ncores; fab++)                       //percorre vetor com as cores da fronteira 
        {                                                        //e escolhe a que esta em maior quantidade
            if(vetor_cores_fronteira[fab] > numero_cores) 
            {
                numero_cores = vetor_cores_fronteira[fab];
                cor = fab;
            }
        }

        resultado[aux2] = cor;

        aux2++;

        pinta_mapa_grafo(copia_grafo,cor);
        marca_raiz(copia_grafo);
        aux ++;
    }

    resultado[PASSOS] =  tamanho_raiz;
    free(vetor_cores_fronteira);

}

void monta_estrategia(Agraph_t* g, int ncores, tmapa *m)
{
    int i , fab;
    int cor, cor2;
    int numero_cores;
    int *vetor_cores_fronteira;
    int *vetor_simula = malloc(PASSOS * sizeof(vetor_simula));
    int *vetor_simula2 = malloc(PASSOS * sizeof(vetor_simula2));
    tplano *p;

    p = aloca_plano(m);

    mydata *data1, *data2;
    Agnode_t* node;
    Agedge_t *e;
    int aux = 1;

    vetor_cores_fronteira = (int*) malloc((ncores + 1) * sizeof(int));
    node = agnode(g,"0",FALSE);   //pega primeira cor
    data1 = (mydata*)aggetrec(node,"mydata",1);
    cor = data1->x;

    marca_raiz(g); 

    while(aux > 0)  //enquanto a fronteira for maior que zero significa que ainda nao completou o mapa
    {
        for(i = 1; i <= ncores; i++) //zera vetor com as cores para fazer novamente
            vetor_cores_fronteira[i] = 0;

        for (node = agfstnode(g); node; node = agnxtnode(g,node)) //for que passa por tds vertices
        {
            data1 = (mydata*)aggetrec(node,"mydata",1);
            for (e = agfstedge(g,node); e; e = agnxtedge(g,e,node))  //visita todos os vizinhos do vertice
            {
                data2 = (mydata*)aggetrec(e->node,"mydata",1); //aggetrec pega os dados do node
                if((data1->raiz == 1) && (data2->raiz != 1) && (data2->estado != 2)) //
                {
                    data2->estado = 2;  //reutilizamos uma das variaveis da struct para marcar que esta cor ja contou
                    vetor_cores_fronteira[data2->x]++; //aqui o indice é a cor e dentro é o contador, cor = 3, vetor_cores_fronteira[3] = 2 por exemplo
                    //aqui faz uma busca em profundidade para ver se tem mais cores iguais alem da borda
                    busca_cores_recursivo(g,e->node,data2,vetor_cores_fronteira);
                } 
            }
        }

        numero_cores = 0;

        for(fab = 1; fab <= ncores; fab++)                   //percorre vetor com as cores da fronteira
        {                                                    //e escolhe as duas maiores cores para simular com essas duas cores
            if(vetor_cores_fronteira[fab] >= numero_cores)   //e depois vai escolher qual das duas é a melhor 
            {
                numero_cores = vetor_cores_fronteira[fab];
                cor2 = cor;
                cor = fab;
            }
        }

        simula_passo(g, ncores, cor, vetor_simula);          //simula para as duas maiores cores
        simula_passo(g, ncores, cor2, vetor_simula2);

        if(vetor_simula[PASSOS] > vetor_simula2[PASSOS]) //se tam raiz1 > tamraiz2
        {
            pinta_mapa_grafo(g,cor);
            marca_raiz(g); 
            insere_plano(p,cor); //insere a cor no plano, que é o resultado
            for(i=0;i<PASSOS;i++)
            {
                pinta_mapa_grafo(g,vetor_simula[i]);
                marca_raiz(g); 
                insere_plano(p,vetor_simula[i]); //insere a cor no plano, que é o resultado
            }           
        }else
        {
            pinta_mapa_grafo(g,cor2);
            marca_raiz(g); 
            insere_plano(p,cor2); //insere a cor no plano, que é o resultado
            for(i=0;i<PASSOS;i++)
            {
                pinta_mapa_grafo(g,vetor_simula2[i]);
                marca_raiz(g); 
                insere_plano(p,vetor_simula2[i]); //insere a cor no plano, que é o resultado
            }
        }
        aux = calcula_fronteira(g);
    }
    free(vetor_simula);
    free(vetor_simula2);
    free(vetor_cores_fronteira);
    mostra_plano(p);
}


Agraph_t *carrega_mapa_grafo(tmapa *m, Agraph_t *g)
{
    int i, j;

    scanf("%d", &(m->nlinhas));
    scanf("%d", &(m->ncolunas));
    scanf("%d", &(m->ncores));

    int numero_vertices = 0;
    int vertice_ant;
    char str[20],str1[20];                         //podemos ter um problema em relação ao tamanho dos mapas

    Agnode_t *n , *v;
    Agedge_t *e;
    g = agopen("G", Agundirected, NULL);

    mydata *p;

    m->mapa = (int**) malloc(m->nlinhas * sizeof(int*));
    for(i = 0; i < m->nlinhas; i++)
    {
        m->mapa[i] = (int*) malloc(m->ncolunas * sizeof(int));
        for(j = 0; j < m->ncolunas; j++)
        {
            scanf("%d", &(m->mapa[i][j]));
            sprintf(str, "%d", numero_vertices); //aqui converte int pra char para dar nomes aos vertices

            n = agnode(g,str,TRUE);
            //se j = 0 significa que é nova linha do mapa, logo nao há uma aresta a menos
            if (j > 0)
            {  
                vertice_ant = numero_vertices - 1;  //coloca aresta nos vizinhos da linha
                sprintf(str1, "%d", vertice_ant);
                v = agnode(g, str1, FALSE);
                e = agedge(g, v, n, NULL, TRUE );
            }
            //se nao for a primeira linha, coloca aresta no vertice que esta na coluna anterior
            if (i > 0)
            {
                vertice_ant = numero_vertices - m->ncolunas;
                sprintf(str1, "%d", vertice_ant);
                v = agnode(g, str1, FALSE);
                e = agedge(g, v, n, NULL, TRUE );
            }
            p=(mydata*) agbindrec(n,"mydata",sizeof(mydata),FALSE);  //adiciona a cor no vertice 
            p->x = m->mapa[i][j];
            if ((i == 0) && (j == 0)) p->raiz = 1; //mostra qual é a raiz, q é a primeira cor
            numero_vertices++;
        }
    }

    return g;
}

int main(int argc, char **argv) 
{
    tmapa m;  //usamos para fazer o plano, na funcao aloca_plano

    Agraph_t *g;

    g = carrega_mapa_grafo(&m,g);  //carrega o mapa em um grafo

    monta_estrategia(g,m.ncores, &m); //principal função 

    return 0;
}