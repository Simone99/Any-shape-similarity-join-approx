#ifndef __HEAP__
#define __HEAP__

#include <vector>
#include <functional>

template<typename T>
class Heap{
    private:

        std::vector<T> container;
        std::function<bool(T, T)> comparison_function;

    public:

        Heap(){
            this->comparison_function = nullptr;
        };
        ~Heap(){

        };
        Heap(std::function<bool(T, T)> comparison_function){
            this->comparison_function = comparison_function;
        };
        void set_comparison_function(std::function<bool(T, T)> comparison_function){
            this->comparison_function = comparison_function;
        };
        void push(T cell_to_add){
            this->container.push_back(cell_to_add);
            std::push_heap(this->container.begin(), this->container.end(), this->comparison_function);
        };
        T first(){
            if(this->container.empty()) return nullptr;
            return this->container[0];
        };
        void update_heap(){
            std::make_heap(this->container.begin(), this->container.end(), this->comparison_function);
        };
};

#endif