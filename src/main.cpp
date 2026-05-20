#include "msh.h"
#include <iostream>
#include <chrono>

int main()
{
    Mesh m("../square.msh");
    //cout << m << endl;
    m.writeGnuplot("gnu.plt");

     double x = 0.4, y = 0.2 ;

    // test 3.2 :
   auto start1 = chrono::steady_clock::now();
    int num_simple = m.rechercheSimple(x,y) ;
    auto end1 = chrono::steady_clock::now();
    double time_simple = chrono::duration<double>(end1 - start1).count(); // 计算耗时
    std::cout << "point (" << x << "," << y <<") est dans le triangle de numero " << num_simple << " par recherche simple." << std::endl;
    std::cout << "methode simple utilise " << time_simple << " s " << std::endl;

    // test 3.3 ：
    m.buildHashGrid(0.1);
    auto start2 = chrono::steady_clock::now();
    int num_hash = m.rechercheHash ( x, y ) ;
    auto end2 = chrono::steady_clock::now();
    double time_hash = chrono::duration<double>(end2 - start2).count();
    std::cout << "point  (" << x << "," << y <<") est dans le triangle de numero " << num_hash << " par recherche hash. " << std::endl;
    std::cout << "methode hash utilise " << time_hash << " s " <<std::endl;

    return 0;
}