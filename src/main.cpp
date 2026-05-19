#include <msh.h>

int main()
{
    Mesh m("../square.msh");
    //cout << m << endl;
    m.writeGnuplot("gnu.plt");

    // test 3.2 :
    double x = 0.4, y = 0.2 ;
    m.isInTriangle(x,y) ;
    std::cout << "point (" << x << "," << y <<") est dans le triangle de numero " << m.isInTriangle(x,y) << std::endl;
    return 0;
}