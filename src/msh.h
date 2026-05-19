#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

using namespace std;

// classe maillage 2d avec liste de noeuds et liste de triangles
class Mesh
{
private:
    vector<array<double, 2>> node; // liste des noeuds (x,y)，
                                                                // 比如 node[0] = {0.0, 0.0} 表示第一个节点的坐标是 (0.0, 0.0)
    vector<array<int, 3>> triangle; // liste des triangles (id des 3 noeuds), 
                                                                // 比如 triangle[0] = {0, 1, 2} 表示第一个三角形由节点 0、1、2 构成

public:
    Mesh(string gmsh2_filename); // 构造函数：传入msh文件的路径，读取文件并构建网格
    friend ostream &operator<<(ostream &os, const Mesh &m); // 重载输出运算符
    // affichage dans un fichier lisible par gnuplot
    void writeGnuplot(const string &filename) const; // 将网格数据写入gnuplot 脚本文件
};

#endif
