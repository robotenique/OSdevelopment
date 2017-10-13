import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
# REQUIREMENTS: Networkx(for python), graphviz(for linux)
#GENERATE the board with:
""" void debug_coisa(){
     for (size_t i = 0; i < speedway.lanes; i++) {
         printf("(");
         for (size_t j = 0; j < speedway.length; j++) {
             if(speedway.road[j][i] != -1)
                 printf("\"%u\",", speedway.road[j][i]);
             else
                printf("0,");
            }

             printf("),\n");

     }
 }"""
board = (("8",0,"4","0","12"),
(0,"9",0,"5","13"),
(0,"10",0,"6","1"),
("11",0,0,"7","3"))

sz = lambda l : range(len(l))

def exist(i, j):
    length = len(board[0])
    lanes = len(board)
    return i >= 0 and i < lanes and j >= 0 and j < length and board[i][j] != 0


nodes = filter(lambda x : x != 0, [board[i][j] for i in sz(board) for j in sz(board[i])])
nodes = list(map(str, nodes))
edges = []
for i in sz(board):
    for j in sz(board[i]):
        x = board[i][j]
        if x != 0:
            if exist(i - 1, (j + 1)%len(board[0])):
                edges.append((str(x), str(board[i - 1][(j + 1)%len(board[0])])))
            if exist(i, (j + 1)%len(board[0])):
                edges.append((str(x), str(board[i][(j + 1)%len(board[0])])))

            if exist(i + 1, (j + 1)%len(board[0])):
                edges.append((str(x), str(board[i + 1][(j + 1)%len(board[0])])))

            if exist(i + 1, j%len(board[0])):
                edges.append((str(x), str(board[i - 1][j])))


val_map = {'A': 1.0}
print(edges)

# Create Directed Graph
G=nx.DiGraph()

# Add a list of nodes:
G.add_nodes_from(nodes)
G.add_edges_from(edges)
# separate calls to draw nodes and edges
nx.nx_agraph.write_dot(G, "test1.dot")

#pos=nx.nx_agraph.graphviz_layout(G, prog='dot')
pos = nx.shell_layout(G)
plt.figure(figsize=(8, 8))
plt.axis('off')
nx.draw_networkx_nodes(G, pos, cmap=plt.get_cmap('jet'), node_size = 500)
nx.draw_networkx_labels(G, pos)
nx.draw_networkx_edges(G, pos, edgelist=G.edges(), edge_color='k', alpha=0.3)
plt.draw()
plt.show()
#Return a list of cycles described as a list o nodes
i = 0
for scc in nx.strongly_connected_components_recursive(G):
    for node in scc:
        val_map[node] = i
    i += 0.2
    print(scc)

nx.nx_agraph.write_dot(G, "test.dot")

values = [val_map.get(node, np.random.random()) for node in G.nodes()]
# separate calls to draw nodes and edges
#pos=nx.nx_agraph.graphviz_layout(G, prog='dot')
pos = nx.shell_layout(G)



plt.figure(figsize=(8, 8))
plt.axis('off')
nx.draw_networkx_nodes(G, pos, cmap=plt.get_cmap('jet'),
                       node_color = values, node_size = 600)
nx.draw_networkx_labels(G, pos)
nx.draw_networkx_edges(G, pos, edgelist=G.edges(), edge_color='k', alpha=0.3)
plt.draw()
plt.show()

exit()
