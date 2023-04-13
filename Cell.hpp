#ifndef __CELL__
#define __CELL__

#include <unordered_map>
#include <vector>
#include <ostream>
#include <iostream>
#include <algorithm>
#include "Database.hpp"

struct Cell{
    std::unordered_map<int, std::vector<Point>> points_set;
    std::vector<int> m;
    bool B_c;
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
        os << "Active: " << obj.B_c << std::endl;
        for(long unsigned int i = 0; i < obj.m.size(); i++){
            os << "m" << i << ": " << obj.m[i] << std::endl;
        }
        os << "Coordinates: ";
        for(const int coordinate : obj.coordinates){
            os << coordinate << " ";
        }
        return os;
    }
};

#endif