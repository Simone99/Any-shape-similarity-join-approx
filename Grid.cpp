#include "Grid.h"
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

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
  std::vector<Node<Cell*>*> tree_vec_active;
  std::vector<Node<Cell>*> tree_vec;
  ofstream output_file;
  tree_to_vec(this->active_cells_tree, tree_vec_active);
  tree_to_vec(this->cells, tree_vec);
  output_file.open("query_result.txt", ios::out | ios::trunc);
  for(Node<Cell*>* c : tree_vec_active){
    if(c->key->points_set.count(0) > 0){
      for(Node<Cell>* c1 : tree_vec){
        if(c1->key.points_set.count(1) > 0 && *c->key != c1->key && c->key->distance_from(c1->key) <= this->R){
          for(Node<Cell>* c2 : tree_vec){
            if(c2->key.points_set.count(2) > 0 &&
                *c->key != c2->key &&
                c1->key != c2->key &&
                c->key->distance_from(c2->key) <= this->R &&
                c1->key.distance_from(c2->key) <= this->R){
              // Loop throught all the points and return all the triples
              for(const Point& a : c->key->points_set[0]){
                for(const Point& b : c1->key.points_set[1]){
                  for(const Point& cc : c2->key.points_set[2]){
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

void Grid::update_mc(Node<Cell>* cell_node, int color, bool addition){
  std::vector<Node<Cell>*> tree_vec;
  tree_to_vec(this->cells, tree_vec);
  if(color == 0){
    for(Node<Cell>* c1 : tree_vec){
      if(c1->key.points_set.count(1) > 0 && cell_node->key != c1->key && cell_node->key.distance_from(c1->key) <= this->R){
        for(Node<Cell>* c2 : tree_vec){
          if(c2->key.points_set.count(2) > 0 &&
              cell_node->key != c2->key &&
              c1->key != c2->key &&
              cell_node->key.distance_from(c2->key) <= this->R &&
              c1->key.distance_from(c2->key) <= this->R){
            if(addition){
              cell_node->key.mc += c1->key.points_set[1].size() * c2->key.points_set[2].size();
              if(cell_node->key.mc > 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active == NULL){
                    cell_node->key.active = true;
                    cell_active = insertNode(this->active_cells_tree, &(cell_node->key));
                    if(this->active_cells_tree != cell_active)
                        this->active_cells_tree = cell_active;
                }
              }
            }else{
              cell_node->key.mc -= c1->key.points_set[1].size() * c2->key.points_set[2].size();
              if(cell_node->key.mc <= 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active != NULL){
                    cell_node->key.active = false;
                    this->active_cells_tree = deleteNode(this->active_cells_tree, &(cell_node->key));
                }
              }
            }
          }
        }
      }
    }
  }else if(color == 1){
    for(Node<Cell>* c1 : tree_vec){
      if(c1->key.points_set.count(0) > 0 && cell_node->key != c1->key && cell_node->key.distance_from(c1->key) <= this->R){
        for(Node<Cell>* c2 : tree_vec){
          if(c2->key.points_set.count(2) > 0 &&
              cell_node->key != c2->key &&
              c1->key != c2->key &&
              cell_node->key.distance_from(c2->key) <= this->R &&
              c1->key.distance_from(c2->key) <= this->R){
            if(addition){
              c1->key.mc += c1->key.points_set[0].size() * c2->key.points_set[2].size();
              if(cell_node->key.mc > 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active == NULL){
                    c1->key.active = true;
                    cell_active = insertNode(this->active_cells_tree, &(cell_node->key));
                    if(this->active_cells_tree != cell_active)
                        this->active_cells_tree = cell_active;
                }
              }
            }else{
              c1->key.mc -= c1->key.points_set[0].size() * c2->key.points_set[2].size();
              if(cell_node->key.mc <= 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active != NULL){
                    c1->key.active = false;
                    this->active_cells_tree = deleteNode(this->active_cells_tree, &(cell_node->key));
                }
              }
            }
          }
        }
      }
    }
  }else if(color == 2){
    for(Node<Cell>* c1 : tree_vec){
      if(c1->key.points_set.count(0) > 0 && cell_node->key != c1->key && cell_node->key.distance_from(c1->key) <= this->R){
        for(Node<Cell>* c2 : tree_vec){
          if(c2->key.points_set.count(1) > 0 &&
              cell_node->key != c2->key &&
              c1->key != c2->key &&
              cell_node->key.distance_from(c2->key) <= this->R &&
              c1->key.distance_from(c2->key) <= this->R){
            if(addition){
              c1->key.mc += c1->key.points_set[0].size() * c2->key.points_set[1].size();
              if(cell_node->key.mc > 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active == NULL){
                    c1->key.active = true;
                    cell_active = insertNode(this->active_cells_tree, &(cell_node->key));
                    if(this->active_cells_tree != cell_active)
                        this->active_cells_tree = cell_active;
                }
              }
            }else{
              c1->key.mc -= c1->key.points_set[0].size() * c2->key.points_set[1].size();
              if(cell_node->key.mc <= 0){
                // Check if the cell already exists in the active cells tree
                Node<Cell*>* cell_active = getNode(this->active_cells_tree, &(cell_node->key));
                if(cell_active != NULL){
                    c1->key.active = false;
                    this->active_cells_tree = deleteNode(this->active_cells_tree, &(cell_node->key));
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
  Node<Cell>* cell_node = getNode(this->cells, cell_tmp);
  if(cell_node == NULL){
      cell_tmp.points_set[color].emplace_back(p);
      cell_node = insertNode(this->cells, cell_tmp);
      if(this->cells != cell_node)
          this->cells = cell_node;
      cell_node = getNode(this->cells, cell_tmp);
  }else{
      cell_node->key.points_set[color].emplace_back(p);
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
  Node<Cell>* cell_node = getNode(this->cells, cell_tmp);
  if(cell_node == NULL)
    return;
  // Check if the color exists
  if(cell_node->key.points_set.count(color) <= 0)
    return;
  // Look for the element to delete
  long unsigned int i = 0;
  for(i = 0; i < cell_node->key.points_set[color].size(); i++){
    if(cell_node->key.points_set[color][i] == p){
      break;
    }
  }
  // If the point doesn't exist return
  if(i == cell_node->key.points_set[color].size())
    return;
  // Remove the element
  cell_node->key.points_set[color].erase(cell_node->key.points_set[color].begin() + i);
  //Update mc
  this->update_mc(cell_node, color, false);
  // Check if the cell is now empty
  bool empty = true;
  for(const std::pair<int, std::vector<Point>> n : cell_node->key.points_set){
    if(n.second.size() > 0){
      empty = false;
      break;
    }
  }
  // If empty delete it first from the active cell tree and then from the non-empty cells tree
  if(empty){
    this->active_cells_tree = deleteNode(this->active_cells_tree, &cell_node->key);
    this->cells = deleteNode(this->cells, cell_node->key);
  }
};