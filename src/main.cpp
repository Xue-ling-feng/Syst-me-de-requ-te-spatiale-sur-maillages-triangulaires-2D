#include "msh.h"
#include <iostream>
#include <chrono>
#include <random>

// 生成 [0,1) 范围内的随机数
double random01() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

int main()
{
    Mesh m("../square.msh");
    //cout << m << endl;
    m.writeGnuplot("gnu.plt");

     double x = 0.4, y = 0.2 ;

    // =================================================================================
    // test 3.2 :
   auto start1 = std::chrono::steady_clock::now();
    int num_simple = m.rechercheSimple(x,y) ;
    auto end1 = std::chrono::steady_clock::now();
    double time_simple = std::chrono::duration<double>(end1 - start1).count(); // 计算耗时
    std::cout << "point (" << x << "," << y <<") est dans le triangle de numero " << num_simple << " par recherche simple." << std::endl;
    std::cout << "methode simple utilise " << time_simple << " s " << std::endl;

    // =================================================================================
    // test 3.3 ：
    const double STEP = 0.05;
    m.buildHashGrid(STEP); // build hashGrid 

    auto start2 = std::chrono::steady_clock::now();
    int num_hash = m.rechercheHash ( x, y ) ;
    auto end2 = std::chrono::steady_clock::now();
    double time_hash = std::chrono::duration<double>(end2 - start2).count();
    std::cout << "point  (" << x << "," << y <<") est dans le triangle de numero " << num_hash << " par recherche hash. " << std::endl;
    std::cout << "methode hash utilise " << time_hash << " s " <<std::endl;

    // =================================================================================
    // test 3.4 : repeter 1000000 fois
    //  生成10万个随机点（避免缓存效应）
    const int TEST_POINT_NUM = 100000;
    std::vector<std::pair<double, double>> test_points;
    for (int i=0; i<TEST_POINT_NUM; i++) {
        test_points.emplace_back(random01(), random01());
    }

    // 测试：
    auto start3 = std::chrono::high_resolution_clock::now();
    for (const auto& p : test_points) {
        // 重复10次查找同一个随机点，放大耗时差异
        for (int k=0; k<10; k++) {
            m.rechercheHash(p.first, p.second);
        }
    }
    auto end3 = std::chrono::high_resolution_clock::now();
    double time_std = std::chrono::duration<double>(end3 - start3).count();

    std::cout << "step =" << STEP << "nombre de recherche = " << TEST_POINT_NUM*10 << std::endl;
    std::cout << " utilise en total " << time_std << " s" << std::endl;


    return 0;
}