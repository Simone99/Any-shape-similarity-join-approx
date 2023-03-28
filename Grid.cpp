#include "Grid.hpp"
#include <functional>
#include <iostream>
#include <fstream>

Grid::Grid(const Database& db, const float eps, const float R){
    this->eps = eps;
    this->R = R;
    // Create all the cell looping on all the points
    const std::unordered_map<int, std::vector<Point>>& data = db.getData();
    for(const std::pair<int, std::vector<Point>> n : data ){
        for(const Point& p : n.second){
          this->add_point(n.first, p);
        }
    }
};

void Grid::answer_query(){
  std::vector<AVLNode<Cell*>*> tree_vec_active = this->active_cells_tree.to_vec();
  std::vector<AVLNode<Cell>*> tree_vec = this->cells.to_vec();
  std::ofstream output_file;
  output_file.open("query_result.txt", std::ios::out | std::ios::trunc);
  for(AVLNode<Cell*>* c : tree_vec_active){
    Cell* c_key = *(c->get_key());
    if(c_key->points_set.count(0) > 0){
      for(AVLNode<Cell>* c1 : tree_vec){
        Cell* c1_key = c1->get_key();
        if(c1_key->points_set.count(1) > 0 && *c_key != *c1_key && c_key->distance_from(*c1_key) <= this->R){
          for(AVLNode<Cell>* c2 : tree_vec){
            Cell* c2_key = c2->get_key();
            if(c2_key->points_set.count(2) > 0 &&
                *c_key != *c2_key &&
                *c1_key != *c2_key &&
                c_key->distance_from(*c2_key) <= this->R &&
                c1_key->distance_from(*c2_key) <= this->R){
              // Loop throught all the points and return all the triples
              for(const Point& a : c_key->points_set[0]){
                for(const Point& b : c1_key->points_set[1]){
                  for(const Point& cc : c2_key->points_set[2]){
                    output_file << a << " - " << b << " - " << cc << std::endl;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  output_file.close();
};

void Grid::update_mc(AVLNode<Cell>* cell_node, int color, bool addition){
  std::vector<AVLNode<Cell>*> tree_vec = this->cells.to_vec();
  Cell* cell_node_key = cell_node->get_key();
  if(color == 0){
    for(AVLNode<Cell>* c1 : tree_vec){
      Cell* c1_key = c1->get_key();
      if(c1_key->points_set.count(1) > 0 && *cell_node_key != *c1_key && cell_node_key->distance_from(*c1_key) <= this->R){
        for(AVLNode<Cell>* c2 : tree_vec){
          Cell* c2_key = c2->get_key();
          if(c2_key->points_set.count(2) > 0 &&
              *cell_node_key != *c2_key &&
              *c1_key != *c2_key &&
              cell_node_key->distance_from(*c2_key) <= this->R &&
              c1_key->distance_from(*c2_key) <= this->R){
            if(addition){
              cell_node_key->mc += c1_key->points_set[1].size() * c2_key->points_set[2].size();
              if(cell_node_key->mc > 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(cell_node_key);
                if(cell_active == nullptr){
                    cell_node_key->active = true;
                    this->active_cells_tree.insert(cell_node_key);
                }
              }
            }else{
              cell_node_key->mc -= c1_key->points_set[1].size() * c2_key->points_set[2].size();
              if(cell_node_key->mc <= 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(cell_node_key);
                if(cell_active != nullptr){
                    cell_node_key->active = false;
                    this->active_cells_tree.pop(cell_node_key);
                }
              }
            }
          }
        }
      }
    }
  }else if(color == 1){
    for(AVLNode<Cell>* c1 : tree_vec){
      Cell* c1_key = c1->get_key();
      if(c1_key->points_set.count(0) > 0 && *cell_node_key != *c1_key && cell_node_key->distance_from(*c1_key) <= this->R){
        for(AVLNode<Cell>* c2 : tree_vec){
          Cell* c2_key = c2->get_key();
          if(c2_key->points_set.count(2) > 0 &&
              *cell_node_key != *c2_key &&
              *c1_key != *c2_key &&
              cell_node_key->distance_from(*c2_key) <= this->R &&
              c1_key->distance_from(*c2_key) <= this->R){
            if(addition){
              c1_key->mc += c1_key->points_set[0].size() * c2_key->points_set[2].size();
              if(c1_key->mc > 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(c1_key);
                if(cell_active == nullptr){
                    c1_key->active = true;
                    this->active_cells_tree.insert(c1_key);
                }
              }
            }else{
              c1_key->mc -= c1_key->points_set[0].size() * c2_key->points_set[2].size();
              if(c1_key->mc <= 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(c1_key);
                if(cell_active != nullptr){
                    c1_key->active = false;
                    this->active_cells_tree.pop(c1_key);
                }
              }
            }
          }
        }
      }
    }
  }else if(color == 2){
    for(AVLNode<Cell>* c1 : tree_vec){
      Cell* c1_key = c1->get_key();
      if(c1_key->points_set.count(0) > 0 && *cell_node_key != *c1_key && cell_node_key->distance_from(*c1_key) <= this->R){
        for(AVLNode<Cell>* c2 : tree_vec){
          Cell* c2_key = c2->get_key();
          if(c2_key->points_set.count(1) > 0 &&
              *cell_node_key != *c2_key &&
              *c1_key != *c2_key &&
              cell_node_key->distance_from(*c2_key) <= this->R &&
              c1_key->distance_from(*c2_key) <= this->R){
            if(addition){
              c1_key->mc += c1_key->points_set[0].size() * c2_key->points_set[1].size();
              if(c1_key->mc > 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(c1_key);
                if(cell_active == nullptr){
                    c1_key->active = true;
                    this->active_cells_tree.insert(c1_key);
                }
              }
            }else{
              c1_key->mc -= c1_key->points_set[0].size() * c2_key->points_set[1].size();
              if(c1_key->mc <= 0){
                // Check if the cell already exists in the active cells tree
                AVLNode<Cell*>* cell_active = this->active_cells_tree.get(c1_key);
                if(cell_active != nullptr){
                    c1_key->active = false;
                    this->active_cells_tree.pop(c1_key);
                }
              }
            }
          }
        }
      }
    }
  }
}

void Grid::add_point(int color, const Point& p){
  std::vector<int> cell_coordinates;
  for(const float coordinate : p.coordinates){
      cell_coordinates.emplace_back(coordinate / this->eps);
  }
  Cell cell_tmp = {
      mc: 0,
      active: false,
      coordinates: cell_coordinates
  };
  // Check if the cell already exists
  AVLNode<Cell>* cell_node = this->cells.get(cell_tmp);
  if(cell_node == nullptr){
      cell_tmp.points_set[color].emplace_back(p);
      this->cells.insert(cell_tmp);
      cell_node = this->cells.get(cell_tmp);
  }else{
      cell_node->get_key()->points_set[color].emplace_back(p);
  }
  this->update_mc(cell_node, color, true);
};

void Grid::delete_point(int color, const Point& p){
  std::vector<int> cell_coordinates;
  for(const float coordinate : p.coordinates){
      cell_coordinates.emplace_back(coordinate / this->eps);
  }
  Cell cell_tmp = {
      mc: 0,
      active: false,
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
  //Update mc
  this->update_mc(cell_node, color, false);
  // Remove the element
  cell_node_key->points_set[color].erase(cell_node_key->points_set[color].begin() + i);
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
    // If the cell is empty and the point removed was of 0 color than it has already been removed from
    // active cells tree by update_mc function.
    //this->active_cells_tree = deleteNode(this->active_cells_tree, &cell_node->key);
    this->cells.pop(*cell_node_key);
  }
};