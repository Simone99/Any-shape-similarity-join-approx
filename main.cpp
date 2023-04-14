#include "Graph.hpp"
#include "Grid.hpp"
#include <fstream>
#include <chrono>
#include <queue>
#include <string>
#include <sstream>

#define GRAPH_FILE "input_graph.txt"
#define OUT_NAME "Test_results.txt"

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

void test_case(Graph& g, float R, int n_dimensions, float eps, std::ofstream& out){
    out << "Test case R(" << R <<"), d(" << n_dimensions << "), eps(" << eps << ")\n";
    Database db(g.V, n_dimensions);

    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Grid gd(db, g, eps, R);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    out << "Time to build the grid = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    remove(QUERY_RESULT_OUTPUT_FILE);
    begin = std::chrono::high_resolution_clock::now();
    gd.answer_query(g);
    end = std::chrono::high_resolution_clock::now();
    out << "Time to answer the query = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    std::ifstream input_file(QUERY_RESULT_OUTPUT_FILE);
    if(input_file.good()){
        long unsigned int real_shapes = 0, approx_shapes = 0;
        while(input_file){
            std::string shape;
            std::getline(input_file, shape);
            std::stringstream ss(shape);
            std::vector<Point> shape_vertices;
            std::string vertex;
            int i;
            for(i = 0; i < g.V; i++){
                std::getline(ss, vertex, '-');
                vertex = trim(vertex);
                std::stringstream vertex_ss(vertex);
                Point point_tmp;
                vertex_ss >> point_tmp;
                shape_vertices.push_back(point_tmp);
            }
            // Queue for BFS
            std::queue<int> Q;
            // Initialize bool vector for keeping track of visited nodes
            std::vector<bool> visited;
            for(int i = 0; i < g.V; i++){
                visited.push_back(false);
            }
            visited[0] = true;
            // Insert vertex into queue
            Q.push(0);
            // Initialize variables
            bool real = true;
            // Start BFS loop
            while (!Q.empty() && real)
            {
                int v_j = Q.front();
                Q.pop();
                for(int v_h : g.adj_list[v_j]){
                    if(!visited[v_h]){
                        visited[v_h] = true;
                        Q.push(v_h);
                    }
                    if(shape_vertices[v_j].euclidean_distance_from(shape_vertices[v_h]) > R){
                        real = false;
                    }
                }
            }

            if(real){
                real_shapes++;
            }else{
                approx_shapes++;
            }
        }
        out << "Real shapes = " << real_shapes << "\nApprox shapes = " << approx_shapes << std::endl;
        out << "Approximation ratio = " << (1.0 + approx_shapes / (double)real_shapes) << std::endl;
    }
    input_file.close();
}

int main(){

    std::ofstream output_file;
    std::ifstream input_file(GRAPH_FILE);
    output_file.open(OUT_NAME, std::ios::out | std::ios::trunc);
    Graph g(input_file);
    input_file.close();

    // Tests with different R
    std::cout << "Running tests with different Rs..." << std::endl;
    test_case(g, 1.5, 2, 0.1, output_file);
    test_case(g, 1, 2, 0.1, output_file);
    test_case(g, 0.5, 2, 0.1, output_file);
    test_case(g, 2, 2, 0.1, output_file);
    test_case(g, 4, 2, 0.1, output_file);

    // Tests with different epsilon
    std::cout << "Running tests with different epsilons..." << std::endl;
    test_case(g, 1.5, 2, 0.1, output_file);
    test_case(g, 1.5, 2, 0.35, output_file);
    test_case(g, 1.5, 2, 0.065, output_file);
    test_case(g, 1.5, 2, 1, output_file);
    test_case(g, 1.5, 2, 0.01, output_file);

    // Tests with different dimensions
    std::cout << "Running tests with different dimensions..." << std::endl;
    remove("input_database.txt");
    test_case(g, 1.5, 7, 0.1, output_file);
    remove("input_database.txt");
    test_case(g, 1.5, 3, 0.1, output_file);
    remove("input_database.txt");
    test_case(g, 1.5, 4, 0.1, output_file);
    remove("input_database.txt");
    test_case(g, 1.5, 2, 0.1, output_file);

    output_file.close();

    return 0;
}