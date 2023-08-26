#ifndef __HEAP__
#define __HEAP__

#include <vector>
#include <functional>
#include "Cell.hpp"
#include <unordered_set>

// Hash function from https://www.geeksforgeeks.org/unordered-set-of-vectors-in-c-with-examples/
struct hashFunction 
{
  size_t operator()(const std::vector<int>& myVector) const 
  {
    std::hash<int> hasher;
    size_t answer = 0;
      
    for (int i : myVector) 
    {
      answer ^= hasher(i) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
    }
    return answer;
  }
};

template<typename T>
class Heap{

    private:

        std::vector<T> container;
        std::function<bool(T&, T&)> comparison_function;

    public:

        Heap(){
            this->comparison_function = nullptr;
        };
        ~Heap(){

        };
        Heap(std::function<bool(T&, T&)> comparison_function){
            this->comparison_function = comparison_function;
        };
        void set_comparison_function(std::function<bool(T&, T&)> comparison_function){
            this->comparison_function = comparison_function;
        };
        void push(T to_add){
            this->container.push_back(to_add);
            std::push_heap(this->container.begin(), this->container.end(), this->comparison_function);
        };
        T* first(){
            if(this->container.empty()) return nullptr;
            return &this->container[0];
        };
        void update_heap(){
            std::make_heap(this->container.begin(), this->container.end(), this->comparison_function);
        };
        void pop(){
            pop_heap(this->container.begin(), this->container.end());
            this->container.pop_back();
            return;
        };
        T* search_element(T el){
            for(auto it = this->container.begin(); it != this->container.end(); it++){
                if(*it == el)
                    return &(*it);
            }
            return nullptr;
        };
        std::vector<T>* get_container(){
            return &(this->container);
        };
};

struct HeapNode{
    std::vector<Cell*> cells;
    float priority;

    friend bool operator==(const HeapNode& l, const HeapNode& r){
        return l.cells == r.cells;
    }
};

#endif