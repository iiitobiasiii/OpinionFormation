/* The following structure is used:
*   There is a Graph struct which consists of a List of Node Pointers and an int Adjacency Matrix
*   Nodes are structs that contain one opinion
*   The adjacency matrix contains all information about connectivity
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


//Input parameter
/*#define NNodes 100
#define NEdges 30
#define NOpinions 25
#define phi100 12
#define MAX_ITER 100000000000*/
int NNodes;
int NEdges;
int NOpinions;
int phi100;
int MAX_ITER;

typedef struct Graph{
    struct Node** NList;
    int ** Adj_Matrix;
} Graph;


typedef struct Node{
    int opinion;
} Node;

typedef struct int_array{
    int len;
    int * data;
} int_array;


void iter_neighbors(Graph * G, int_array * Checklist, int NIndex, int * consensus, int op, int * preventSO, int * comp);

//function that creates a random graph and returns a pointer to it
Graph * create_graph()
{
    Graph *G = (Graph *) malloc(sizeof(Graph));
    if (G == NULL)
    {   
        printf("malloc of Graph failed \n");
        return NULL;
    }
    G->NList = malloc(NNodes * sizeof(Node *));
    if (G->NList == NULL)
    {
        printf("malloc failed\n");
        free(G);
        exit(0);
    }
    G->Adj_Matrix = (int **) malloc(NNodes * sizeof(int *));
    if (G->Adj_Matrix == NULL)
    {
        printf("malloc failed\n");
        exit(0);
    }
    for (int i = 0; i < NNodes; ++i)
    {
        G->Adj_Matrix[i] = (int *)malloc(NNodes * sizeof(int));
        if (G->Adj_Matrix[i] == NULL)
        {
            printf("malloc failed\n");
            exit(0);
        }
    }    

   /* For creating Random edges we use the following procedure: 
    * We have in general max_Edges = 1/2 * NNodes * (NNodes - 1) possible edges while only NEdges allowed
    * We iterize over all node pairs and connect them with probability prob_Edge
    */

    int Curr_NEdges = 0;
    int max_Edges = (NNodes * (NNodes -1))/2;
    int k=0;
    for(int i=0; i<NNodes; i++)
    {
        for(int j=0; j<i; j++)
        {
            //prob is Number of remaining edges to initialize / still possible edges
            k+=1;
            int remaining_Edges = max_Edges - k;
            float prob_Edge = (float) (NEdges - Curr_NEdges) / (float) (remaining_Edges);
            int rnd = rand() %(100);

            //if rnd number < prob_Edge then create edge
            if(rnd < (prob_Edge*100))
            {
               G->Adj_Matrix[i][j] = 1; 
               Curr_NEdges +=1 ;
            }

            //else no connection
            else
            {
                G->Adj_Matrix[i][j] = 0;
            }

            //symmetrize adjacency matrix
            G->Adj_Matrix[j][i] = G->Adj_Matrix[i][j];
            
        }
        //No Self-loops
        G->Adj_Matrix[i][i] = 0;
    }


    for(int i=0; i<NNodes; i++)
    {  
        Node *new_node = (Node *)malloc(sizeof(Node));

        //if not successfull free all nodes and the graph:
        if (new_node ==NULL)
        {
            //free(new_node);
            for (int g = 0; g < i; ++g)
            {
                free(G->NList[g]);
                G->NList[g] = NULL;
            }
            free(G);
            G = NULL;
            printf("Allocation for new_node failed\n");
            return NULL;
        }

        //if success: get random opinion and append to nodelist
        new_node -> opinion = rand() %NOpinions;
        G->NList[i] = new_node;
    }
    return G;
}

//Checks whether initialization failed

/*void check_graph(Graph * G)
{
    int ones = 0;
    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            if (G->Adj_Matrix[i][j] ==1)
            {
                ones +=1;
            }
            else if (G->Adj_Matrix[i][j]!=0)            {
                printf("initialization error in adj matrix\n");
            }
            {

            }
        }
    }
    printf("Number of 1 entries in Adj_Matrix: %d \n", ones);
    printf("Should be %d \n", 2*NEdges);
}*/


/*needs Graph and Index of a node
 * ouput: List of Indices that correspond to the neighbor nodes of the input node
 * NULL pointer if any kind of memory problem OR No Neighbors!
 */

int_array * getNeighbors(Graph * G, int NodeIndex)
{
    int_array * Neighborlist = (int_array *) malloc(sizeof(int_array));

    if (Neighborlist == NULL)
    {
        printf("Allocation of Nehborlist failed \n");
        return NULL;
    }

    Neighborlist->data = (int *) malloc(NNodes * sizeof(int));
    if (Neighborlist->data == NULL)
    {
        printf("Allocation of Neighborlist failed \n");
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }

    Neighborlist->len = 0;

    //Add Neighbors to Neighborlist
    for (int i=0; i<NNodes; i++)
    {
        if (G->Adj_Matrix[NodeIndex][i] == 1)
        {
            Neighborlist->data[Neighborlist->len]=i;
            Neighborlist->len +=1; 
        }
    }

    //If no neighbors, free space and return NULL
    if (Neighborlist->len == 0)
    {
        free(Neighborlist->data);
        Neighborlist->data = NULL;
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }

    //If some neighbors, reallocate for less than NNodes neighbors
    int * temp = (int *) realloc(Neighborlist->data, Neighborlist->len * sizeof(int));
    if (temp == NULL) {
        printf("length %d\n", Neighborlist->len );
        printf("Reallocation of Neighborlist failed \n");
        free(Neighborlist->data);
        Neighborlist->data = NULL;
        free(Neighborlist);
        Neighborlist = NULL;
        return NULL;
    }
    else
    {
        Neighborlist->data = temp;
    }
    return Neighborlist;
}


/*int_array * getNeighbors2(Graph * G, int NodeIndex)
{
    int temp_Neighbors[NNodes] = {0};
    int length = 0;

    for (int i=0; i<NNodes; i++)
    {
        if (G->Adj_Matrix[NodeIndex][i] == 1)
        {
            temp_Neighbors[length] = i;
            length +=1; 
        }
    }
    
    if (length == 0)
    {
        return NULL;
    }
    int_array * Neighborlist = (int_array *) malloc(sizeof(int_array));

    if (Neighborlist == NULL)
    {
        printf("Allocation of Nehborlist failed \n");
        exit(0);
        return NULL;
    }

    Neighborlist->data = (int *) malloc(length * sizeof(int));
    if (Neighborlist->data == NULL)
    {
        printf("Allocation of Neighborlist failed \n");
        free(Neighborlist);
        Neighborlist = NULL;
        exit(0);
        return NULL;
    }

    for (int i = 0; i < length; ++i)
    {
        Neighborlist->data[i] = temp_Neighbors[i];
    }

    Neighborlist->len = length;

    return Neighborlist;
}*/


//Returns int_array with all Nodes that are not yet connected but share the opinion
int_array * getSameOpinion(Graph * G, int NIndex)
{

    /*Create int_array (Node - Index - List) that contains all the other Nodes that
    have my opinion but are not yet connected to me */
    int_array * bubblestranger = malloc(sizeof(int_array));
    if (bubblestranger == NULL)
    {
        printf("Allocation of bubblestranger failed\n");
        return NULL;
    }

    bubblestranger->data = malloc(NNodes*sizeof(int));
    if (bubblestranger->data == NULL)
    {
        printf("Allocation of bubblestranger data failed \n");
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }
    bubblestranger->len = 0;
    
    //Get My opinion
    int curr_opinion = (G->NList[NIndex])->opinion;

    //Get My neighbors
    int_array * MyFriends = getNeighbors(G, NIndex);


    //Iterate all Nodes
    
    for (int i=0; i<NNodes; i++)
    {
        //If Opinion of ith Node is my opinion and its not me
        if( (G->NList[i])->opinion == curr_opinion && i != NIndex)
        {

            //Iterate through my friends and check whether i is already my friend
            
            int IsAlreadyMyFriend = 0;

            for (int j=0; j<MyFriends->len; j++)
            {
                if(MyFriends->data[j] == i)
                {
                    IsAlreadyMyFriend = 1;
                    break;
                }
            
            }
            //when not yet myfriend
            if (IsAlreadyMyFriend == 0)
            {
                //i is appended to bubblestranger list
                bubblestranger->data[bubblestranger->len] = i;
                //and list is now 1 element longer
                bubblestranger->len+=1;
            }
        }
    }

    //We no longer need my friends
    free(MyFriends->data);
    MyFriends->data = NULL;
    free(MyFriends);
    MyFriends = NULL;

    //return NULL pointer if no stranger shares my opinion 
    if (bubblestranger->len == 0)
    {
        free(bubblestranger->data);
        bubblestranger->data = NULL;
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }

    //Reallocate memory for bubblestranger
    bubblestranger->data = realloc(bubblestranger->data, bubblestranger->len * sizeof(int));
    if (bubblestranger->data == NULL)
    {
        printf("Reallocation of bubblestranger data failed\n");
        printf("No space for %d integers \n", bubblestranger->len );
        free(bubblestranger);
        bubblestranger = NULL;
        return NULL;
    }
    return bubblestranger;
}


void process1(Graph * G)
{

    //Pick random Node from Nodelist
    
    int NodeIndex = rand() %NNodes ;
    
    //Get Neighbors
    int_array * Neighbors = getNeighbors(G, NodeIndex);

    //return if no neighbors
    if (Neighbors == NULL){
        return;
    }
    

    //PickRandom Neighbor
    int NeighborIndex = rand() %Neighbors->len;
    int old_neighbor = Neighbors->data[NeighborIndex];
    
    int_array * bubblepeople = getSameOpinion(G, NodeIndex);

    //Exclude lonely nodes:
    if (bubblepeople == NULL)
    {
        free(Neighbors->data);
        Neighbors->data = NULL;
        free(Neighbors);
        Neighbors = NULL;
        return;
    }
    if (bubblepeople->data == NULL)
    {
        free(Neighbors);
        Neighbors = NULL;
        free(bubblepeople);
        bubblepeople = NULL;
        return;
    }

    if (bubblepeople->len == 0)
    {
        free(Neighbors->data);
        Neighbors->data = NULL;
        free(Neighbors);
        Neighbors = NULL;
        free(bubblepeople->data);
        bubblepeople->data = NULL;
        free(bubblepeople);
        bubblepeople = NULL;
        return;
    }
    
    //Take ith person that is not yet connected to you but has same opinion
    int new_friend_Bubbleindex = rand () %bubblepeople->len;

    int new_friend_index = -1;
    if (bubblepeople->data[new_friend_Bubbleindex] > -1)
        {
            new_friend_index = bubblepeople->data[new_friend_Bubbleindex];
        }
    //Get index in NList of this person
    
    //Delete old neighbor
    G->Adj_Matrix[NodeIndex][old_neighbor] = 0;
    G->Adj_Matrix[old_neighbor][NodeIndex] = 0;

    //Create new neighbor
    G->Adj_Matrix[NodeIndex][new_friend_index] = 1;
    G->Adj_Matrix[new_friend_index][NodeIndex] = 1;

    free(Neighbors->data);
    Neighbors->data = NULL;
    free(Neighbors);
    Neighbors = NULL;
    free(bubblepeople->data);
    bubblepeople->data = NULL;
    free(bubblepeople);
    bubblepeople = NULL;


    return;
}

void process2(Graph * G)
{
    //Pick random Node from Nodelist
    int NodeIndex = rand() %NNodes ;
    Node * curr_Node = G->NList[NodeIndex];

    //Get its opinion
    int curr_opinion = curr_Node->opinion;

    //Get its neighbors
    int_array * Neighbors = getNeighbors(G, NodeIndex);
    if (Neighbors == NULL)
    {
        return;
    }

    //Take random neighbor
    int ith_Neighbor = rand ()%Neighbors->len;
    int Neighbor = Neighbors->data[ith_Neighbor];

    //Change its opinion to my opinion
    (G->NList[Neighbor])->opinion = curr_opinion;

    free(Neighbors->data);
    Neighbors->data = NULL;
    free(Neighbors);
    Neighbors = NULL;
    return;
}


/*Think of a more efficient algorithm!!! */
int check_consensus(Graph * G)
{
    for (int i = 0; i < NNodes; i++)
    {
        int_array * curr_friends = getNeighbors(G, i);
        if (curr_friends == NULL)
        {
            continue;
        }

        
        int curr_opinion = (G->NList[i])->opinion;

        for (int j = 0; j < curr_friends->len; j++)
        {
            if ((G->NList[(curr_friends->data[j])])->opinion != curr_opinion)
                {
                    free(curr_friends->data);
                    curr_friends->data = NULL;
                    free(curr_friends);
                    curr_friends = NULL;
                    return 0;
                }    
        }

        free(curr_friends->data);
        curr_friends->data = NULL;
        free(curr_friends);
        curr_friends = NULL;
    }

    return 1;
}


int_array *check_consensus_fast(Graph * G)
{
    //printf("NEW CONSENSUS TEST\n");
    //Create array [0,0,..,0] Indicates for each node whether it is already in "its" consensus state
    int_array * Checklist = (int_array *) malloc(sizeof(int_array));
    if (Checklist==NULL)
    {
        printf("Malloc failed\n");
        exit(0);
        return NULL;
    }
    Checklist->data = calloc(NNodes, sizeof(int));
    if (Checklist->data == NULL)
    {
        printf("Malloc failed\n");
        free(Checklist);
        Checklist = NULL;
        exit(0);
        return NULL;
    }
    Checklist->len = NNodes;


    //Create "global variable" consensus which will be -1 during the process, 1 when finished in consensus state and 0 for abortion
     int * consensus = (int *) malloc(sizeof(int));
     if (consensus == NULL)
     {
         printf("Malloc failed\n");
         free(Checklist->data);
         Checklist->data = NULL;
         free(Checklist);
         Checklist = NULL;
         exit(0);
         return NULL;
     }
     * consensus = -1;
    
    //Prevent Stack Overflow, just a debug variable
     int * preventSO = (int *) malloc(sizeof(int));
     if (preventSO == NULL)
     {
         exit(0);
     }
     *preventSO = 0;

     //Prevent Stack Overflow, just a debug variable
     int * comp = (int *) malloc(sizeof(int));
     if (comp == NULL)
     {
         exit(0);
     }
     *comp = 0;

    //Create int that corresponds to the number of ordered nodes that are already checked
    int checked = 0;
    //iterate over components
    while(*consensus == -1)
        {
            *comp +=1;
            int j = 0;
            for (int i = 0; i < NNodes ; ++i)
            {
                if(Checklist->data[i] == 0)
                {
                    break;
                }
                j++;
            }
            checked = j;
            //printf("checked: %d \n", checked);

            // if array = [1,1,1..1] we are in consensus state !!
            if (checked == NNodes)
            {
                printf("CONSENSUS\n");
                *consensus = 1;
                break;
            }

            //get opinion of first unchecked node
            int op = G->NList[checked] -> opinion;


            //start recursion process to check connected component
            iter_neighbors(G, Checklist, checked, consensus, op, preventSO, comp);
        }

        if (*consensus != 1)
        {
            free(Checklist->data);
            Checklist->data = NULL;
            free(Checklist);
            Checklist = NULL; 
            free(consensus);
            consensus = NULL;
            return NULL;
        }

        else
        {
            free(consensus);
            consensus = NULL;
            return Checklist;
        }
    }




void iter_neighbors(Graph * G, int_array * Checklist, int NIndex, int * consensus, int op, int * preventSO, int * comp)
{
    //exit if process already finished 
    if (*consensus != -1)
    {
        return;
    }

    //Get Neighbors
    int_array * neighbors = getNeighbors(G, NIndex);

    if (neighbors == NULL)
    {
        Checklist->data[NIndex] = *comp;
        return;
    }

    //Check if NIndex - Node is in local consensus state
    for (int i = 0; i < neighbors->len; ++i)
    {
        int curr_neighbor = neighbors->data[i];
        //If a neighbor has a different opinion -> No consensus state!
        if (G->NList[curr_neighbor]->opinion != op) 
        {
            *consensus = 0;
            free(neighbors->data);
            neighbors->data = NULL;
            free(neighbors);
            neighbors = NULL;
            return;
        }
    }
    Checklist->data[NIndex] = *comp;
    //printf("Node on CList set to %d \n", Checklist->data[NIndex]);


    //Setze alle aktiven Nachbarn auf -1
    for (int i = 0; i < neighbors->len; ++i)
    {
        int curr_neighbor = neighbors->data[i];
        if(Checklist->data[curr_neighbor] != *comp)
        {
            Checklist->data[curr_neighbor] = -1;
        }
    }
    //free heap memory for neighbors
    free(neighbors->data);
    neighbors->data = NULL;
    free(neighbors);
    neighbors = NULL;

    

    for (int i = 0; i < NNodes; ++i)
    {
        if (Checklist->data[i] == -1 )
        {
            iter_neighbors(G, Checklist, i, consensus, op, preventSO, comp);
        }
        
    }

    
}


void export_data(Graph * G, int_array * Checklist, int iterations, char* fname)
{
    char str[25];
    strcpy(str, "./data/");
    strcat(str, fname);
    strcat(str, ".txt");
    printf("Data written to %s\n", str);
    FILE *fp = fopen(str,"a");
/*Vier Zeilen:
1. Zeile Input Parameter
2. Zeile Adjazenzmatrix
3. Meinungsvektor
4. Checklist
*/
    //Print parameters
    fprintf(fp, "Nodes: %d, Edges: %d, phi*100: %d, Iterations: %d \n", NNodes, NEdges, phi100, iterations);
    //print adj Matrix
    for (int i = 0; i < NNodes; ++i)
    {
        for (int j = 0; j < NNodes; ++j)
        {
            fprintf(fp, "%d ", G->Adj_Matrix[i][j]);
        }
        fprintf(fp, ";");
    }
    fprintf(fp, "\n");

    //print Opinions

    for (int i = 0; i < NNodes; ++i)
    {
        fprintf(fp, "%d ", G->NList[i]->opinion);
    }
    fprintf(fp, "\n");

    //print checklist

    for (int i = 0; i < Checklist->len; ++i)
    {
        fprintf(fp, "%d ", Checklist->data[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc != 8 || strlen(argv[7]) > 20)
    {
        printf("Call function with arguments: NNodes, NEdges, NOpinions, phi100, MAX_ITER, ITER_Step, file name shorter than 20char \n");
        exit(0);
    }
    NNodes = atoi(argv[1]);
    NEdges = atoi(argv[2]);
    NOpinions = atoi(argv[3]);
    phi100 = atoi(argv[4]);
    MAX_ITER = atoi(argv[5]);
    int ITER_Step = atoi(argv[6]);
    char* fname = argv[7];

    clock_t begin = clock();
    //For Random Numbers
    srand(time(NULL));

    Graph * G= create_graph();
    //check_graph(G);



    int iter;
    for (iter = 0; iter < MAX_ITER; iter++)
    {
        //printf("Consensus Check No %d\n", iter);
        int check = 0;
        if (iter%ITER_Step == 0)
        {
            check = check_consensus(G);
        }
        if (check == 1)
        {
            //printf("CONSENSUS STATE\n");
            
            int_array * Checklist = check_consensus_fast(G);
            if (Checklist != NULL)
            {
                printf("confirmed \n");
                export_data(G, Checklist, iter, fname);
                free(Checklist->data);
                free(Checklist);
            }
            else
            {
                printf("CONSENSUS FATAL ERROR\n");
                exit(0);
            }
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
    printf("Finished after %d iterations \n", iter);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("computation time: %e \n", time_spent);

    for (int i = 0; i < NNodes; ++i)
    {
        Node * curr_node = G->NList[i];
        free(curr_node);
        curr_node = NULL;
    }

    //free(G->NList);
    free(G);
    G = NULL;
    
    return 0;
}
