#ifndef __GRID__
#define __GRID__

#include "Database.h"
#include "AvlTree.h"

class Grid{
    
    private:

        Node<Cell>* cells;
        Node<Cell*>* active_cells_tree;
        float eps, R;
        void update_mc(Node<Cell>* cell_node, int color, bool addition);

    public:
        Grid(const Database& db, const float eps, const float R);
        void answer_query();
        void add_point(int color, const Point& p);
        void delete_point(int color, const Point& p);
};

#endif