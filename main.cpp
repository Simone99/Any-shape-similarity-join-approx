#include "Graph.hpp"
#include "Grid.hpp"
#include <fstream>
#include <chrono>
#include <thread>
#include <queue>
#include <string>
#include <sstream>

#define GRAPH_FILE "input_graph.txt"
#define OUT_NAME "Test_results.txt"
#define TEST_CASE_SWITCH true
#define VARIABLE_R false

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

std::vector<unsigned long int> check_real_approx(Graph& g, float R){
    long unsigned int real_shapes = 0, approx_shapes = 0;
    std::ifstream input_file(QUERY_RESULT_OUTPUT_FILE);
    if(input_file.good()){
        while(input_file){
            std::string shape;
            std::getline(input_file, shape);
            if(!shape.empty()){
                std::stringstream ss(shape);
                std::vector<Point> shape_vertices;
                std::string vertex;
                int i;
                for(i = 0; i < g.V; i++){
                    std::getline(ss, vertex, '|');
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
                        if(shape_vertices[v_j].distance_from(shape_vertices[v_h]) > R){
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
        }
    }
    input_file.close();
    return std::vector<unsigned long int> {real_shapes, approx_shapes};
}

void test_case(Graph& g, float R, int n_dimensions, float eps, unsigned int n_to_report, std::ofstream& out){
    out << "Test case R(" << R <<"), d(" << n_dimensions << "), eps(" << eps << ")\n";
    Database db(g.V, n_dimensions);

    std::cout << "Initializing the grid..." << std::endl;
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Grid gd(db, g, eps, R);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    out << "Time to build the grid = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    std::cout << "Answering query..." << std::endl;
    remove(QUERY_RESULT_OUTPUT_FILE);
    begin = std::chrono::high_resolution_clock::now();
    gd.answer_query(g, n_to_report);
    end = std::chrono::high_resolution_clock::now();
    out << "Time to answer the query = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    std::cout << "Checking shapes..." << std::endl;
    std::vector<unsigned long int> real_approx = check_real_approx(g, R);
    out << "Real shapes = " << real_approx[0] << "\nApprox shapes = " << real_approx[1] << std::endl;
    out << "Approximation ratio = " << (1.0 + (real_approx[0] != 0? real_approx[1] / (double)real_approx[0] : 0.0)) << std::endl;
}

void test_case_1(Graph& g, float R, int n_dimensions, float eps, int initialization_percentage, int percentage_to_split, unsigned int n_to_report, std::ofstream& out){
    out << "Test case R(" << R <<"), d(" << n_dimensions << "), eps(" << eps << "), init_percentage(" << initialization_percentage << "), percentage_to_split(" << percentage_to_split << ")\n";
    Database db(g.V, n_dimensions);

    std::vector<Database> db_splitted = db.split_points(initialization_percentage);

    std::cout << "Initializing the grid..." << std::endl;
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Grid gd(db_splitted[0], g, eps, R);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    out << "Time to build the grid (" << initialization_percentage << "%) = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;

    Database to_split = db_splitted[1];
    std::vector<float> update_times, query_times;
    std::vector<std::pair<int, int>> real_approx_shapes;
    for(int i = 0; i < 100 / percentage_to_split; i++){
        std::cout << "---------------------" << std::endl << "Iteration " << i + 1 << std::endl;
        std::vector<Database> split = to_split.split_points(percentage_to_split);
        to_split = split[1];
        int n_points_added = 0;

        std::cout << "Adding points..." << std::endl;
        begin = std::chrono::high_resolution_clock::now();
        for(std::pair<int, std::vector<Point>> pair : split[0].getData()){
            for(const Point& p : pair.second){
                gd.add_point(pair.first, p, g);
                n_points_added++;
            }
        }
        end = std::chrono::high_resolution_clock::now();
        update_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / (1000.0 * (n_points_added != 0? (float)n_points_added : 1.0)));

        std::cout << "Answering query..." << std::endl;
        remove(QUERY_RESULT_OUTPUT_FILE);
        begin = std::chrono::high_resolution_clock::now();
        gd.answer_query(g, n_to_report);
        end = std::chrono::high_resolution_clock::now();
        query_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0);

        std::cout << "Checking shapes..." << std::endl;
        std::vector<unsigned long int> real_approx = check_real_approx(g, R);
        real_approx_shapes.push_back(std::pair<int, int>{real_approx[0], real_approx[1]});
    }

    out << "Update time after each update: ";
    for(float update_time : update_times){
        out << update_time << "s ";
    }
    out << std::endl;

    out << "Query time after each update: ";
    for(float query_time : query_times){
        out << query_time << "s ";
    }
    out << std::endl;

    out << "(Real, approx) shapes after each update: ";
    for(std::pair<int, int> r_a : real_approx_shapes){
        out << "(" << r_a.first << ", " << r_a.second << ") - ";
    }
    out << std::endl;

}

void test_case_3(Grid* grids, Graph& g, std::vector<float>& rs, float radius, unsigned int n_to_report, int end_i){
    int l = 0, r = end_i;
    while (l <= r) {
        int m = l + (r - l) / 2;
    
        // If x greater, ignore left half
        if (rs[m] < radius)
            l = m + 1;

        // If x is smaller, ignore right half
        else
            r = m - 1;
    }

    std::cout << "Answering query with radius: " << rs[r] << std::endl;
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    grids[r].answer_query(g, n_to_report);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Time to answer the query: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;
}

int main(){

    std::ofstream output_file;
    std::ifstream input_file(GRAPH_FILE);
    output_file.open(OUT_NAME, std::ios::out | std::ios::trunc);
    Graph g(input_file);
    input_file.close();

    if(!VARIABLE_R){
        if(TEST_CASE_SWITCH){
            // Tests with different R
            std::cout << "Running tests with different k..." << std::endl;
            test_case(g, 1.5, 2, 0.1, 100, output_file);
        }else{
            if(GEO){
                test_case_1(g, 5, 2, 0.01, 50, 5, 100, output_file);
            }else{
                test_case_1(g, 0.5, 2, 0.01, 50, 5, 100, output_file);
            }
        }
    }else{

        // Calculate the dataset spread
        Database db(g.V, 2);
        float spread = db.get_spread();

        // Set all needed parameters
        float eps = 0.1;
        float R = MAX_RECORD_VALUE * sqrt(2.0);
        int end_i = log(spread) / log(1 + eps / 4);

        // Create the r vector
        std::vector<float> rs;
        for(int i = 0; i <= end_i; i++){
            rs.push_back(R / spread * pow(1 + eps / 4, i));
        }

        // Creat all the needed grids
        Grid* grids = new Grid[end_i + 1];
        unsigned int max_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        auto lambda = [&grids, &rs, &eps, &g, &db](const int init, const int step){
            for(long unsigned int i = init; i < rs.size(); i+=step){
                grids[i] = Grid(db, g, eps, rs[i]);
            }
        };
        unsigned int i;
        std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        for(i = 0; i < max_threads; i++){
            threads.push_back(std::thread(lambda, i, max_threads));
        }
        for(i = 0; i < max_threads; i++){
            threads[i].join();
        }
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::cout << "Time to build all grids: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;
        
        test_case_3(grids, g, rs, 0.84, 100, end_i); 
        test_case_3(grids, g, rs, 1.27, 100, end_i);
        test_case_3(grids, g, rs, 1.93, 100, end_i);
        test_case_3(grids, g, rs, 3.03, 100, end_i);
        
        delete[] grids;
    }

    output_file.close();

    return 0;
}