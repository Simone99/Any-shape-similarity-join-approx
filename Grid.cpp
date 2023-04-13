#include "Grid.hpp"
#include <functional>
#include <iostream>
#include <fstream>
#include <queue>
#include <functional>

Grid::Grid(const Database& db, const Graph& g, const float eps, const float R){
    this->eps = eps;
    this->R = R;
    // Create all the cell looping on all the points
    const std::unordered_map<int, std::vector<Point>>& data = db.getData();
    for(const std::pair<int, std::vector<Point>> n : data ){
        for(const Point& p : n.second){
          this->add_point(n.first, p, g);
        }
    }
};

void Grid::answer_query(const Graph& g){
  for(AVLNode<Cell*>* solution : this->active_cells_tree.to_vec()){
    this->update_mc(this->cells.get(**solution->get_key()), g, 0, false, true, [&](const std::vector<std::list<Cell*>>& cells_by_vertex){

      auto report_all_points = [&](Cell** solution, int pos, Point** final_combination, std::ofstream& output_f, auto&& report_all_points){
        if(pos >= g.V){
          int i;
          for(i = 0; i < g.V - 1; i++){
            output_f << *final_combination[i] << " - ";
          }
          output_f << *final_combination[i] << std::endl;
          return;
        }
        for(Point& p : solution[pos]->points_set[pos]){
          final_combination[pos] = &p;
          report_all_points(solution, pos + 1, final_combination, output_f, report_all_points);
        }
      };

      auto recursive_function = [&](const std::vector<std::list<Cell*>>& cells_by_vertex, int pos, Cell** solution, std::ofstream& output_f, auto&& recursive_function){
        if(pos >= g.V){
          Point **tmp = new Point*[g.V];
          report_all_points(solution, 0, tmp, output_f, report_all_points);
          delete[] tmp;
          return;
        }

        for(Cell* c : cells_by_vertex[pos]){
          solution[pos] = c;
          recursive_function(cells_by_vertex, pos + 1, solution, output_f, recursive_function);
        }
      };

      std::ofstream output_file;
      output_file.open(QUERY_RESULT_OUTPUT_FILE, std::ios::out | std::ios::app);
      Cell **tmp = new Cell*[g.V];
      recursive_function(cells_by_vertex, 0, tmp, output_file, recursive_function);
      delete[] tmp;
      output_file.close();
    });
  }
};

void Grid::update_mc(AVLNode<Cell>* cell_node, const Graph& g, int color, bool addition, bool answer, auto&& lambda){
  if(!answer){
    if(cell_node->get_key()->m[color] > 0){
      cell_node->get_key()->m[color] += 1;
      return;
    }
    //Insertion in the case m[color] is equal to 0
    cell_node->get_key()->m[color] += 1;
  }
  // Queue for BFS
  std::queue<int> Q;
  // Initialize bool vector for keeping track of visited nodes
  std::vector<bool> visited;
  for(int i = 0; i < g.V; i++){
    visited.push_back(false);
  }
  visited[color] = true;
  // Lists to keep track of the cells assigned to all vertices
  std::vector<std::list<Cell*>> cells_by_vertex;
  for(int i = 0; i < g.V; i++){
    cells_by_vertex.push_back(std::list<Cell*>());
  }
  cells_by_vertex[color].push_back(cell_node->get_key());
  // Insert vertex into queue
  Q.push(color);
  // Take all available cells
  std::vector<AVLNode<Cell>*> tree_vec = this->cells.to_vec();
  // Start BFS loop
  while (!Q.empty())
  {
    int v_j = Q.front();
    Q.pop();
    for(Cell* cell_prime : cells_by_vertex[v_j]){
      for(int v_h : g.adj_list[v_j]){
        if(visited[v_h]){
          auto cell_bar = cells_by_vertex[v_h].begin();
          while(cell_bar != cells_by_vertex[v_h].end()){
            if(cell_prime->distance_from(**cell_bar) > this->R){
              cell_bar = cells_by_vertex[v_h].erase(cell_bar);
            }else{
              cell_bar++;
            }
          }
          if(cells_by_vertex[v_h].empty())
            return;
        }else{
          // If the vertex has not been visited
          for(AVLNode<Cell>* c : tree_vec){
            Cell* cell_bar = c->get_key();
            if(*cell_bar != *cell_prime && cell_bar->m[v_h] > 0 && cell_prime->distance_from(*cell_bar) <= this->R){
              cells_by_vertex[v_h].push_back(cell_bar);
            }
          }
          visited[v_h] = true;
          Q.push(v_h);
        }
      }
    }
  }
  // If solutions exists, run the lambda function

  lambda(cells_by_vertex);

};

void Grid::add_point(int color, const Point& p, const Graph& g){
  std::vector<int> cell_coordinates;
  for(const float coordinate : p.coordinates){
      cell_coordinates.emplace_back(coordinate / this->eps);
  }
  Cell cell_tmp = {
    coordinates: cell_coordinates
  };
  // Check if the cell already exists
  AVLNode<Cell>* cell_node = this->cells.get(cell_tmp);
  if(cell_node == nullptr){
      for(int i = 0; i < g.V; i++){
        cell_tmp.m.push_back(0);
      }
      this->cells.insert(cell_tmp);
      cell_node = this->cells.get(cell_tmp);
  }
  cell_node->get_key()->points_set[color].emplace_back(p);
  this->update_mc(cell_node, g, color, true, false, [&](const std::vector<std::list<Cell*>>& cells_by_vertex){
    // Set B_c to true for cells stored in vertex 0 and store them in the active cells tree
    for(Cell* c1 : cells_by_vertex[0]){
      c1->B_c = true;
      this->active_cells_tree.insert(c1);
    }
  });
};

void Grid::delete_point(int color, const Point& p){
  // std::vector<int> cell_coordinates;
  // for(const float coordinate : p.coordinates){
  //     cell_coordinates.emplace_back(coordinate / this->eps);
  // }
  // Cell cell_tmp = {
  //     coordinates: cell_coordinates
  // };
  // // Check if the cell already exists
  // AVLNode<Cell>* cell_node = this->cells.get(cell_tmp);
  // if(cell_node == nullptr)
  //   return;
  // // Check if the color exists
  // Cell* cell_node_key = cell_node->get_key();
  // if(cell_node_key->points_set.count(color) <= 0)
  //   return;
  // // Look for the element to delete
  // long unsigned int i = 0;
  // for(i = 0; i < cell_node_key->points_set[color].size(); i++){
  //   if(cell_node_key->points_set[color][i] == p){
  //     break;
  //   }
  // }
  // // If the point doesn't exist return
  // if(i == cell_node_key->points_set[color].size())
  //   return;
  // //Update mc
  // this->update_mc(cell_node, color, false);
  // // Remove the element
  // cell_node_key->points_set[color].erase(cell_node_key->points_set[color].begin() + i);
  // // Check if the cell is now empty
  // bool empty = true;
  // for(const std::pair<int, std::vector<Point>> n : cell_node_key->points_set){
  //   if(n.second.size() > 0){
  //     empty = false;
  //     break;
  //   }
  // }
  // // If empty delete it first from the active cell tree and then from the non-empty cells tree
  // if(empty){
  //   // If the cell is empty and the point removed was of 0 color than it has already been removed from
  //   // active cells tree by update_mc function.
  //   //this->active_cells_tree = deleteNode(this->active_cells_tree, &cell_node->key);
  //   this->cells.pop(*cell_node_key);
  // }
};