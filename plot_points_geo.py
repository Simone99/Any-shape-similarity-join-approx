# Script to plot latitude and longitude points on Earth

import matplotlib.pyplot as plt
import math

POINT_SEPARATOR = ' | '
TO_GEO = False
MAX_INTERVAL = 10000.0

colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']

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

def to_geo(point_map):
    for key, values in point_map.items():
        for i in range(len(values)):
            tmp = list(point_map[key][i])
            tmp[0] =  -90.0 + tmp[0] * 180 / MAX_INTERVAL
            tmp[1] =  -180.0 + tmp[1] * 360 / MAX_INTERVAL
            point_map[key][i] = tuple(tmp)

def draw_points(point_map, length, ax):
    for i in range(length):
        point_map[i] = [*set(point_map[i])]
        x = [math.cos(math.radians(point[1]))*6371 for point in point_map[i]]
        y = [math.sin(math.radians(point[1]))*6371 for point in point_map[i]]
        z = [math.sin(math.radians(point[0]))*6371 for point in point_map[i]]
        ax.scatter(x, y, z, c=colors[i])

def draw_edges(points, edges, ax):
    for edge in edges:
        x = [math.cos(math.radians(points[edge[0]][1]))*6371, math.cos(math.radians(points[edge[1]][1]))*6371]
        y = [math.sin(math.radians(points[edge[0]][1]))*6371, math.sin(math.radians(points[edge[1]][1]))*6371]
        z = [math.sin(math.radians(points[edge[0]][0]))*6371, math.sin(math.radians(points[edge[1]][0]))*6371]
        ax.plot(x, y, z, color='#000000')
        

if __name__ == '__main__':

    V = None
    edge_list = []
    with open("input_graph.txt", 'r') as graph_file:
        V = int(graph_file.readline())
        E = int(graph_file.readline())
        for i in range(E):
            edges_str = graph_file.readline().strip().split(" ")
            edge_list.append((int(edges_str[0]), int(edges_str[1])))

    point_map = {}
    for i in range(V):
        point_map[i] = []
    ax = plt.figure().add_subplot(projection='3d')
    with open("query_result.txt", "r") as f:
        for line in f.readlines():
            points_str = line.strip().split(POINT_SEPARATOR)
            points = []
            key = 0
            for point_str in points_str:
                tmp = pointStr_to_point(point_str)
                point_map[key].append(tmp)
                points.append(tmp)
                key += 1
            #draw_edges(points, edge_list, ax)
    if TO_GEO:
        to_geo(point_map)
    draw_points(point_map, V, ax)
    plt.show()
