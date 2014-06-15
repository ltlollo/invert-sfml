#include <iostream> // std::cout, std::endl
#include "invert.h" // inv::Inverter

using namespace std;
using namespace inv;

int main(int argc, char* argv[])
{
    const auto print_help=[&](){
        cerr << "Usage: " << argv[0] << " IN [[OUT] X Y R]\n"
             << "Scope: invert the space such that the polar coordinate system"
                "\n\tcentered in (X,Y) {r, θ} becomes {r' = R^2/r, θ' = θ}\n"
             << " IN(string):"
                " the image file to invert (:png)\n"
             << " OUT(string, defaults \"out-\"in):"
                " the output file (:overwrites)\n"
             << " X(int, defaults width/2):"
                " the x coordinate of the center of inversion\n"
             << " Y(int, defaults height/2):"
                " the y coordinate of the center of inversion\n"
             << " R(int, defaults min(X,Y)/2):"
                " the radius of the circle of inversion"
             << endl;
    };
    switch (argc)
    {
    case 6:
    {
        const Coord inv_center(atoi(argv[3]), atoi(argv[4]));
        const int inv_radius{atoi(argv[5])};
        Inverter inv(argv[1], argv[2], inv_center, inv_radius);
        inv();
    }
        break;
    case 5:
    {
        const Coord inv_center(atoi(argv[2]), atoi(argv[3]));
        const int inv_radius{atoi(argv[4])};
        Inverter inv(argv[1], inv_center, inv_radius);
        inv();
    }
        break;
    case 2:
    {
        Inverter inv(argv[1]);
        inv();
    }
        break;
    default:
        print_help();
        return 1;
    }
    return 0;
}
