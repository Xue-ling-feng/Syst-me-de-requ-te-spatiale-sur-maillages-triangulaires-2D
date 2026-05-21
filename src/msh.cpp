#include "msh.h"
#include <cassert>
#include <string>



// classe maillage 2d avec liste de noeuds et liste de triangles
// class Mesh 的构造函数：
Mesh::Mesh(string gmsh2_filename) // 输入 .msh 文件 
{
    ifstream file = ifstream(gmsh2_filename); // ifstream = 一个 C++ 专门用来「读文本文件」的工具
                                                                                // 读取 .msh 文件 储存 进 file 变量
    // 如果文件打不开，抛出异常：
    if (!file.is_open()) {
        throw runtime_error("file not found");
    }
    string line;
    while (std::getline(file, line)) // 遍历文件的每一行
                                                        // getline = 按行读取 : 从 file 里读取一行文本，储存在 line 里
    {
        // 解析 $Nodes 部分：
        if (line == "$Nodes")
        {
            int n;
            file >> n; // 按数字读取 
                            // 读取 n 是因为 n 是节点数量
            node.resize(n); // node 开辟 n 个位置
            for (int i = 0; i < n; i++)
            {
                int id;
                double z;
                file >> id; // 读取节点编号
                file >> node[id - 1][0] >> node[id - 1][1] >> z; // 读取坐标(x,y,z)，作为一个数组储存进 node 的第 id-1 个位置，
                // file >> 储存连续数字到一个数组中，只需要提供最后一个变量就可以了
                // std::array<double , 3 > t ; file >> t[0] >> t[1] >> t[2] ; node.push_back(t) ; 也可以这样写
            }
        }
        if (line == "$Elements")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; i++)
            {
                int id, type, ntags; 
                file >> id >> type >> ntags; // 编号，类型，标签数量
                for (int j = 0; j < ntags; j++)  // 将 ntags 后面两个数字读取但不使用
                // 因为后面两个数字，一个是物理标签，一个是几何标签，在这个程序里不用
                {
                    int tag;
                    file >> tag;
                }
                if (type == 2) // 只在 triangle 中储存三角形单元
                {
                    array<int, 3> t;
                    file >> t[0] >> t[1] >> t[2];
                    t[0]--;
                    t[1]--;
                    t[2]--; // 为了在 node 数组中找得到，所以用node数组中的字节编号
                    triangle.push_back(t); 
                } // sinon sauter la ligne (on ne connait pas sa taille)
                else
                {
                    string line;
                    std::getline(file, line); // 直接跳过这一行
                }
            }
        }
    }
};

// affichage dans un fichier lisible par gnuplot
// class Mesh 的成员函数：
void Mesh::writeGnuplot(const string &filename) const
{
    ofstream file(filename); // ofstream = 一个 C++ 专门用来「写文本文件」的工具

    // file << "set terminal postscript eps enhanced color" << endl; // 把图片输出成 .eps 格式（矢量图），带颜色。
    // file << "set output '" << filename << ".eps'" << endl; // 把画好的图保存成 xxx.eps 文件。
    //  file << "set xrange [-1:1]" << endl; // 强制 X 轴范围从 -1 到 1
    //  file << "set yrange [-1:1]" << endl; // 强制 Y 轴范围从 -1 到 1。
    //  freeze aspect ratio // 冻结横纵比例

    file << "set size ratio -1" << endl; // 让 x 轴和 y 轴的单位长度相同，不然的话会让图形视觉效果上变形
                                                                    // - 表示自动保持真实几何比例
    file << "plot '-' with linespoints" << endl; // 开始画图，数据从下面的文字里读取，用线、点 linespoints 画出来
                                                                                // ‘-’ 表示数据从下面的文字里读取

    for (int i = 0; i < triangle.size(); i++)
    {
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; // 画三角形的第一个点， 以13 2 2 0 1 6 3 11举例，画出了 6 和 3 这两个点
        file << node[triangle[i][1]][0] << " " << node[triangle[i][1]][1] << endl; // 画三角形的第二个点，以13 2 2 0 1 6 3 11举例，画出了 3 和 11 这两个点
        file << node[triangle[i][2]][0] << " " << node[triangle[i][2]][1] << endl; // 画三角形的第三个点， 以13 2 2 0 1 6 3 11举例，画出了 11 和 6 这两个点
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; // 画三角形的第一个点（闭合三角形）
        file << endl; // 空行分隔不同的三角形
    }
    file << "e" << endl;  // 告诉Gnuplot数据结束
    file << "pause -1";  // 暂停Gnuplot窗口（防止闪退）
    file.close();
}

// clss Mesh 的重载运算符<< :
ostream &operator<<(ostream &os, const Mesh &m)
{
    os << "Mesh with " << m.node.size() << " nodes and " << m.triangle.size() << " triangles" << endl;
    for (int i = 0; i < m.node.size(); i++)
    {
        os << "Node " << i << ": " << m.node[i][0] << " " << m.node[i][1] << endl;
    }
    for (int i = 0; i < m.triangle.size(); i++)
    {
        os << "Triangle " << i << ": " << m.triangle[i][0] << " " << m.triangle[i][1] << " " << m.triangle[i][2] << endl;
    }
    return os;
}

// =============================================================================================================
// 3.2 ：暴力点搜索：输入(x,y),判断哪个三角形包含这个点，返回三角形的编号，如果没有找到返回-1
int Mesh::rechercheSimple(double x, double y) const {
    if (x < 0 || x > 1 || y < 0 || y > 1  ){return -1;} // 如果超出范围，返回-1
    // 遍历所有 mesh 里的三角形元素
    for (int i = 0 ; i < triangle.size() ; i++ ) {
        // 提取点 A， 点B ， 点C 的坐标：
        double x1 = node [triangle[i][0]][0] ;
        double y1 = node [triangle[i][0]][1] ;
        double x2 = node [triangle[i][1]][0] ;
        double y2 = node [triangle[i][1]][1] ;
        double x3 = node [triangle[i][2]][0] ;
        double y3 = node [triangle[i][2]][1] ;
        // 求叉积
        double cross1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1) ; //  AB × AP
        double cross2 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2) ; //  BC × BP
        double cross3 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3) ; //  CA × CP
        // 判断点 (x,y) 是否在里面，是，返回三角形元素编号；否，返回-1
        // 条件：所有叉积全部大于等于0，或小于等于0 ，P在三角形ABC内
        if ((cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0)){return i +1;}   
    }
    return -1;
}

// =============================================================================================================
// 3.3 : 预处理哈希表（提前吧三角形元素放进格子）
void Mesh::buildHashGrid (double step){
    this->step = step;
    hashGrid.clear(); // 先清空哈希表
    // 遍历所有三角形元素
    for (int i = 0 ; i < triangle.size() ; i++){
        double x1 = node [triangle [i] [0] ][0];
        double x2 = node [triangle [i] [1] ][0];
        double x3 = node [triangle [i] [2] ][0];
        double y1 = node [triangle [i] [0] ][1];
        double y2 = node [triangle [i] [1] ][1];
        double y3 = node [triangle [i] [2] ][1];

        // 计算三角形元素覆盖的坐标的范围：
        int min_i = std::min({x1, x2, x3}) / step;
        int min_j = std::min({y1, y2, y3}) / step;
        int max_i = std::max({x1, x2, x3}) / step;
        int max_j = std::max({y1,y2, y3}) / step;

        // 把三角形元素的编号放进所有覆盖的格子对应的列表里：
        for (int k = min_i ; k <= max_i ; k++) {
            for (int j = min_j ; j <= max_j ; j ++ ){
                hashGrid[{k , j}].push_back(i); // 注意，这里放进去的是 triangle列表 里的三角形元素的下标，真正的编号 = i+1
            }
        }
    }
}

// 判断点是否在三角形内
bool Mesh::isPointInTriangle(int tri_id, double x, double y) const {
    double x1 = node[triangle[tri_id][0]][0];
    double y1 = node[triangle[tri_id][0]][1];
    double x2 = node[triangle[tri_id][1]][0];
    double y2 = node[triangle[tri_id][1]][1];
    double x3 = node[triangle[tri_id][2]][0];
    double y3 = node[triangle[tri_id][2]][1];

    double cross1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1);
    double cross2 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2);
    double cross3 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3);

    return (cross1 >= 0 && cross2 >= 0 && cross3 >= 0) ||
           (cross1 <= 0 && cross2 <= 0 && cross3 <= 0);
}

// 3.3 : 哈希搜索（对暴力点搜索的优化）
 int Mesh::rechercheHash (double x, double y) const {
    if (x < 0 || x > 1 || y < 0 || y > 1  ){return -1;} // 如果超出范围，返回-1
    // 计算坐标在哪个格子
    int i = x / step;
    int j = y / step;
    // 查找哈希表：拿到对应的三角形元素列表
    auto it = hashGrid.find({i, j});
    if ( it == hashGrid.end()) return -1;
    // 只遍历对应的列表中的三角形元素
    for (int tri_id : it->second) {
        if (isPointInTriangle(tri_id , x , y )) return tri_id + 1;
    }
    return -1;
}

// =============================================================================================================
// 判断两个三角形是否共边：
bool Mesh::isShareEdge(int tri_id1, int tri_id2) const {
    int common = 0 ; 
    for (int i : triangle[tri_id1]) {
        for (int j : triangle[tri_id2]) {
            if ( i == j ) common ++;
            if ( common >= 2 ) return true ; // 找到两个共点 = 共边
        }
    }
    return false;
}

// 3.5 : fonction naive : 输入指定三角形元素的编号n ( 对应的下标tri_id = n-1)，返回这个三角形元素周围的三角形元素组成的列表
std::vector<int> Mesh::rechercheTriVNaive (int n ) const {
    // 创建一个列表用于储存相邻三角形元素的编号（从1开始）
    vector<int> neighbors; 
    // triangle.size() 返回的是 size_t（无符号整数），n 是 int（有符号），不能直接比较！
    // 所以要加 static_cast<int> 把它强制变成 int
    if ( n < 1 || n > static_cast<int>(triangle.size()) ) return neighbors; 

    int tri_id = n - 1 ; // triangle 列表中三角形的下标
    for (int i = 0 ; i <  static_cast<int>(triangle.size())  ; i++ ){
        if ( i == tri_id ) continue;
        if (isShareEdge( i , tri_id)) neighbors.push_back( i + 1 ) ;
    }
    return neighbors;
}

// 辅助函数：生成标准化边
std::vector<std::pair<int, int >> Mesh::getEdges (const std::array<int , 3>& tri) const {
    std::vector<std::pair<int, int>> edges;
    edges.emplace_back(std::min(tri[0],tri[1]), std::max(tri[0],tri[1]));
    edges.emplace_back(std::min(tri[1],tri[2]), std::max(tri[1],tri[2]));
    edges.emplace_back(std::min(tri[2],tri[0]), std::max(tri[2],tri[0]));
    return edges;
}

// 3.5： 预处理：构建哈希表
void Mesh::buildHashGrid_edge() {
    hashGrid_edge.clear();
    // 遍历所有三角形元素
    for (int i = 0 ; i <  static_cast<int>(triangle.size())  ; i++ ) {
        // 遍历所有三角形元素的标准边组成的列表
        for (const auto& j : getEdges(triangle[i])){
            hashGrid_edge[ j ].push_back(i) ; // 放进哈希表里
        }
    }
} 
// 3.5 : 哈希搜索
std::vector<int > Mesh::rechercheTriVHash (int n ) const {
    // 创建一个列表用于储存相邻三角形元素的编号（从1开始）
    std::vector<int> neighbors ; 
    if ( n < 1 || n >  static_cast<int>(triangle.size())  ) return neighbors;

    int tri_id = n - 1 ; // 三角形元素的下标
    const auto& currentTri = triangle[ tri_id ] ;
    // 遍历currentTri的标准边组成的列表，
    for ( const auto& i : getEdges(currentTri)) {
        // 查找哈希表：
        auto it = hashGrid_edge.find ( i ) ;
        if ( it != hashGrid_edge.end() ) { // 当it 存在于哈希表中的时候，提取其中的三角形编号放进 neighbors
            for (int tri : it->second) {
                if ( tri != tri_id ) neighbors.push_back ( tri ) ; // 这里添加的是下标
            }
        }
    }
    // 去重：（如果三角形与当前共享两条边，理论上不可能，但在退化网格中可能）
    sort(neighbors.begin(), neighbors.end());
    auto last = unique(neighbors.begin(), neighbors.end());
    neighbors.erase(last, neighbors.end());

    // 下标 改成 编号
    for (auto& idx : neighbors) idx += 1;
    return neighbors;
}


