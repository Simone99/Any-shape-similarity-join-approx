#ifndef __GRID__
#define __GRID__

#include "Database.hpp"
#include "AvlTree.hpp"
#include "Cell.hpp"
#include "Graph.hpp"
#define QUERY_RESULT_OUTPUT_FILE "query_result.txt"

class Grid{
    
    private:

        AVLTree<Cell> cells;
        AVLTree<Cell*> active_cells_tree;
        float eps, R;
        void update_mc(AVLNode<Cell>* cell_node, const Graph& g, int color, auto&& lambda);

    public:
        Grid(const Database& db, const Graph& g, const float eps, const float R);
        void answer_query(const Graph& g);
        void add_point(int color, const Point& p, const Graph& g);
        void delete_point(Graph& g, int color, const Point& p);
};

#endif