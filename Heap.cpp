#include "Heap.hpp"

// auto comparison_function = [](Cell* a, Cell* b){
//         return *a < *b;
//     };

template <typename T>
Heap<T>::Heap(){
    this->comparison_function = nullptr;
};

template<typename T>
Heap<T>::~Heap(){

};

template <typename T>
Heap<T>::Heap(std::function<bool(T, T)> comparison_function){
    this->comparison_function = comparison_function;
};

template <typename T>
void Heap<T>::set_comparison_function(std::function<bool(T, T)> comparison_function){
    this->comparison_function = comparison_function;
}

template <typename T>
void Heap<T>::push(T cell_to_add){
    this->container.push_back(cell_to_add);
    std::push_heap(this->container.begin(), this->container.end(), this->comparison_function);
};

template <typename T>
T Heap<T>::first(){
    return this->container[0];
};

template <typename T>
void Heap<T>::update_heap(){
    std::make_heap(this->container.begin(), this->container.end(), this->comparison_function);
};
