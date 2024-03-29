#ifndef __GRID__
#define __GRID__

#include "Database.hpp"
#include "AvlTree.hpp"
#include "Cell.hpp"
#include "Graph.hpp"
#include <queue>
#define QUERY_RESULT_OUTPUT_FILE "query_result.txt"
#define POINT_SEPARATOR " | "

class Grid{
    
    private:

        AVLTree<Cell> cells;
        AVLTree<Cell*> active_cells_tree;
        float eps, R;
        void update_mc(AVLNode<Cell>* cell_node, const Graph& g, int color, auto&& lambda);
        void update_mc_recursive(std::queue<int> Q, const Graph& g, std::vector<bool>* visited, std::vector<std::list<Cell*>>* cells_by_vertex, std::vector<Cell*>& tree_vec, std::vector<Cell*>* solution, std::vector<std::vector<Cell*>>* all_solutions);

    public:
        Grid();
        Grid(const Database& db, const Graph& g, const float eps, const float R);
        void answer_query(const Graph& g);
        void add_point(int color, const Point& p, const Graph& g);
        void delete_point(Graph& g, int color, const Point& p);
};

#endif