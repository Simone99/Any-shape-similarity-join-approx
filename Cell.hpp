#ifndef __CELL__
#define __CELL__

#include <unordered_map>
#include <vector>
#include <ostream>
#include <iostream>
#include <algorithm>
#include "Database.hpp"

/*
    Used to compile different code versions according to the distance method we want to use.

    0 - distance between the points stored within the cell
    1 - distance between all vertices
    2 - distance between cell centers
*/
#define CELL_DISTANCE_METHOD 2

struct Cell{
    std::unordered_map<int, std::vector<Point>> points_set;
    std::vector<int> m;
    int m_c;
    std::vector<int> coordinates;
    #if CELL_DISTANCE_METHOD == 1
    // If we check all vertices
    std::vector<Point> cell_vertices;
    #endif
    #if CELL_DISTANCE_METHOD == 2
    // If we check just the center
    Point cell_center;
    float eps;
    #endif

    float distance_from(const Cell& other){
        float min_distance = INT32_MAX;
        #if CELL_DISTANCE_METHOD == 0
        for(const std::pair<int, std::vector<Point>> set_a : this->points_set){
            for(const std::pair<int, std::vector<Point>> set_b : other.points_set){
                for(Point a : set_a.second){
                    for(const Point& b : set_b.second){
                        float distance = a.distance_from(b);
                        if(distance < min_distance)
                            min_distance = distance;
                    }
                }
            }
        }
        #endif

        #if CELL_DISTANCE_METHOD == 1
        for(const Point& a : this->cell_vertices){
            for(const Point& b : other.cell_vertices){
                float distance = a.distance_from(b);
                if(distance < min_distance)
                    min_distance = distance;
            }
        }
        #endif

        #if CELL_DISTANCE_METHOD == 2
        #if POINT_DISTANCE_METRIC == 0
        // Somehow it doesn't work
        min_distance = this->cell_center.distance_from(other.cell_center) - this->eps;
        #endif
        #if POINT_DISTANCE_METRIC == 1
        // Somehow it doesn't work
        min_distance = this->cell_center.distance_from(other.cell_center) - this->eps;
        #endif
        #if POINT_DISTANCE_METRIC == 2
        min_distance = this->cell_center.distance_from(other.cell_center) - sqrt(cell_center.coordinates.size()) * this->eps;
        #endif
        #endif

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
        os << "Number of solutions: " << obj.m_c << std::endl;
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