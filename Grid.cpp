#include "Grid.hpp"
#include <functional>
#include <iostream>
#include <fstream>
#include <functional>

auto comparison_function_for_heap = [](HeapNode& a, HeapNode& b){
        return a.priority < b.priority;
    };

auto priority_function = [](float distance, float R, int e){return e * R / distance;};

// https://www.geeksforgeeks.org/binary-search/
template<typename T>
int binarySearch(T arr[], int l, int r, T x)
{
    while (l <= r) {
        int m = l + (r - l) / 2;
 
        // Check if x is present at mid
        if (arr[m] == x)
            return m;
 
        // If x smaller, ignore left half
        if (arr[m] > x)
            l = m + 1;
 
        // If x is greater, ignore right half
        else
            r = m - 1;
    }
 
    // If we reach here, then element was not present
    return -1;
};

Grid::Grid(){
  this->eps = 0;
  this->R = 0;
  this->cells_heap.set_comparison_function(comparison_function_for_heap);
};

Grid::Grid(const Database& db, const Graph& g, const float eps, const float R){
    this->eps = eps;
    this->R = R;
    this->cells_heap.set_comparison_function(comparison_function_for_heap);
    // Create all the cell looping on all the points
    for(const std::pair<int, std::vector<Point>> n : db.getData() ){
        for(const Point& p : n.second){
          this->add_point(n.first, p, g);
        }
    }
};

void Grid::answer_query(const Graph& g, unsigned int n_to_report){

  auto report_all_points = [&](std::vector<Cell*>& solution, int pos, Point** final_combination, std::ofstream& output_f, auto&& report_all_points){
    if(n_to_report <= 0) return;
    if(pos >= g.V){
      int i;
      for(i = 0; i < g.V - 1; i++){
        output_f << *final_combination[i] << POINT_SEPARATOR;
      }
      output_f << *final_combination[i] << std::endl;
      n_to_report--;
      return;
    }
    for(Point& p : solution[pos]->points_set[pos]){
      final_combination[pos] = &p;
      report_all_points(solution, pos + 1, final_combination, output_f, report_all_points);
    }
  };

  std::ofstream output_file;
  output_file.open(QUERY_RESULT_OUTPUT_FILE, std::ios::out | std::ios::app);

  while(n_to_report > 0){

    // Report the highest shape
    HeapNode* node = this->cells_heap.first();
    if(node == nullptr || node->priority == 0) break;
    std::vector<Cell*> sol = node->cells;

    Point **tmp = new Point*[g.V];
    report_all_points(sol, 0, tmp, output_file, report_all_points);
    delete[] tmp;

    // Time to update heapnode priority
    node->priority = 0;
    this->cells_heap.update_heap();
  }
  output_file.close();

  // Time to reset all data structures to be ready to answer another query
  for(HeapNode& hn : *(this->cells_heap.get_container())){
    hn.priority = get_priority(hn.cells, g, priority_function);
  }

  this->cells_heap.update_heap();
};

void Grid::update_priority_recursive(std::queue<int> Q, const Graph& g, std::vector<bool>* visited, std::vector<std::list<Cell*>>* cells_by_vertex, std::vector<Cell*>& tree_vec, std::vector<Cell*>* solution, std::vector<std::vector<Cell*>>* all_solutions){
  if(Q.empty()){
    (*all_solutions).push_back(*solution);
    return;
  }
  int v_j = Q.front();
  Q.pop();
  for(Cell* cell_prime : (*cells_by_vertex)[v_j]){
    bool erase_neighbor_lists = false;
    std::vector<int> non_visited_neighbor;
    (*solution)[v_j] = cell_prime;
    for(int v_h : g.adj_list[v_j]){
      if((*visited)[v_h]){
        if((*solution)[v_h] != nullptr){
          if(cell_prime->distance_from(*(*solution)[v_h]) > this->R){
            (*solution)[v_j] = nullptr;
            erase_neighbor_lists = true;
            break;
          }
        }
      }else{
        non_visited_neighbor.push_back(v_h);
        for(Cell* cell_bar : tree_vec){
          if(cell_bar->points_set[v_h].size() > 0 && cell_prime->distance_from(*cell_bar) <= this->R){
            (*cells_by_vertex)[v_h].push_back(cell_bar);
          }
        }
        if((*cells_by_vertex)[v_h].empty()){
          (*solution)[v_j] = nullptr;
          erase_neighbor_lists = true;
          break;
        }
      }
    }
    if(erase_neighbor_lists){
      for(int v_h : non_visited_neighbor){
        (*cells_by_vertex)[v_h].clear();
      }
    }else{
      for(int v_h : non_visited_neighbor){
        (*visited)[v_h] = true;
        Q.push(v_h);
      }
      update_priority_recursive(Q, g, visited, cells_by_vertex, tree_vec, solution, all_solutions);
      for(int v_h : non_visited_neighbor){
        (*visited)[v_h] = false;
        Q.pop();
        (*cells_by_vertex)[v_h].clear();
      }
      (*solution)[v_j] = nullptr;
    }
  }
}

void Grid::update_priority(AVLNode<Cell>* cell_node, const Graph& g, int color, auto&& lambda){
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
  std::vector<Cell*> tree_vec = this->cells.to_vec();

  std::vector<Cell*> solution;
  for(int i = 0; i < g.V; i++){
    solution.push_back(nullptr);
  }
  std::vector<std::vector<Cell*>> all_solutions;

  update_priority_recursive(Q, g, &visited, &cells_by_vertex, tree_vec, &solution, &all_solutions);

  // If solutions exists, run the lambda function
  if(!all_solutions.empty())
    lambda(all_solutions);

};

float Grid::get_priority(std::vector<Cell*>& sol, const Graph& g, auto&& priority_function){
  float result = 0;
  for(std::pair<int, int> edge : g.edge_list){
    result += sol[edge.first]->distance_from(*sol[edge.second]);
  }
  return priority_function(result, this->R, g.E);
};

void Grid::add_point(int color, const Point& p, const Graph& g){
  std::vector<int> cell_coordinates;
  for(const float coordinate : p.coordinates){
      cell_coordinates.emplace_back(coordinate / this->eps);
  }
  Cell cell_tmp = {
    coordinates: cell_coordinates,
  };
  // Check if the cell already exists
  AVLNode<Cell>* cell_node = this->cells.get(cell_tmp);
  if(cell_node == nullptr){

      #if CELL_DISTANCE_METHOD == 1 || CELL_DISTANCE_METHOD == 2
      // If we have to check the cell vertices for the distance
      Point core_point;
      for(int coordinate_i : cell_tmp.coordinates){
          core_point.coordinates.push_back(coordinate_i * this->eps);
      }
      #endif
      #if CELL_DISTANCE_METHOD == 1
      
      auto find_vertices = [&](std::vector<Point>* cell_vertices, Point* core_point, int i, int k, auto&& find_vertices){
          if(i >= k){
              (*cell_vertices).push_back(*core_point);
              return;
          }
          (*core_point).coordinates[i] += this->eps;
          find_vertices(cell_vertices, core_point, i + 1, k, find_vertices);
          (*core_point).coordinates[i] -= this->eps;
          find_vertices(cell_vertices, core_point, i + 1, k, find_vertices);
      };
      find_vertices(&cell_tmp.cell_vertices, &core_point, 0, cell_tmp.coordinates.size(), find_vertices);
      #endif
      #if CELL_DISTANCE_METHOD == 2
      // If we have to check just the center
      float incr = this->eps / 2;
      for(float dim : core_point.coordinates){
        cell_tmp.cell_center.coordinates.push_back(dim + incr);
      }
      cell_tmp.eps = this->eps;
      #endif
      this->cells.insert(cell_tmp);
      cell_node = this->cells.get(cell_tmp);
  }
  // Normal insertion
  Cell* cell_to_edit = cell_node->get_key();
  cell_to_edit->points_set[color].push_back(p);

  this->update_priority(cell_node, g, color, [&](const std::vector<std::vector<Cell*>>& all_solutions){
    // Update m_c for all cells in sol[0] and their priority in the heap
    for(std::vector<Cell*> sol : all_solutions){
      float priority = this->get_priority(sol, g, priority_function);
      // Look for the solution inside the heap
      HeapNode* node = this->cells_heap.search_element(HeapNode{cells: sol});
      if(node != nullptr){
        node->priority = priority;
        this->cells_heap.update_heap();
      }else{
        this->cells_heap.push(HeapNode{cells: sol, priority: priority});
      }
    }
  });
};

void Grid::delete_point(const Graph& g, int color, const Point& p){
  std::vector<int> cell_coordinates;
  for(const float coordinate : p.coordinates){
      cell_coordinates.emplace_back(coordinate / this->eps);
  }
  Cell cell_tmp = {
      coordinates: cell_coordinates
  };
  // Check if the cell already exists
  AVLNode<Cell>* cell_node = this->cells.get(cell_tmp);
  if(cell_node == nullptr)
    return;
  // Check if the color exists
  Cell* cell_node_key = cell_node->get_key();
  if(!cell_node_key->points_set.contains(color))
    return;
  // Look for the element to delete
  long unsigned int i = 0;
  for(i = 0; i < cell_node_key->points_set[color].size(); i++){
    if(cell_node_key->points_set[color][i] == p){
      break;
    }
  }
  // If the point doesn't exist return
  if(i == cell_node_key->points_set[color].size())
    return;
  
  //Update priority
  this->update_priority(cell_node, g, color, [&](const std::vector<std::vector<Cell*>>& all_solutions){

    // Remove the element
    cell_node_key->points_set[color].erase(cell_node_key->points_set[color].begin() + i);

    // Update priority

    if(i == 0){
      // This is the only case i which the cell could be empty, because if we delete a point in another section of the vector
      // for sure we have other elements stored in the vector, so the cell is not empty.
      // Check if the cell is now empty
      bool empty = true;
      for(const std::pair<int, std::vector<Point>> n : cell_node_key->points_set){
        if(n.second.size() > 0){
          empty = false;
          break;
        }
      }

      // If it was the first element we need to actually change the priority in the corresponding heap node
      for(std::vector<Cell*> sol : all_solutions){
        // Look for the solution inside the heap
        HeapNode* node = this->cells_heap.search_element(HeapNode{cells: sol});
        if(node != nullptr){
          if(empty){
            node->priority = 0;
            this->cells_heap.update_heap();
          }else{
            // Check if the deleted point was the last stored in the corresponding relation
            if(cell_node_key->points_set[color].size() == 0){
              node->priority = 0;
              this->cells_heap.update_heap();
            }
          }
        }
      }

      // I don't actually delete the cell, because I keep pointers in the heap, so I store empty cells too
      // if(empty){
      //   this->cells.pop(*cell_node_key);
      // }
    }
  });
};