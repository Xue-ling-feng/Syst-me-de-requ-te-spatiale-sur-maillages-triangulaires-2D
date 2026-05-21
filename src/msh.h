#ifndef MESH_H
#define MESH_H
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <utility>
#include <algorithm> 
#include "parallel_hashmap/phmap.h"

using namespace std;

// 给pair<int,int>写的哈希函数
// struct PairHash {
//     size_t operator()(const pair<int , int >& p) const {
//         auto h1 = hash<int >{}(p.first);
//         auto h2 = hash<int >{}(p.second);
//         return h1 ^ ( h2 << 1) ;
//     }
// };

// 优化 PairHash（减少冲突，phmap 也兼容这个哈希函数）
struct PairHash {
    size_t operator()(const pair<int , int >& p) const {
        // 更高效的组合方式：避免简单异或的冲突
        size_t h = hash<int>()(p.first);
        h ^= hash<int>()(p.second) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
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
    // unordered_map<pair<int, int >, vector<int>, PairHash> hashGrid ; // 哈希表：key = 坐标， value = 三角形元素的编号组成的数组
    double step; // 固定步长

    phmap::flat_hash_map<pair<int, int>, vector<int>, PairHash> hashGrid; // 在 parallel_hashmap 下的新hash表

    phmap::flat_hash_map<pair<int, int> , vector<int > , PairHash> hashGrid_edge; // parallel_hashmap 下的找相邻三角形的hash表

    bool isPointInTriangle(int tri_id, double x, double y) const; // 辅助函数：判断点是否在三角形内
    bool isShareEdge(int tri_id1, int tri_id2) const ;  // 辅助函数：判断两个三角形是否共边：
    vector<pair<int, int >> getEdges (const array<int , 3>& tri) const ; // 辅助函数：生成标准化边

public:
    Mesh(string gmsh2_filename); // 构造函数：传入msh文件的路径，读取文件并构建网格
    friend ostream &operator<<(ostream &os, const Mesh &m); // 重载输出运算符
    // affichage dans un fichier lisible par gnuplot
    void writeGnuplot(const string &filename) const; // 将网格数据写入gnuplot 脚本文件

    int rechercheSimple(double x, double y) const ; // 3.2 ：暴力点搜索：输入(x,y),判断哪个三角形包含这个点，返回三角形的编号，如果没有找到返回-1

    void buildHashGrid (double step = 0.1) ; // 3.3 : 预处理：构建哈希表（提前把三角形元素放进格子）
    int rechercheHash (double x, double y) const ; // 3.3 : 哈希搜索（对暴力点搜索的优化）

    vector<int> rechercheTriVNaive (int n ) const ; // 3.5 : fonction naive : 输入指定三角形元素的编号n ( 对应的下标tri_id = n-1)，返回这个三角形元素周围的三角形元素组成的列表
    void buildHashGrid_edge() ; // 3.5： 预处理：构建哈希表
    vector<int > rechercheTriVHash (int n ) const ; // 3.5 : 哈希搜索

    void vsPoint (const string &filename, double x , double y , int tri_id ) const ; //  3.6： 定点标记为绿色，包含它的三角形标记为红色
    void vsNeighbors (const string &filename, int n , const vector<int> &neighbors ) const ; // 3.6 : 指定的三角形标记为红色，它的相邻三角形标记为蓝色

};

#endif
