#ifndef GRID_H
#define GRID_H

class Grid2d
{
private:
int nx;  // Nombre de points par direction
double dx;  // Pas d'espace
double dt;  // Pas de temps
double *data;  // Stockage des valeurs
int*ref_count;  // Compteur de références

public:
// Constructeur principal
Grid2d(double L, int nx);

// Constructeur par copie
Grid2d(const Grid2d &other);

// Destructeur
~Grid2d();

// accesseur dt
double get_dt() const { return dt; }

// Accesseur opérateur ()
double &operator()(int i, int j);
const double &operator()(int i, int j) const;

// condition initiale (1 sur un cercle de rayon r centré en (x0, y0))
void init(double x0, double y0, double r);

// tracé: génération d'un fichier de données
// puis tracer par gnuplot
void plot() const;

// avancer d'un pas de temps
// schéma chaleur explicite à partir de unow (constant)
void step(const Grid2d &unow);

// fonction amie pour échanger deux grilles
friend void swap(Grid2d &a, Grid2d &b);
};
#endif
