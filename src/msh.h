#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>
#include <algorithm> 

using namespace std;

// 给pair<int,int>写的哈希函数
struct PairHash {
    size_t operator()(const pair<int , int >& p) const {
        auto h1 = hash<int >{}(p.first);
        auto h2 = hash<int >{}(p.second);
        return h1 ^ ( h2 << 1) ;
    }
};

// classe maillage 2d avec liste de noeuds et liste de triangles
class Mesh
{
private:
    vector<array<double, 2>> node; // liste des noeuds (x,y)，
                                                                // 比如 node[0] = {0.0, 0.0} 表示第一个节点的坐标是 (0.0, 0.0)
    vector<array<int, 3>> triangle; // liste des triangles (id des 3 noeuds), 
                                                                // 比如 triangle[0] = {0, 1, 2} 表示第一个三角形由节点 0、1、2 构成
    unordered_map<pair<int, int >, vector<int>, PairHash> hashGrid ; // 哈希表：key = 坐标， value = 三角形元素的编号组成的数组
    double step; // 固定步长


public:
    Mesh(string gmsh2_filename); // 构造函数：传入msh文件的路径，读取文件并构建网格
    friend ostream &operator<<(ostream &os, const Mesh &m); // 重载输出运算符
    // affichage dans un fichier lisible par gnuplot
    void writeGnuplot(const string &filename) const; // 将网格数据写入gnuplot 脚本文件
    
    // 判断点是否在三角形内
    bool isPointInTriangle(int tri_id, double x, double y) const;

    int rechercheSimple(double x, double y) const ; // 3.2 ：暴力点搜索：输入(x,y),判断哪个三角形包含这个点，返回三角形的编号，如果没有找到返回-1

    void buildHashGrid (double step = 0.1) ; // 3.3 : 预处理：构建哈希表（提前把三角形元素放进格子）
    int rechercheHash (double x, double y) const ; // 3.3 : 哈希搜索（对暴力点搜索的优化）


};

#endif
