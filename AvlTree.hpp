#ifndef __AVLTREE__
#define __AVLTREE__

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <ostream>
#include <iostream>
#include "Database.hpp"

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

template<typename T>
class AVLNode
{
    template<typename U> friend class AVLTree;

private:
    T key_;
    int height_;

    AVLNode<T>* left_node_;
    AVLNode<T>* right_node_;

public:
    AVLNode(T key, AVLNode<T>* left_node=nullptr, AVLNode<T>* right_node=nullptr)
    {
        key_= key;
        left_node_ = left_node;
        right_node_ = right_node;
        height_ = 0;
    }
    ~AVLNode()
    {

    }

    T* get_key()
    {
        return &(this->key_);
    }
};

template<typename T>
class AVLTree
{
private:
    AVLNode<T>* root_;

    AVLNode<T>* right_rotation(AVLNode<T>* p)
    {
        AVLNode<T>* new_p = p->left_node_;
        p->left_node_ = new_p->right_node_;
        new_p->right_node_ = p;

        p->height_ = std::max(sub_tree_height(p->left_node_), sub_tree_height(p->right_node_));
        new_p->height_ = std::max(sub_tree_height(new_p->left_node_), sub_tree_height(new_p->right_node_));

        return new_p;
    }

    AVLNode<T>* left_rotation(AVLNode<T>* p)
    {
        AVLNode<T>* new_p = p->right_node_;
        p->right_node_ = new_p->left_node_;
        new_p->left_node_ = p;

        p->height_ = std::max(sub_tree_height(p->left_node_), sub_tree_height(p->right_node_));
        new_p->height_ = std::max(sub_tree_height(new_p->left_node_), sub_tree_height(new_p->right_node_));

        return new_p;
    }

    AVLNode<T>* maximum_node_of_tree(AVLNode<T>* node)
    {
        if(node->right_node_ == nullptr) return node;
        else return maximum_node_of_tree(node->right_node_);
    }

    int sub_tree_height(AVLNode<T>* node)
    {
        return (node != nullptr)?node->height_ + 1:0;
    }

    AVLNode<T>* insert_(T key, AVLNode<T>* node)
    {
        if(node == nullptr)
        {
            return node = new AVLNode<T>(key);
        }

        if(key < node->key_) 
        {
            node->left_node_ = insert_(key, node->left_node_);
        }
        else if(key > node->key_) 
        {
            node->right_node_ = insert_(key, node->right_node_);
        }
        else if(node->key_ == key) return node;

        int height_L = sub_tree_height(node->left_node_);
        int height_R = sub_tree_height(node->right_node_);

        node->height_ = std::max(height_L, height_R);
        
        int balance_factor = height_L - height_R;

        if(balance_factor > 1)//L
        {
            if(key < node->left_node_->key_)//L
            {
                return node = right_rotation(node);
            }
            else//R
            {
                node->left_node_ = left_rotation(node->left_node_);
                return node = right_rotation(node);
            }
        }
        else if(balance_factor < -1)//R
        {
            if(key < node->right_node_->key_)//L
            {
                node->right_node_ = right_rotation(node->right_node_);
                return node = left_rotation(node);
            }
            else//R
            {
                return node = left_rotation(node);
            }
        }

        return node;
    }

    AVLNode<T>* get_(T key, AVLNode<T>* node){
        if(node == nullptr) return nullptr;

        if(key < node->key_) 
        {
            return get_(key, node->left_node_);
        }
        else if(key > node->key_) 
        {
            return get_(key, node->right_node_);
        }
        else //if(node->key_ == key) 
        {
            return node;
        }

    }

    AVLNode<T>* pop_(T key, AVLNode<T>* node)
    {
        if(node == nullptr) return nullptr;

        if(key < node->key_) 
        {
            node->left_node_ = pop_(key, node->left_node_);
        }
        else if(key > node->key_) 
        {
            node->right_node_ = pop_(key, node->right_node_);
        }
        else //if(node->key_ == key) 
        {
            if(node->left_node_ == nullptr && node->right_node_ == nullptr)
            {
                delete node;
                return nullptr;
            }
            else if(node->left_node_ == nullptr && node->right_node_ != nullptr)
            {
                AVLNode<T>* sub_right_tree = node->right_node_;
                delete node;
                return sub_right_tree;
            }
            else if(node->left_node_ != nullptr && node->right_node_ == nullptr)
            {
                AVLNode<T>* sub_left_tree = node->left_node_;
                delete node;
                return sub_left_tree;
            }
            else            
            {
                AVLNode<T>* maxium_node_in_sub_left_tree = maximum_node_of_tree(node->left_node_);
                node->key_ = maxium_node_in_sub_left_tree->key_;
                node->left_node_ = pop_(maxium_node_in_sub_left_tree->key_, node->left_node_);
            }
        }

        int height_L = sub_tree_height(node->left_node_);
        int height_R = sub_tree_height(node->right_node_);

        node->height_ = std::max(height_L, height_R);
        
        int balance_factor = height_L - height_R;

        if(balance_factor > 1)//L
        {
            if(sub_tree_height(node->left_node_->left_node_) > sub_tree_height(node->left_node_->right_node_))//L
            {
                return node = right_rotation(node);
            }
            else//R
            {
                node->left_node_ = left_rotation(node->left_node_);
                return node = right_rotation(node);
            }
        }
        else if(balance_factor < -1)//R
        {
            if(sub_tree_height(node->right_node_->left_node_) > sub_tree_height(node->right_node_->right_node_))//L
            {
                node->right_node_ = right_rotation(node->right_node_);
                return node = left_rotation(node);
            }
            else//R
            {
                return node = left_rotation(node);
            }
        }

        return node;
    }

    void to_vec_(AVLNode<T>* node, std::vector<AVLNode<T>*>* result)
    {
        if(node == nullptr)
            return;

        to_vec_(node->left_node_, result);
        to_vec_(node->right_node_, result);
        (*result).emplace_back(node);
    }

public:

    AVLTree()
    {
        root_ = nullptr;
    }

    ~AVLTree()
    {

    }

    void insert(T key)
    {
        root_ = insert_(key, root_);
    }

    AVLNode<T>* get(T key)
    {
        return get_(key, root_);
    }

    void pop(T key)
    {
        root_ = pop_(key, root_);
    }

    std::vector<AVLNode<T>*> to_vec()
    {
        std::vector<AVLNode<T>*> result;
        to_vec_(root_, &result);
        return result;
    }

};

#endif