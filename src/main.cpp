#include "transform.h"

using namespace std;
using namespace fun;
using namespace tr;

int main(int argc, char* argv[]) {
    const auto print_help=[&]() {
        cerr << "Usage:\t" << argv[0]
             << " -i input [-o output|-s] [-h] [-c coord|-t trasform]\n"
                "Scope:\tinvert the space such that the polar coordinate system"
                " centered in (X,Y) {r, θ} becomes {r' = R^2/r, θ' = θ}\n"
                "Flags:\t-input(string):"
                " the image file to invert (:png)\n"
                "\t-output(string, defaults to \"out-\"input):"
                " the output file (:overwrites if exists)\n"
                "\t-coord: X,Y,R:\n"
                "\t\t-X(int, defaults width/2):"
                " the x coordinate of the center of inversion\n"
                "\t\t-Y(int, defaults height/2):"
                " the y coordinate of the center of inversion\n"
                "\t\t-R(int, defaults min(X,Y)/2):"
                " the radius of the circle of inversion\n"
                "\t-transform: Re{a},Img{a},Re{b},Img{b},Re{c},"
                "Img{c},Re{d},Img{d}:\n"
                "\t\t(X+i*Y) = z becomes:\n"
                "\t\tz*(Re{a}+i*Img{a})+(Re{b}+i*Img{b})\n"
                "\t\t―――――――――――――――――――――――――――――――――――\n"
                "\t\tz*(Re{c}+i*Img{c})+(Re{d}+i*Img{d})\n"
                "\t\twhere (X, Y) is a coordinate relative"
                " to the center of the image"
             << endl;
    };
    string oname, iname;
    array<int, 3> xyr;
    array<float, 8> t;
    bool outopt{false}, showopt{false}, inopt{false}, coordopt{false},
    transopt{false};
    int opt;
    while ((opt = getopt(argc, argv, "hso:i:c:t:")) != -1) {
        switch (opt) {
        case 's':
            showopt = true;
            break;
        case 'o':
            oname = string(optarg);
            outopt = true;
            break;
        case 'i':
            iname = string(optarg);
            inopt = true;
            break;
        case 'c':
            xyr = parse_coord<3, int>(string(optarg));
            coordopt = true;
            break;
        case 't':
            t = parse_coord<8, float>(string(optarg));
            transopt = true;
            break;
        case 'h':
            print_help();
            return 0;
        default:
            print_help();
            return 1;
        }
    }
    if ((outopt && oname.empty()) || !inopt || iname.empty() ||
        (showopt && outopt) || (transopt &&  coordopt) || optind != argc ) {
        print_help();
        return 1;
    }

    sf::Image image;
    image.loadFromFile(iname);

    if(!outopt) {
        oname = iname + "-out.png";
    }
    Transformation tr(std::move(image));
    if (transopt) {
        const TransParams tp{{t[0],t[1]}, {t[2],t[3]}, {t[4],t[5]}, {t[6],t[7]}};
        tr.transform(tp);
    } else if (coordopt) {
        const Coord center(xyr[0],xyr[1]);
        const unsigned radius{absolute(xyr[2])};
        tr.invert({radius}, center);
    } else {
        tr.invert();
    }
    if (showopt) {
        tr.draw().show();
    } else {
        tr.draw().save(oname);
    }
    return 0;
}
