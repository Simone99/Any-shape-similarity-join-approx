#include "Graph.hpp"

Graph::Graph(std::ifstream& input_file){
    int u, v, i;
    input_file >> this->V;
    input_file >> this->E;
    this->adj_list = new std::list<int> [this->V];
    for(i = 0; i < this->E; i++){
        input_file >> u >> v;
        this->adj_list[u].push_back(v);
        this->adj_list[v].push_back(u);
    }
};

Graph::~Graph(){
    delete[] this->adj_list;
};
