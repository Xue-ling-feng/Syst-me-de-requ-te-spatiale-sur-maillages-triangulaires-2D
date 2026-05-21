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

// Fonction de hachage pour pair<int, int>
// struct PairHash {
//     size_t operator()(const pair<int , int >& p) const {
//         auto h1 = hash<int >{}(p.first);
//         auto h2 = hash<int >{}(p.second);
//         return h1 ^ ( h2 << 1) ;
//     }
// };

// Optimisation PairHash 
struct PairHash {
    size_t operator()(const pair<int , int >& p) const {
        // Une combinaison plus efficace : éviter les collisions du XOR simple
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
                                                                // par ex:  node[0] = {0.0, 0.0} signifie que les coordonnées du premier noeud sont (0.0, 0.0)
    vector<array<int, 3>> triangle; // liste des triangles (id des 3 noeuds), 
                                                                // par ex:  triangle[0] = {0, 1, 2} signifie que le premier triangle est formé par les noeuds  0, 1, et 2
    // unordered_map<pair<int, int >, vector<int>, PairHash> hashGrid ; // hashmap：key = coordonnées， value = tableau des numéros des éléments triangulaires
    double step; // Pas fixe

    phmap::flat_hash_map<pair<int, int>, vector<int>, PairHash> hashGrid; // dans parallel_hashmap , hashGrid nouvelle

    phmap::flat_hash_map<pair<int, int> , vector<int > , PairHash> hashGrid_edge; // dans parallel_hashmap , hashGrid pour trouver les triangles voisins

    bool isPointInTriangle(int tri_id, double x, double y) const; // Fonction auxiliaire : Vérifier si un point se trouve à l'intérieur d'un triangle
    bool isShareEdge(int tri_id1, int tri_id2) const ;  // Fonction auxiliaire : Vérifier si deux triangles partagent un côté
    vector<pair<int, int >> getEdges (const array<int , 3>& tri) const ; // Fonction auxiliaire : Générer les côtés normalisés

public:
    Mesh(string gmsh2_filename); //  Constructeur 
    friend ostream &operator<<(ostream &os, const Mesh &m); // Surcharge
    // affichage dans un fichier lisible par gnuplot
    void writeGnuplot(const string &filename) const; // Écrire les données du maillage dans un fichier script gnuplot

    int rechercheSimple(double x, double y) const ; // 3.2 ：recherche Simple：Prendre (x,y) en entrée, déterminer quel triangle contient ce point, retourner le numéro du triangle, ou -1 si aucun n'est trouvé

    void buildHashGrid (double step = 0.1) ; // 3.3 : Prétraitement：build  hashGrid  (placer les éléments triangulaires dans les cases à l'avance)
    int rechercheHash (double x, double y) const ; // 3.3 : recherche hashmap

    std::vector<std::vector<int>> fonctionNaive() const ;
    vector<int> rechercheTriVNaive (int n ) const ; // 3.5 : naive : Prendre le numéro n de l'élément triangulaire spécifié (l'indice correspondant tri_id = n-1), retourner la liste des éléments triangulaires voisins de cet élément
    void buildHashGrid_edge() ; // 3.5： Prétraitement：build  hashGrid 
    vector<int > rechercheTriVHash (int n ) const ; // 3.5 : recherche hashmap

    void vsPoint (const string &filename, double x , double y , int tri_id ) const ; //  3.6： Marquer un point fixe en vert et le triangle qui le contient en rouge
    void vsNeighbors (const string &filename, int n , const vector<int> &neighbors ) const ; // 3.6 : Marquer le triangle spécifié en rouge et ses triangles voisins en bleu

};

#endif
