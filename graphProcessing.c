/* The following structure is used:
*   There is a Graph struct which consists of a List of Node Pointers and an int Adjacency Matrix
*   Nodes are structs that contain one opinion (at the moment)
*   The adjacency matrix contains all information about connectivity
*   So far, it is always symmetrized. Later one could limit the calculations to one half to save memory/processing power
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NNodes 4
#define NEdges 2
#define NOpinions 2
#define phi100 12
#define MAX_ITER 10000

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

typedef struct int_array{
    int len;
    int * data;
} int_array;



Graph * create_graph()
{
    Graph *G = malloc(sizeof(Graph));
    if (G == NULL)
    {   
        printf("malloc of Graph failed \n");
        return NULL;
    }
    G->NList = malloc(NNodes * sizeof(Node*));
    if (G->NList == NULL)
    {
        printf("malloc of NList of Graph failen \n");
        free(G);
        return NULL;
    }
   /* For creating Random edges we use the following procedure: 
    * We have in general max_Edges = 1/2 * NNodes * (NNodes - 1) possible edges while only NEdges allowed
    * We iterize over all node pairs and connect them with probability prob_Edge
    * prob_Edge = (NEdges - Curr_NEdges)/(max_Edges - Curr_NEdges)
    */
    int Curr_NEdges = 0;
    int max_Edges = (NNodes * (NNodes -1))/2;
    int k=0;
    for(int i=0; i<NNodes; i++)
    {
        for(int j=0; j<i; j++)
        {
            float prob_Edge = 1- (float)(NEdges - Curr_NEdges) / (float)(max_Edges - Curr_NEdges); 

            int rnd = rand() %(100);
            //printf("rnd: %d , prob: %.2f \n", rnd, prob_Edge);
            if(rnd < (prob_Edge*100))
            {
               G->Adj_Matrix[i][j] = 1; 
               Curr_NEdges +=1 ;
            }
            else
            {
                G->Adj_Matrix[i][j] = 0;
            }
            G->Adj_Matrix[j][i] = G->Adj_Matrix[i][j];
            k+=1;
        }
    }
    printf("Number of Edges %d\n", Curr_NEdges);
    printf("Number of MAXEDGE %d\n", max_Edges);

    for(int i=0; i<NNodes; i++)
    {  
        Node *new_node = (Node *)malloc(sizeof(Node *));
        if (new_node ==NULL)
        {
            printf("Allocation for new_node failed\n");
            return NULL;
        }
        new_node -> opinion = rand() %NOpinions;
        new_node -> next = NULL;
        G->NList[i] = *new_node;
    }
    return G;
}


/*needs Graph and Index of a node
 * ouput: List of Indices that correspond to the neighbor nodes of the input node
 */

int_array * getNeighbors(Graph * G, int NodeIndex)
{
    int_array * Neighborlist = (int_array *) malloc(sizeof(int_array *));
    if (Neighborlist == NULL)
    {
        printf("Allocation of Nehborlist failed \n");
        return NULL;
    }
    Neighborlist->len = 0;
    Neighborlist->data = malloc(NNodes * sizeof(int));
    if (Neighborlist->data == NULL)
    {
        printf("Allocation of Neighborlist failed \n");
        free(Neighborlist);
        return NULL;
    }

    for (int i=0; i<NNodes; i++)
    {
        if (G->Adj_Matrix[NodeIndex][i] == 1)
        {
            Neighborlist->data[Neighborlist->len]=i;
            Neighborlist->len +=1; 
        }
    }
    Neighborlist->data = realloc(Neighborlist->data, Neighborlist->len);
    if (Neighborlist == NULL) {
        printf("Reallocation of Neighborlist failed \n");
        return NULL;
    }
    return Neighborlist;
}

//Returns int_array with all Nodes that are not yet connected but share the opinion
int_array * getSameOpinion(Graph * G, int NIndex)
{

    int_array * bubblepeople = malloc(sizeof(int_array));
    if (bubblepeople == NULL)
    {
        printf("Allocation of bubblepeople failed\n");
        return NULL;
    }
    bubblepeople->data = malloc(NNodes*sizeof(int));
    if (bubblepeople->data == NULL)
    {
        printf("Allocation of bubblepeople data failed \n");
        return NULL;
    }
    
    int curr_opinion = (G->NList[NIndex]).opinion;
    int_array * MyFriends = getNeighbors(G, NIndex);
    bubblepeople->len = 0;
    
    for (int i=0; i<NNodes; i++)
    {
        if( (G->NList[i]).opinion == curr_opinion && i != NIndex)
        {
            for (int j=0; j<MyFriends->len; j++)
            {
                if(MyFriends->data[j] != NIndex)
                {
                    bubblepeople->data[bubblepeople->len] += 1;
                    bubblepeople->len+=1;
                }
            
            }
        }
    }
    free(MyFriends);
    bubblepeople->data = realloc(bubblepeople->data, bubblepeople->len);
    if (bubblepeople->data == NULL)
    {
        printf("Reallocation of bubblepeople data failed\n");
        return NULL;
    }
    return bubblepeople;
}


void process1(Graph * G)
{

    //Pick random Node from Nodelist
    
    int NodeIndex = rand() %NNodes ;
    //Node * curr_Node = &(G->NList[NodeIndex]);

    //Get its opinion
    //int curr_opinion = curr_Node->opinion;
    
    //Get Neighbors
    //If no neighbors return
    int_array * Neighbors = getNeighbors(G, NodeIndex);
    if (Neighbors->len == 0){
        return;
    }
    
    //PickRandom Neighbor
    int NeighborIndex = rand() %Neighbors->len;
    int old_neighbor = Neighbors->data[NeighborIndex];
    
    int_array * bubblepeople = getSameOpinion(G, NodeIndex);
    
    //Take ith person that is not yet connected to you but has same opinion
    int new_friend_Bubbleindex = rand () %bubblepeople->len;

    //Get index in NList of this person
    int new_friend_index = bubblepeople->data[new_friend_Bubbleindex];

    //Delete old neighbor
    G->Adj_Matrix[NodeIndex][old_neighbor] = 0;
    G->Adj_Matrix[old_neighbor][NodeIndex] = 0;

    //Create new neighbor
    G->Adj_Matrix[NodeIndex][new_friend_index] = 1;
    G->Adj_Matrix[new_friend_index][NodeIndex] = 1;

    free(bubblepeople);
    free(Neighbors);
    //Is whole struct now freed??

    return;
}

void process2(Graph * G)
{
    //Pick random Node from Nodelist
    
    int NodeIndex = rand() %NNodes ;
    Node * curr_Node = &(G->NList[NodeIndex]);

    //Get its opinion
    int curr_opinion = curr_Node->opinion;

    int_array * Neighbors = getNeighbors(G, NodeIndex);
    if (Neighbors->len == 0){
        return;
    }
    int ith_Neighbor = rand ()%Neighbors->len;
    int old_friend_index = Neighbors->data[ith_Neighbor];
    (G->NList[old_friend_index]).opinion = curr_opinion;

    free(Neighbors);
    return;
}


/*Think of a more efficient algorithm!!! */
int check_consensus(Graph * G)
{
    for (int i = 0; i < NNodes; ++i)
    {
        int_array * curr_friends = getNeighbors(G, i);
        
        int curr_opinion = (G->NList[i]).opinion;

        for (int j = 0; j < curr_friends->len; ++j)
        {
            if ((G->NList[(curr_friends->data[j])]).opinion != curr_opinion)
                {
                    return 0;
                }    
        }
    }

    return 1;
}

int main()
{
    //For Random Numbers
    srand(time(NULL));

    Graph * G= create_graph();

    for (int i=0; i<NNodes; i++)
    {
        for (int j=0; j<NNodes; j++)
        {
        printf("%d", G->Adj_Matrix[i][j]);
        }
        printf("\n");
    }

    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        int check = check_consensus(G);
        if (check == 1)
        {
            break;
        }
        if ( rand() %100 < phi100 )
        {
            process1(G);
        }
        else
        {
            process2(G);
        }

    }
    printf("Finished. \n");
    return 0;
}
