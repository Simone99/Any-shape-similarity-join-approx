#include "Database.hpp"
#include "Grid.hpp"
#include <iostream>
#include <chrono>
#include <cstdio>
#include <fstream>
#define OUT_NAME "Test_results.txt"

int N_SETS = 3;
int N_DIMENSIONS = 2;

void test_case(float R, int n_dimensions, float eps, std::ofstream& out){
    out << "Test case R(" << R <<"), d(" << n_dimensions << "), eps(" << eps << ")\n";
    Database db(N_SETS, n_dimensions);

    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Grid gd(db, eps, R);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    out << "Time to build the grid = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    gd.answer_query();
    end = std::chrono::high_resolution_clock::now();
    out << "Time to answer the query = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

}

int main(int argc, char **argv){

    std::ofstream output_file;
    output_file.open(OUT_NAME, std::ios::out | std::ios::trunc);

    // Tests with different R
    std::cout << "Running tests with different Rs..." << std::endl;
    test_case(1.5, 2, 0.1, output_file);
    test_case(1, 2, 0.1, output_file);
    test_case(0.5, 2, 0.1, output_file);
    test_case(2, 2, 0.1, output_file);
    test_case(4, 2, 0.1, output_file);

    // Tests with different epsilon
    std::cout << "Running tests with different epsilons..." << std::endl;
    test_case(1.5, 2, 0.1, output_file);
    test_case(1.5, 2, 0.35, output_file);
    test_case(1.5, 2, 0.065, output_file);
    test_case(1.5, 2, 1, output_file);
    test_case(1.5, 2, 0.01, output_file);

    // Tests with different dimensions
    std::cout << "Running tests with different dimensions..." << std::endl;
    remove("input_database.txt");
    test_case(1.5, 7, 0.1, output_file);
    remove("input_database.txt");
    test_case(1.5, 3, 0.1, output_file);
    remove("input_database.txt");
    test_case(1.5, 4, 0.1, output_file);
    remove("input_database.txt");
    test_case(1.5, 2, 0.1, output_file);

    output_file.close();

    return 0;
}