#include "Database.hpp"
#include "Grid.hpp"
#include <iostream>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#define OUT_NAME "Test_results.txt"

int N_SETS = 3;
int N_DIMENSIONS = 2;

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

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

    std::ifstream input_file(QUERY_RESULT_OUTPUT_FILE);
    if(input_file.good()){
        long unsigned int real_triangles = 0, approx_triangles = 0;
        while(input_file){
            std::string triangle;
            std::getline(input_file, triangle);
            std::stringstream ss(triangle);
            std::vector<Point> triangle_vertices;
            std::string vertex;
            int i;
            for(i = 0; i < N_SETS; i++){
                std::getline(ss, vertex, '-');
                vertex = trim(vertex);
                std::stringstream vertex_ss(vertex);
                Point point_tmp;
                vertex_ss >> point_tmp;
                triangle_vertices.emplace_back(point_tmp);
            }
            if(triangle_vertices[0].euclidean_distance_from(triangle_vertices[1]) > R ||
                triangle_vertices[1].euclidean_distance_from(triangle_vertices[2]) > R ||
                triangle_vertices[2].euclidean_distance_from(triangle_vertices[0]) > R){
                    approx_triangles++;
            }else{
                real_triangles++;
            }
        }
        out << "Real triangles = " << real_triangles << "\nApprox triangles = " << approx_triangles << std::endl;
        out << "Approximation ratio = " << (1.0 + approx_triangles / (double)real_triangles) << std::endl;
    }
    input_file.close();
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