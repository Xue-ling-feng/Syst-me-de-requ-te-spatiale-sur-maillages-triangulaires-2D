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
