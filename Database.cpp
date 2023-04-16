#include "Database.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#define SEPARATOR "**************************************************************"
#define FILE_NAME "input_database.txt"

float randomFloat()
{
    return ((float)(std::rand()) / (float)(RAND_MAX)) * MAX_RECORD_VALUE;
}

Database::Database(const int n_sets, const int d){
    std::ifstream input_file(FILE_NAME);
    if(input_file.good()){
        while(input_file){
            int color = 0;
            input_file >> color;
            std::string line;
            std::getline(input_file, line);
            std::getline(input_file, line);
            while(line != SEPARATOR && line != ""){
                Point p;
                std::stringstream ss(line);
                ss >> p;
                this->data[color].emplace_back(p);
                std::getline(input_file, line);
            }
        }
        input_file.close();
    }else{
        std::ofstream output_file;
        output_file.open(FILE_NAME, std::ios::out | std::ios::trunc);
        for(int i = 0; i < n_sets; i++){
            output_file << i << std::endl;
            std::vector<Point> tmp;
            for(int j = 0; j < N_MAX_RECORDS; j++){
                Point tmp_point = this->generate_random_point(d);
                output_file << tmp_point << std::endl;
                tmp.emplace_back(tmp_point);
            }
            output_file << SEPARATOR << std::endl;
            this->data[i] = tmp;
        }
        output_file.close();
    }
};

const std::unordered_map<int, std::vector<Point>>& Database::getData() const{
    return this->data;
};

std::vector<std::pair<int, std::vector<Point>>> Database::get_random_points(int percentage){
    std::vector<std::pair<int, std::vector<Point>>> result;
    for(const std::pair<int, std::vector<Point>> pair : this->data){
        int random_number = (std::rand() / (float) RAND_MAX) * 100.0;
        if(random_number < percentage){
            result.emplace_back(pair);
        }
    }
    return result;
};

Point Database::generate_random_point(int d){
    Point tmp_point;
    for(int k = 0; k < d; k++){
        tmp_point.coordinates.emplace_back(randomFloat());
    }
    return tmp_point;
};

std::ostream& operator<<(std::ostream& os, const Database& obj){
    for(const std::pair<int, std::vector<Point>> n : obj.data ){
        os << "Color: " << n.first << std::endl;
        for(const Point& p : n.second){
            os << p << std::endl;
        }
    }
    return os;
};
