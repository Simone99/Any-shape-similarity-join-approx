# Script to plot 2-dimensional similarity join data

import matplotlib.pyplot as plt

POINT_SEPARATOR = ' | '

colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']

def pointStr_to_point(point_str):
    result = []
    point_str = point_str.strip()
    point_str = point_str.replace('(', '')
    point_str = point_str.replace(')', '')
    point_str = point_str.replace(' ', '')
    weight = float(point_str.split('-')[1])
    point_str = point_str.split('-')[0]
    coordinates = point_str.split(',')
    for coordinate in coordinates:
        result.append(float(coordinate))
    return tuple(result), weight

def draw_points(point_map, length):
    for i in range(length):
        point_map[i] = [*set(point_map[i])]
        x = [point[0][0] for point in point_map[i]]
        y = [point[0][1] for point in point_map[i]]
        plt.scatter(x, y, c=colors[i], vmin=0, vmax=10)
        for p in point_map[i]:
            label = "{:.2f}".format(p[1])
            plt.annotate(label, # this is the text
                    (p[0][0],p[0][1]), # these are the coordinates to position the label
                    textcoords="offset points", # how to position the text
                    xytext=(0,5), # distance from text to points (x,y)
                    ha='center') # horizontal alignment can be left, right or center


def draw_edges(points, edges):
    for edge in edges:
        x = [points[edge[0]][0][0], points[edge[1]][0][0]]
        y = [points[edge[0]][0][1], points[edge[1]][0][1]]
        plt.plot(x, y, color='#000000')
        

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
            draw_edges(points, edge_list)
    draw_points(point_map, V)
    plt.show()
