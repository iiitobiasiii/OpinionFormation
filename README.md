# OpinionFormation
### Project for earth dynamics lecture 2017/2018 @ Potsdam Institute for Climate Impact Research

Opinion Formation on Adaptive Voter Model (AVM)
based on ”Nonequilibrium phase transition in the
coevolution of networks and opinions" by Holme, Newman (2006)

We reproduce some results of the above paper and then modify by introducing zealots as done in "Zealotry Effects on Opinion Dynamics in the Adaptive Voter Model" by Klamser, Wiedemann (2017).

## C_Execute.ipynb
Executes C Program with parameters that are in the arrays of this notebook.
2nd cell does the same using multiprocessing

## C Code:
Takes as arguments: NNodes, NEdges, NOpinions, phi100, MAX_ITER, ITER_Step, file name (max 20chars)
exports data into file in ./data/filename.txt
data has format
1 line parameters
(2 line: adjacency matrix "0 1 ;1 0" -> " "after every entry, ";" after each row)
3 line: opinion vector (array of length NNodes, each entry represents the opinion attribute of the node in consensus state)
4 line: connected component vector (array of length NNodes, each entry corresponds to the connected component a node belongs to)



## Further Ideas
- Upper limit for opinion changes for every node
- Multiple Categories of Opinions, set of parameters with different weights
- parameter that controls tendency to change opinion
- adapt your opinion to the majority of your friends' opinions and not to a single one!
