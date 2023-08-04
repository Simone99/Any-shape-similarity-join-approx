#include "Grid.hpp"
#include <functional>
#include <iostream>
#include <fstream>
#include <functional>

Grid::Grid(){
  this->eps = 0;
  this->R = 0;
};

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
  for(Cell** solution : this->active_cells_tree.to_vec()){
    this->update_mc(this->cells.get(**solution), g, 0, [&](const std::vector<std::vector<Cell*>>& all_solutions){

      auto report_all_points = [&](std::vector<Cell*>& solution, int pos, Point** final_combination, std::ofstream& output_f, auto&& report_all_points){
        if(pos >= g.V){
          int i;
          for(i = 0; i < g.V - 1; i++){
            output_f << *final_combination[i] << POINT_SEPARATOR;
          }
          output_f << *final_combination[i] << std::endl;
          return;
        }
        for(Point& p : solution[pos]->points_set[pos]){
          final_combination[pos] = &p;
          report_all_points(solution, pos + 1, final_combination, output_f, report_all_points);
        }
      };

      std::ofstream output_file;
      output_file.open(QUERY_RESULT_OUTPUT_FILE, std::ios::out | std::ios::app);
      for(std::vector<Cell*> sol : all_solutions){
        Point **tmp = new Point*[g.V];
        report_all_points(sol, 0, tmp, output_file, report_all_points);
        delete[] tmp;
      }
      output_file.close();
    });
  }
};

void Grid::update_mc_recursive(std::queue<int> Q, const Graph& g, std::vector<bool>* visited, std::vector<std::list<Cell*>>* cells_by_vertex, std::vector<Cell*>& tree_vec, std::vector<Cell*>* solution, std::vector<std::vector<Cell*>>* all_solutions){
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
          if(cell_bar->m[v_h] > 0 && cell_prime->distance_from(*cell_bar) <= this->R){
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
      update_mc_recursive(Q, g, visited, cells_by_vertex, tree_vec, solution, all_solutions);
      for(int v_h : non_visited_neighbor){
        (*visited)[v_h] = false;
        Q.pop();
        (*cells_by_vertex)[v_h].clear();
      }
      (*solution)[v_j] = nullptr;
    }
  }
}

void Grid::update_mc(AVLNode<Cell>* cell_node, const Graph& g, int color, auto&& lambda){
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

  update_mc_recursive(Q, g, &visited, &cells_by_vertex, tree_vec, &solution, &all_solutions);

  // If solutions exists, run the lambda function
  if(!all_solutions.empty())
    lambda(all_solutions);

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
      for(int i = 0; i < g.V; i++){
        cell_tmp.m.push_back(0);
      }
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
  Cell* cell_to_edit = cell_node->get_key();
  cell_to_edit->points_set[color].emplace_back(p);
  cell_to_edit->m[color] += 1;

  // Optimization
  if(color == 0 && cell_to_edit->m[color] != 1){
    cell_to_edit->m_c += cell_to_edit->m_c / (cell_to_edit->m[0] - 1);
    return;
  }

  this->update_mc(cell_node, g, color, [&](const std::vector<std::vector<Cell*>>& all_solutions){
    // Update m_c for all cells in sol[0]
    for(std::vector<Cell*> sol : all_solutions){
      int n_solutions = 1;
      for(int i = 1; i < g.V; i++){
        n_solutions *= sol[i]->m[i];
      }
      sol[0]->m_c += sol[0]->m[0] * n_solutions;
      this->active_cells_tree.insert(sol[0]);
    }
  });
};

void Grid::delete_point(Graph& g, int color, const Point& p){
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
  if(cell_node_key->points_set.count(color) <= 0)
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

  // Optimization
  if(color == 0 && cell_node_key->m[color] > 1){
    cell_node_key->m_c -= cell_node_key->m_c / cell_node_key->m[0];
    cell_node_key->m[0]--;
    cell_node_key->points_set[0].erase(cell_node_key->points_set[color].begin() + i);
    return;
  }
  
  //Update mc
  this->update_mc(cell_node, g, color, [&](const std::vector<std::vector<Cell*>>& all_solutions){

    // Remove the element
    cell_node_key->points_set[color].erase(cell_node_key->points_set[color].begin() + i);
    cell_node_key->m[color]--;

    // Update m_c for all cells in cells_by_vertex[0]
    for(std::vector<Cell*> sol : all_solutions){
      int n_solutions = 1;
      for(int i = 1; i < g.V; i++){
        n_solutions *= sol[i]->m[i];
      }
      sol[0]->m_c += sol[0]->m[0] * n_solutions;
      if(sol[0]->m_c == 0){
        this->active_cells_tree.pop(sol[0]);
      }
    }

    // Handle the case in which the cell is empty
    // Check if the cell is now empty
    bool empty = true;
    for(const std::pair<int, std::vector<Point>> n : cell_node_key->points_set){
      if(n.second.size() > 0){
        empty = false;
        break;
      }
    }
    // If empty delete it first from the active cell tree and then from the non-empty cells tree
    if(empty){
      this->cells.pop(*cell_node_key);
    }
  });
};