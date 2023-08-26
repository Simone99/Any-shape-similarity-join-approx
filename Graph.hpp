#ifndef __GRAPH__
#define __GRAPH__

#include <list>
#include <fstream>
#include "Cell.hpp"

class Graph{

    public:

        int V, E;
        std::list<int> *adj_list;
        Graph(std::ifstream& input_file);
        ~Graph();
        
};

#endif