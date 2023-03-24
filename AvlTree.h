#ifndef __AVL__
#define __AVL__

#include <unordered_map>
#include <vector>
#include <ostream>
#include <iostream>
#include "Database.h"

struct Cell{
    std::unordered_map<int, std::vector<Point>> points_set;
    int mc;
    bool active;
    std::vector<int> coordinates;

    float distance_from(const Cell& other){
        float min_distance = INT32_MAX;
        for(const std::pair<int, std::vector<Point>> set_a : this->points_set){
            for(const std::pair<int, std::vector<Point>> set_b : other.points_set){
                for(Point a : set_a.second){
                    for(const Point& b : set_b.second){
                        float distance = a.euclidean_distance_from(b);
                        if(distance < min_distance)
                            min_distance = distance;
                    }
                }
            }
        }
        return min_distance;
    }

    friend bool operator<(const Cell& l, const Cell& r)
    {
        return std::tie(l.coordinates) < std::tie(r.coordinates); // keep the same order
    }
    friend bool operator>(const Cell& l, const Cell& r)
    {
        return r < l; // keep the same order
    }
    friend bool operator<=(const Cell& l, const Cell& r)
    {
        return !(l > r); // keep the same order
    }
    friend bool operator>=(const Cell& l, const Cell& r)
    {
        return !(l < r); // keep the same order
    }
    friend bool operator==(const Cell& l, const Cell& r)
    {
        return std::tie(l.coordinates) == std::tie(r.coordinates); // keep the same order
    }
    friend bool operator!=(const Cell& l, const Cell& r)
    {
        return !(l == r); // keep the same order
    }
    friend std::ostream& operator<<(std::ostream& os, const Cell& obj){
        os << "Cell:" <<std::endl;
        for(const std::pair<int, std::vector<Point>> n : obj.points_set ){
            os << "Color " << n.first << std::endl;
            for(const Point& p : n.second){
                os << p << std::endl;
            }
        }
        os << "Active: " << obj.active << std::endl;
        os << "mc: " << obj.mc << std::endl;
        os << "Coordinates: ";
        for(const int coordinate : obj.coordinates){
            os << coordinate << " ";
        }
        os << std::endl;
        return os;
    }
};

template <typename T>
class Node {

    public:
        T key;
        Node *left;
        Node *right;
        int height;

};

// Calculate height
template <typename T>
int height(Node<T> *N) {
  if (N == NULL)
    return 0;
  return N->height;
}

// New node creation
template <typename T>
Node<T> *newNode(T key) {
  Node<T> *node = new Node<T>();
  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;
  return (node);
}

// Rotate right
template <typename T>
Node<T> *rightRotate(Node<T> *y) {
  Node<T> *x = y->left;
  Node<T> *T2 = x->right;
  x->right = y;
  y->left = T2;
  y->height = std::max(height(y->left),
          height(y->right)) +
        1;
  x->height = std::max(height(x->left),
          height(x->right)) +
        1;
  return x;
}

// Rotate left
template <typename T>
Node<T> *leftRotate(Node<T> *x) {
  Node<T> *y = x->right;
  Node<T> *T2 = y->left;
  y->left = x;
  x->right = T2;
  x->height = std::max(height(x->left),
          height(x->right)) +
        1;
  y->height = std::max(height(y->left),
          height(y->right)) +
        1;
  return y;
}

// Get the balance factor of each node
template <typename T>
int getBalanceFactor(Node<T> *N) {
  if (N == NULL)
    return 0;
  return height(N->left) -
       height(N->right);
}

// Insert a node
template <typename T>
Node<T> *insertNode(Node<T> *node, T key) {
  // Find the correct postion and insert the node
  if (node == NULL)
    return (newNode(key));
  if (key < node->key)
    node->left = insertNode(node->left, key);
  else if (key > node->key)
    node->right = insertNode(node->right, key);
  else
    return node;

  // Update the balance factor of each node and
  // balance the tree
  node->height = 1 + std::max(height(node->left),
               height(node->right));
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1) {
    if (key < node->left->key) {
      return rightRotate(node);
    } else if (key > node->left->key) {
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }
  }
  if (balanceFactor < -1) {
    if (key > node->right->key) {
      return leftRotate(node);
    } else if (key < node->right->key) {
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }
  }
  return node;
}

// Node with minimum value
template <typename T>
Node<T> *nodeWithMimumValue(Node<T> *node) {
  Node<T> *current = node;
  while (current->left != NULL)
    current = current->left;
  return current;
}

// Search for a specific node
template <typename T>
Node<T>* getNode(Node<T> *node, T key){
    // Find the correct postion and insert the node
    if (node == NULL)
        return NULL;
    if (key < node->key)
        return getNode(node->left, key);
    else if (key > node->key)
        return getNode(node->right, key);
    else
        return node;
}

// Delete a node
template <typename T>
Node<T> *deleteNode(Node<T> *root, T key) {
  // Find the node and delete it
  if (root == NULL)
    return root;
  if (key < root->key)
    root->left = deleteNode(root->left, key);
  else if (key > root->key)
    root->right = deleteNode(root->right, key);
  else {
    if ((root->left == NULL) ||
      (root->right == NULL)) {
      Node<T> *temp = root->left ? root->left : root->right;
      if (temp == NULL) {
        temp = root;
        root = NULL;
      } else
        *root = *temp;
      free(temp);
    } else {
      Node<T> *temp = nodeWithMimumValue(root->right);
      root->key = temp->key;
      root->right = deleteNode(root->right,
                   temp->key);
    }
  }

  if (root == NULL)
    return root;

  // Update the balance factor of each node and
  // balance the tree
  root->height = 1 + std::max(height(root->left),
               height(root->right));
  int balanceFactor = getBalanceFactor(root);
  if (balanceFactor > 1) {
    if (getBalanceFactor(root->left) >= 0) {
      return rightRotate(root);
    } else {
      root->left = leftRotate(root->left);
      return rightRotate(root);
    }
  }
  if (balanceFactor < -1) {
    if (getBalanceFactor(root->right) <= 0) {
      return leftRotate(root);
    } else {
      root->right = rightRotate(root->right);
      return leftRotate(root);
    }
  }
  return root;
}

// Print the tree
template <typename T>
void printTree(Node<T> *root, std::string indent, bool last) {
  if (root != nullptr) {
    std::cout << indent;
    if (last) {
      std::cout << "R----";
      indent += "   ";
    } else {
      std::cout << "L----";
      indent += "|  ";
    }
    std::cout << root->key << std::endl;
    printTree(root->left, indent, false);
    printTree(root->right, indent, true);
  }
}

template <typename T>
void tree_to_vec(Node<T>* node, std::vector<Node<T>*>& vec){
  if(node == NULL)
    return;
  tree_to_vec(node->left, vec);
  tree_to_vec(node->right, vec);
  vec.emplace_back(node);
}

#endif