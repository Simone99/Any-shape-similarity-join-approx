#ifndef __GRID__
#define __GRID__

#include "Database.hpp"
#include "AvlTree.hpp"
#define QUERY_RESULT_OUTPUT_FILE "query_result.txt"

class Grid{
    
    private:

        AVLTree<Cell> cells;
        AVLTree<Cell*> active_cells_tree;
        float eps, R;
        void update_mc(AVLNode<Cell>* cell_node, int color, bool addition);

    public:
        Grid(const Database& db, const float eps, const float R);
        void answer_query();
        void add_point(int color, const Point& p);
        void delete_point(int color, const Point& p);
};

#endif