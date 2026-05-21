#include "msh.h"
#include <cassert>
#include <string>
#include <iomanip>



// classe maillage 2d avec liste de noeuds et liste de triangles
Mesh::Mesh(string gmsh2_filename) // Prendre le fichier .msh en entrée
{
    ifstream file = ifstream(gmsh2_filename); // ifstream = outil C++ dédié à la lecture de fichiers texte
                                                                                // Lire le fichier .msh et le stocker dans la variable file
    // Si le fichier ne peut pas être ouvert, lancer une exception :
    if (!file.is_open()) {
        throw runtime_error("file not found");
    }
    string line;
    while (std::getline(file, line)) // Parcourir chaque ligne du fichier
                                                        // getline = lecture ligne par ligne : lire une ligne de texte depuis file et la stocker dans line
    {
        // Analyser la section $Nodes :
        if (line == "$Nodes")
        {
            int n;
            file >> n; // Lecture par nombre
                            // Lire n car n est le nombre de noeuds
            node.resize(n); // Réserver n emplacements pour node
            for (int i = 0; i < n; i++)
            {
                int id;
                double z;
                file >> id; // Lire le numéro du noeud
                file >> node[id - 1][0] >> node[id - 1][1] >> z; 
                // Lire les coordonnées (x,y,z) et les stocker dans le (id-1)ème emplacement de node sous forme de tableau,
                // file >> permet de stocker des nombres consécutifs dans un tableau, il suffit de fournir la dernière variable
                // On peut aussi écrire : std::array<double , 3 > t ; file >> t[0] >> t[1] >> t[2] ; node.push_back(t) ;
            }
        }
        if (line == "$Elements")
        {
            int n;
            file >> n;
            for (int i = 0; i < n; i++)
            {
                int id, type, ntags; 
                file >> id >> type >> ntags; //  Numéro, type, nombre de tags
                for (int j = 0; j < ntags; j++)  // Lire les deux nombres après ntags mais ne pas les utiliser
                // Parce que les deux nombres suivants sont un tag physique et un tag géométrique, inutiles dans ce programme
                {
                    int tag;
                    file >> tag;
                }
                if (type == 2) // Stocker uniquement les éléments triangulaires dans triangle
                {
                    array<int, 3> t;
                    file >> t[0] >> t[1] >> t[2];
                    t[0]--;
                    t[1]--;
                    t[2]--; // Pour pouvoir trouver dans le tableau node, on utilise l'indice du tableau node (d'où la soustraction de 1)
                    triangle.push_back(t); 
                } // sinon, sauter la ligne (on ne connaît pas sa taille)
                else
                {
                    string line;
                    std::getline(file, line); // Sauter directement cette ligne
                }
            }
        }
    }
};

// affichage dans un fichier lisible par gnuplot
void Mesh::writeGnuplot(const string &filename) const
{
    ofstream file(filename); // ofstream = outil C++ dédié à la lecture de fichiers texte

    // file << "set terminal postscript eps enhanced color" << endl; // Exporter l'image au format .eps (image vectorielle), en couleur
    // file << "set output '" << filename << ".eps'" << endl; // Enregistrer le graphique dans un fichier xxx.eps
    //  file << "set xrange [-1:1]" << endl; // Forcer la plage de l'axe X de -1 à 1
    //  file << "set yrange [-1:1]" << endl; // Forcer la plage de l'axe Y de -1 à 1
    //  freeze aspect ratio // Geler le rapport d'aspect

    file << "set size ratio -1" << endl; // Faire en sorte que les unités de longueur des axes X et Y soient identiques, sinon le graphique serait déformé visuellement
                                                                // Le signe - signifie maintenir automatiquement le vrai rapport géométrique
    file << "plot '-' with linespoints" << endl; // Commencer à dessiner le graphique, les données sont lues depuis le texte ci-dessous, dessinées avec des lignes et des points (linespoints)
                                                                             // '-' signifie que les données sont lues depuis le texte ci-dessous


    for (int i = 0; i < triangle.size(); i++)
    {
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; 
        file << node[triangle[i][1]][0] << " " << node[triangle[i][1]][1] << endl; 
        file << node[triangle[i][2]][0] << " " << node[triangle[i][2]][1] << endl; 
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; 
        file << endl; // Ligne vide pour séparer les différents triangles
    }
    file << "e" << endl;  // Indiquer à Gnuplot la fin des données
    file << "pause -1";  // Mettre en pause la fenêtre Gnuplot (pour éviter qu'elle se ferme abruptement)
    file.close();
}

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
// 3.2 ：recherche Simple：Prendre (x,y) en entrée, déterminer quel triangle contient ce point, retourner le numéro du triangle, ou -1 si aucun n'est trouvé
int Mesh::rechercheSimple(double x, double y) const {
    if (x < 0 || x > 1 || y < 0 || y > 1  ){return -1;} // Si le point est hors de la plage, retourner -1
    // Parcourir tous les éléments triangulaires du maillage
    for (int i = 0 ; i < triangle.size() ; i++ ) {
         // Extraire les coordonnées des points A, B et C :
        double x1 = node [triangle[i][0]][0] ;
        double y1 = node [triangle[i][0]][1] ;
        double x2 = node [triangle[i][1]][0] ;
        double y2 = node [triangle[i][1]][1] ;
        double x3 = node [triangle[i][2]][0] ;
        double y3 = node [triangle[i][2]][1] ;
        // Calculer les produits vectoriels
        double cross1 = (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1) ; //  AB × AP
        double cross2 = (x3 - x2) * (y - y2) - (y3 - y2) * (x - x2) ; //  BC × BP
        double cross3 = (x1 - x3) * (y - y3) - (y1 - y3) * (x - x3) ; //  CA × CP
         // Vérifier si le point (x,y) se trouve à l'intérieur, si oui, retourner le numéro de l'élément triangulaire ; sinon, retourner -1
        // Condition : tous les produits vectoriels sont >= 0 ou <= 0, alors P est à l'intérieur du triangle ABC
        if ((cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0)){return i +1;}   
    }
    return -1;
}

// =============================================================================================================
// 3.3 : Prétraitement：build  hashGrid  (placer les éléments triangulaires dans les cases à l'avance)
void Mesh::buildHashGrid (double step){
    this->step = step;
    hashGrid.clear(); // Vider d'abord la table de hachage
    // Parcourir tous les éléments triangulaires
    for (int i = 0 ; i < triangle.size() ; i++){
        double x1 = node [triangle [i] [0] ][0];
        double x2 = node [triangle [i] [1] ][0];
        double x3 = node [triangle [i] [2] ][0];
        double y1 = node [triangle [i] [0] ][1];
        double y2 = node [triangle [i] [1] ][1];
        double y3 = node [triangle [i] [2] ][1];

         // Calculer la plage de coordonnées couverte par l'élément triangulaire :
        int min_i = std::min({x1, x2, x3}) / step;
        int min_j = std::min({y1, y2, y3}) / step;
        int max_i = std::max({x1, x2, x3}) / step;
        int max_j = std::max({y1,y2, y3}) / step;

       // Ajouter le numéro de l'élément triangulaire à la liste correspondante de toutes les cellules couvertes
        for (int k = min_i ; k <= max_i ; k++) {
            for (int j = min_j ; j <= max_j ; j ++ ){
                hashGrid[{k , j}].push_back(i); // Attention : l'indice de l'élément triangulaire dans la liste `triangle` est stocké ici, le véritable numéro = i+1
            }
        }
    }
}

//si un point se trouve à l'intérieur d'un triangle
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

// 3.3 : recherche de Hashmap
 int Mesh::rechercheHash (double x, double y) const {
    if (x < 0 || x > 1 || y < 0 || y > 1  ){return -1;} // hors de la plage, retourner -1
    // calcule coordonnee
    int i = x / step;
    int j = y / step;
    // Rechercher dans la table de hachage : obtenir la liste correspondante des éléments triangulaires
    auto it = hashGrid.find({i, j});
    if ( it == hashGrid.end()) return -1;
    // // Parcourir uniquement les éléments triangulaires présents dans la liste correspondante
    for (int tri_id : it->second) {
        if (isPointInTriangle(tri_id , x , y )) return tri_id + 1;
    }
    return -1;
}

// =============================================================================================================
// si deux triangles partagent un côté
bool Mesh::isShareEdge(int tri_id1, int tri_id2) const {
    int common = 0 ; 
    for (int i : triangle[tri_id1]) {
        for (int j : triangle[tri_id2]) {
            if ( i == j ) common ++;
            if ( common >= 2 ) return true ; // trouver 2 ptr = un cote
        }
    }
    return false;
}

// 3.5 : fonction naive : 
std::vector<std::vector<int>> Mesh::fonctionNaive() const {
    // triangle.size() renvoie un type size_t (entier non signé), tandis que n est de type int (entier signé) : ils ne peuvent pas être comparés directement !
    // Il faut donc ajouter static_cast<int> pour convertir explicitement en int
    int n = static_cast<int>(triangle.size());
    std::vector<std::vector<int>> all_neighbors(n) ;
    for (int i = 0 ; i < n ; i++ ) {
        for (int j = 0 ; j < n ; j ++ ){
            if ( i == j ) continue;
            if (isShareEdge( i , j)) all_neighbors[ i ].push_back( j + 1 ) ;
        }
    }
    return all_neighbors;
}
std::vector<int> Mesh::rechercheTriVNaive (int n ) const {
    std::vector<std::vector<int>> all_neighbors  = fonctionNaive();
    if ( n < 1 || n > static_cast<int>(triangle.size()) ) return { }; 
    return all_neighbors[ n-1 ];
}

// Fonction auxiliaire : Générer les côtés normalisés
std::vector<std::pair<int, int >> Mesh::getEdges (const std::array<int , 3>& tri) const {
    std::vector<std::pair<int, int>> edges;
    edges.emplace_back(std::min(tri[0],tri[1]), std::max(tri[0],tri[1]));
    edges.emplace_back(std::min(tri[1],tri[2]), std::max(tri[1],tri[2]));
    edges.emplace_back(std::min(tri[2],tri[0]), std::max(tri[2],tri[0]));
    return edges;
}

// 3.5：  Prétraitement：build  hashGrid 
void Mesh::buildHashGrid_edge() {
    hashGrid_edge.clear();
    // parcour triangle 
    for (int i = 0 ; i <  static_cast<int>(triangle.size())  ; i++ ) {
        // parcour list des cote 
        for (const auto& j : getEdges(triangle[i])){
            hashGrid_edge[ j ].push_back(i) ; 
        }
    }
} 
// 3.5 : recherche Hashmap
std::vector<int > Mesh::rechercheTriVHash (int n ) const {
    std::vector<int> neighbors ; 
    if ( n < 1 || n >  static_cast<int>(triangle.size())  ) return neighbors;

    int tri_id = n - 1 ; //  indice de tri
    const auto& currentTri = triangle[ tri_id ] ;
    // // Parcour la liste des côtés normalisés de `currentTri`
    for ( const auto& i : getEdges(currentTri)) {
        //  recherche dans hashmap:
        auto it = hashGrid_edge.find ( i ) ;
        if ( it != hashGrid_edge.end() ) { // Lorsque l'itérateur `it` existe dans la table de hachage, extraire les numéros de triangles et les ajouter à `neighbors`
            for (int tri : it->second) {
                if ( tri != tri_id ) neighbors.push_back ( tri ) ; //  ici tri  = tri_id 
            }
        }
    }

    // Supprimer les doublons : (Théoriquement impossible si un triangle partage deux côtés avec le courant, mais cela peut arriver dans un maillage dégradé)
    sort(neighbors.begin(), neighbors.end());
    auto last = unique(neighbors.begin(), neighbors.end());
    neighbors.erase(last, neighbors.end());

    //  tri_id ---> numero de tri
    for (auto& idx : neighbors) idx += 1;
    return neighbors;
}

// =============================================================================================================

// 3.6： Marquer un point fixe en vert et le triangle qui le contient en rouge
void Mesh::vsPoint (const string &filename, double x , double y , int tri_id ) const {
    ofstream file (filename) ;
    if ( ! file.is_open()) throw runtime_error("file not found"); 
    // Configuration des paramètres de gnuplot :
    file << "set size ratio -1" << endl;
    file << "set title 'Point ( " << x << " , " << y << " ) is in triangle " << tri_id + 1 << " ' " << endl;
    file << "set xlabel 'x'" << endl;
    file << "set ylabel 'y' " << endl;

    file << "plot '-' w l lc 'black' lw 1 title 'Mesh',\\" << endl;
    file << "     '-' w l lc 'red' lw 3 title 'Triangle " << tri_id + 1 << "',\\" << endl;
    file << "     '-' w p pt 7 ps 2 lc 'green' title 'Point'" << endl;

    // tous les triangles :
    for (int i = 0; i <  static_cast<int>(triangle.size()); i++)
    {
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl;
        file << node[triangle[i][1]][0] << " " << node[triangle[i][1]][1] << endl; 
        file << node[triangle[i][2]][0] << " " << node[triangle[i][2]][1] << endl; 
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; 
        file << endl; 
    }
    file << "e" << endl;
    // Mettre en surbrillance le triangle :
        if ( tri_id >= 0 && tri_id < static_cast<int>(triangle.size()) )
    {
        file << node[triangle[tri_id][0]][0] << " " << node[triangle[tri_id][0]][1] << endl; 
        file << node[triangle[tri_id][1]][0] << " " << node[triangle[tri_id][1]][1] << endl;  
        file << node[triangle[tri_id][2]][0] << " " << node[triangle[tri_id][2]][1] << endl;  
        file << node[triangle[tri_id][0]][0] << " " << node[triangle[tri_id][0]][1] << endl; 
    }
    file << "e" <<endl;

    // Mettre en surbrillance le point :
    file << x << " " << y << endl;
    file << "e" << endl;

    file << "pause -1" << endl;   
    file.close();
}

// 3.6 : Marquer le triangle spécifié en rouge et ses triangles voisins en bleu
void Mesh::vsNeighbors (const string &filename, int n , const vector<int> &neighbors ) const {
    ofstream file (filename) ;
    if ( ! file.is_open()) throw runtime_error("file not found"); 
    // configuration des paramètres de gnuplot :
    file << "set size ratio -1" << endl;
    file << "set title 'Triangle " << n << " and its neighbors'" << endl;
    file << "set key off" << endl;

    file << "plot '-' w l lc 'black' lw 1 title 'Other triangles',\\" << endl;
    file << "     '-' w l lc 'red' lw 3 title 'Target triangle',\\" << endl;
    file << "     '-' w l lc 'blue' lw 2 title 'Neighbors'" << endl;
    // tous les triangles :
    for (int i = 0; i <  static_cast<int>(triangle.size()); i++)
    {
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl;
        file << node[triangle[i][1]][0] << " " << node[triangle[i][1]][1] << endl; 
        file << node[triangle[i][2]][0] << " " << node[triangle[i][2]][1] << endl; 
        file << node[triangle[i][0]][0] << " " << node[triangle[i][0]][1] << endl; 
        file << endl; 
    }
    file << "e" << endl;
    // triangle cible :
    if ( n >= 1 && n <= static_cast<int>(triangle.size())){
    file << node[triangle[n-1][0]][0] << " " << node[triangle[n-1][0]][1] << endl;
    file << node[triangle[n-1][1]][0] << " " << node[triangle[n-1][1]][1] << endl; 
    file << node[triangle[n-1][2]][0] << " " << node[triangle[n-1][2]][1] << endl; 
    file << node[triangle[n-1][0]][0] << " " << node[triangle[n-1][0]][1] << endl; 
    }
    file << "e" << endl;
    // tirangle voisin
    for (int i : neighbors) 
    {
        int neighbor_idx = i - 1;
        if(neighbor_idx <0 || neighbor_idx >= static_cast<int>(triangle.size())) continue;
        file << node[triangle[neighbor_idx][0]][0] << " " << node[triangle[neighbor_idx][0]][1] << endl;
        file << node[triangle[neighbor_idx][1]][0] << " " << node[triangle[neighbor_idx][1]][1] << endl; 
        file << node[triangle[neighbor_idx][2]][0] << " " << node[triangle[neighbor_idx][2]][1] << endl; 
        file << node[triangle[neighbor_idx][0]][0] << " " << node[triangle[neighbor_idx][0]][1] << endl; 
        file << endl;
    }
    file << "e" << endl;
    file <<"pause -1" <<endl;
    file.close() ;
} 


