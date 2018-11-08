#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <fstream>
#include "ArgumentManager.h"

using namespace std;
class graphType
{
protected:
    //used to hold the number of vertices
    int maxVertices;
    //used to hold the current number of vertices
    int nVertices;
    list<int> *graph;
public:
    graphType() {
        maxVertices = 0;
        nVertices = 0;
    }
    graphType(int size)
    {
        maxVertices = size;
        nVertices = size;
        graph = new list<int>[size];
    }
    ~graphType()
    {
        delete[] graph;
    }
    
    int getnVertices() const
    {
        return nVertices;
    }
    bool isEmpty() const
    {
        return nVertices == 0;
    }
    void clearGraph()
    {
        for (int index = 0; index < nVertices; index++)
            graph[index].clear();
        nVertices = 0;
    }
    void make_Graph(ifstream &input, const int& size)
    {
        int vX;
        int adj_Vertex;
        string read_Line;
        if (nVertices != 0)
            clearGraph();
        nVertices = size;
        if (input.fail())
        {
            cout << "Cannot open input file." << endl;
            return;
        }
        vX = 0;
        while (getline(input, read_Line) && !read_Line.empty())
        {
            stringstream ss(read_Line);
            while (ss >> adj_Vertex) {
                graph[vX].push_back(adj_Vertex);
            }
            vX++;
        }
    }
    
    //function pulled from the book to traverse list
    void Traversal(int var, bool found[])
    {
        found[var] = true;
        
        for (auto& i : graph[var])
        {
            int testing = i;//returns the label of the vertex
            if (!found[testing])
                Traversal(testing, found);
        }
    }
    
    //function pulled from the book to traverse the list depth first
    void DTransversal(int& size)
    {
        bool *found;//points to a created array that will keep track of vertex that has been visited
        bool *last_position;
        found = new bool[nVertices];
        last_position = new bool[nVertices];
        for (int i = 0; i < nVertices; i++)
        {
            found[i] = false;
            last_position[i] = false;
        }
        //Traverse applied to vetices not found
        for (int dT = 0; dT < nVertices; dT++)
        {
            if (!found[dT])
            {
                Traversal(dT, found);
                size++;
                
            }
        }
        delete[]found;
        delete[]last_position;
        
    }
};

//class pulled from the book that implements Prim's algorithm
class MinimumSpannigTree : public graphType
{
private:
    double **nweights;
    int *numEdges;
    double *edgeWeight;
public:
    MinimumSpannigTree(int size) : graphType(size)
    {
        nweights = new double*[size];
        for (int i = 0; i < size; i++)
        {
            nweights[i] = new double[size];
        }
        numEdges = new int[size];
        edgeWeight = new double[size];
    }
    MinimumSpannigTree() {
        nweights = 0;
        numEdges = 0;
        edgeWeight = 0;
    }
    //function to set the weights of the numEdges of the tree
    void set_E_Weights(ifstream& input)
    {
        int i = 0;
        string line_read;
        while (getline(input, line_read) && !line_read.empty())
        {
            int k = 0;
            int edge_weight;
            stringstream parse(line_read);
            while (parse >> edge_weight)
            {
                nweights[i][k] = edge_weight;
                k++;
            }
            i++;
        }
    }
    
    void DepthT(int num_cluster, ofstream& output)//transverses to see which vertex with weights connects
    {
        int size = 0;
        //loop checking to see the amount of clusters isnt equal clusters wanted
        while (size != num_cluster) {
            size = 0;
            DTransversal(size);//
            if (size != num_cluster) {
                //searching for the highest weight and stores it to max
                double max_weight = 0;
                for (int i = 0; i < nVertices; i++)
                {
                    for (int j = 0; j < nVertices; j++)
                    {
                        if (max_weight < nweights[i][j])
                        {
                            max_weight = nweights[i][j];
                        }
                    }
                }
                //for loops determining conditions is the weight is -999
                for (int i = 0; i < nVertices; i++)
                {
                    for (int j = 0; j < nVertices; j++)
                    {
                        if (max_weight == nweights[i][j])
                        {
                            nweights[i][j] = -999;
                            graph[i].remove(j);
                            graph[j].remove(i);
                        }
                    }
                }
            }
            else {
                size = 0;
                bool *found = new bool[nVertices], *last = new bool[nVertices];
                for (int i = 0; i < nVertices; i++) {
                    found[i] = false;last[i] = false;
                }
                //depth traversal for the vertices not found
                for (int depth = 0; depth < nVertices; depth++)
                {
                    if (!found[depth])
                    {
                        Traversal(depth, found);
                        //itterate size to keep track of clusters
                        size++;
                        for (int i = 0; i < nVertices; i++) {
                            if (found[i] && !last[i]) {
                                output << i << " ";
                            }
                        }
                        output << endl;
                        for (int i = 0; i < nVertices; i++) {
                            if (!last[i] && found[i]) last[i] = found[i];
                        }
                    }
                }
                delete[] found;
                delete[] last;
            }
        }
        
    }
    ~MinimumSpannigTree()
    {
        for (int i = 0; i < nVertices; i++)
            delete[] nweights[i];
        delete[] nweights;
        delete[] numEdges;
        delete edgeWeight;
    }
};

int main(int argc, char *argv[])
{
    
    ArgumentManager am(argc, argv);
    string inputA = am.get("A");
    string outputC = am.get("C");
    
    
    ifstream input(inputA);
    ofstream output(outputC);
    
    
    int total_Size = 0;
    string line_read;
    while (getline(input, line_read) && !line_read.empty()) {
        total_Size++;
    }
    
    int matrix_size = 0;
    while (getline(input, line_read) && !line_read.empty())
        matrix_size++;
    if (total_Size != matrix_size)
        output.close();
    else
    {
        MinimumSpannigTree graph(total_Size);
        input.seekg(0);
        graph.make_Graph(input, total_Size);
        graph.set_E_Weights(input);
        int clusters;
        input >> clusters;
        if (clusters<1 || clusters > total_Size)
            output.close();
        else {
            graph.DepthT(clusters, output);
            input.close();
            output.close();
        }
    }
    return 0;
}
