#include "Database.h"
#include "Grid.h"
#include "InputParse.h"
#include <iostream>
#include <chrono>

int N_SETS = 3;
int N_DIMENSIONS = 2;
float EPS = 0.1;

int main(int argc, char **argv){

    float R = 1.5;
    InputParser input(argc, argv);
    if(input.cmdOptionExists("-R")){
        try
        {
            R = std::stof(input.getCmdOption("-R"));
        }
        catch(const std::invalid_argument& e)
        {
            std::cerr << "Please enter for example: " << argv[0] << " -R 0.5" << std::endl;
            std::cerr << "Default R value = 1.5" << std::endl;
        }
    }

    Database db(N_SETS, N_DIMENSIONS);

    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Grid gd(db, EPS, R);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Time to build the grid = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    gd.answer_query();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Time to answer the query = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    return 0;
}