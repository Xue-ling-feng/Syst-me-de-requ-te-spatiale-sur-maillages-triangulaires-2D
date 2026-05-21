#include "msh.h"
#include <iostream>
#include <chrono>
#include <random>
#include <string>

// Générer un nombre aléatoire dans l'intervalle [0,1)
static double random01() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

// Fonction pour afficher un vecteur
static void printVector (const std::vector<int>& vec, const std::string& name,const int& num) {
    std::cout << name << ": ";
    if (vec.empty()) {
        std::cout << "il n'a pas de triangle voisin ." << std::endl ;
    } else {
        for (int i : vec) {
            std::cout << i << " ";
        }
        std::cout << "sont des triangle voisin de " << num << std::endl;
    }
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
    double time_simple = std::chrono::duration<double>(end1 - start1).count(); 
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
    // Générer 100 000 points aléatoires pour éviter l'effet de cache
    const int TEST_POINT_NUM = 100000;
    std::vector<std::pair<double, double>> test_points;
    for (int i=0; i<TEST_POINT_NUM; i++) {
        test_points.emplace_back(random01(), random01());
    }

    auto start3 = std::chrono::high_resolution_clock::now();
    for (const auto& p : test_points) {
        // Répéter la recherche 10 fois sur le même point aléatoire pour amplifier la différence de temps d'exécution
        for (int k=0; k<10; k++) {
            m.rechercheHash(p.first, p.second);
        }
    }
    auto end3 = std::chrono::high_resolution_clock::now();
    double time_std = std::chrono::duration<double>(end3 - start3).count();

    std::cout << "step =" << STEP << "nombre de recherche = " << TEST_POINT_NUM*10 << std::endl;
    std::cout << " utilise en total " << time_std << " s" << std::endl;

    // ===================================================================================
    // test 3.5 : 
    int NUM_TRI = 4; // numero de tri
    // naive : 
   auto start4 = std::chrono::steady_clock::now();
    std::vector<int> neighbor_naive = m.rechercheTriVNaive( NUM_TRI );
    auto end4 = std::chrono::steady_clock::now();
    double time_naive = std::chrono::duration<double>(end4 - start4).count(); 
    string name1 = "par fonction naive";
    printVector(neighbor_naive, name1 , NUM_TRI);
    std::cout << name1 << "il utilise " << time_naive << " s " << std::endl;

    //hash :
    m.buildHashGrid_edge(); // build hashGrid_edge
    auto start5 = std::chrono::steady_clock::now();
    std::vector<int> neighbor_hash = m.rechercheTriVHash( NUM_TRI );
    auto end5 = std::chrono::steady_clock::now();
    double time_hashEdge = std::chrono::duration<double>(end5 - start5).count(); 
    string name2 = "par methode hashmap";
    printVector(neighbor_hash, name2, NUM_TRI) ;
    std::cout << name2 << "il utilise " << time_hashEdge << " s " << std::endl;

    // ===================================================================================
    // test 3.6 : 
    // visualisation de recherche Simple  :
    m.vsPoint( "simple_point.plt", x, y , num_simple -1 ) ;

    // visualisation de recherche Hash  :
    m.vsPoint ("hash_point.plt" , x, y, num_hash -1 ); 

    // visualisation de recherche triangle voisin par fonction naive :
    m.vsNeighbors("neighbors_naive.plt" , NUM_TRI, neighbor_naive) ; 

    //  visualisation de recherche triangle voisin par hashmap :
    m.vsNeighbors( "neighbors_hash.plt" , NUM_TRI, neighbor_hash) ;

    return 0;
}