# Script to plot 2D triangle similarity join data

import matplotlib.pyplot as plt
import copy

def pointStr_to_point(point_str):
    result = []
    point_str = point_str.strip()
    point_str = point_str.replace('(', '')
    point_str = point_str.replace(')', '')
    point_str = point_str.replace(' ', '')
    coordinates = point_str.split(',')
    for coordinate in coordinates:
        result.append(float(coordinate))
    return tuple(result)

def draw_points(point_map):
    point_map[0] = [*set(point_map[0])]
    point_map[1] = [*set(point_map[1])]
    point_map[2] = [*set(point_map[2])]
    x = [point[0] for point in point_map[0]]
    y = [point[1] for point in point_map[0]]
    plt.scatter(x, y, c='#0000ff', vmin=0, vmax=100)
    x = [point[0] for point in point_map[1]]
    y = [point[1] for point in point_map[1]]
    plt.scatter(x, y, c='#ff0000', vmin=0, vmax=100)
    x = [point[0] for point in point_map[2]]
    y = [point[1] for point in point_map[2]]
    plt.scatter(x, y, c='#00ff00', vmin=0, vmax=100)

def draw_edges(point_map):
    for i in range(len(point_map[0])):
        x = [point_map[0][i][0], point_map[1][i][0]]
        y = [point_map[0][i][1], point_map[1][i][1]]
        plt.plot(x, y, color='#000000')
        x = [point_map[1][i][0], point_map[2][i][0]]
        y = [point_map[1][i][1], point_map[2][i][1]]
        plt.plot(x, y, color='#000000')
        x = [point_map[2][i][0], point_map[0][i][0]]
        y = [point_map[2][i][1], point_map[0][i][1]]
        plt.plot(x, y, color='#000000')
        

if __name__ == '__main__':
    point_map = {
        0: [],
        1: [],
        2: []
    }
    with open("query_result.txt", "r") as f:
        for line in f.readlines():
            points_str = line.strip().split(' - ')
            key = 0
            for point_str in points_str:
                point_map[key].append(pointStr_to_point(point_str))
                key += 1
    draw_edges(copy.deepcopy(point_map))
    draw_points(point_map)
    plt.show()
