#include "grid.h"
#include <iostream>
int main()
{
// Parametres physiques
const double L = 1.0; // Longueur du domaine
const int nx = 50;     // Nombre de points

// Initialisation des grilles
Grid2d unow(L, nx);
Grid2d unext(L, nx);

unow.init(0.5, 0.5, 0.1);
unext.init(0.5, 0.5, 0.1);

unow.plot();

// Boucle en temps
double t = 0;
double tmax = 0.01;

while (t < tmax)
{
t += unow.get_dt();
unext.step(unow);
swap(unow, unext);
}
unow.plot();

return 0;
}
