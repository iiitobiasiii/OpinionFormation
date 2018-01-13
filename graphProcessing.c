#include <stdio.h>
#include <stdlib.h>


#define NNodes 10
#define NEdges 5
#define NOpinions 2

typedef struct Graph{
    struct Node* NList;
    int Adj_Matrix[NNodes][NNodes];
} Graph;


typedef struct Node{
    int opinion;
    struct Node* next;
} Node;

typedef struct Edge{
    int sr;
} Edge;



void create_graph()
{
    Graph *G = malloc(sizeof(Graph));
    if (G == NULL)
    {   
        printf("malloc of Graph failed \n");
        return;
    }
    G->NList = malloc(NNodes * sizeof(Node*));
    if (G->NList == NULL)
    {
        printf("malloc of NList of Graph failen \n");
        free(G);
        return;
    } 
    int Curr_NEdges = 0;
    float max_Edges = (NNodes * (NNodes -1))/2;
    float prob_Edge = Curr_NEdges / max_Edges; 
    printf("%.2f\n", prob_Edge);

    for(int i=0; i<NNodes; i++)
    {
        for(int j=0; j<i; j++)
        {
            int rnd = rand() %(100);
            if(rnd < (prob_Edge*100) && !(Curr_NEdges > NEdges ))
            {
               G->Adj_Matrix[i][j] = 1; 
            }
            else
            {
                G->Adj_Matrix[i][j] = 0;
            }
            G->Adj_Matrix[j][i] = G->Adj_Matrix[i][j];
        }
    }

    for(int i=0; i<NNodes; i++)
    {  
        Node *new_node = (Node *)malloc(sizeof(Node *));
        if (new_node ==NULL)
        {
            printf("Allocation for new_node failed\n");
            return;
        }
        new_node -> opinion = rand() %NOpinions;
        new_node -> next = NULL;
        G->NList[i] = *new_node;
    }
}

int main()
{
    create_graph();
    return 0;
}
