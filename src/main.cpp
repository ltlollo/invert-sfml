#include "transform.h"
#include "conf.h"

using namespace std;
using namespace fun;
using namespace tr;
using namespace sf;

int main(int argc, char* argv[]) {
    const auto print_help=[]() {
        printf("%s\n", help);
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
        (showopt && outopt) || (transopt && coordopt) || optind != argc ) {
        print_help();
        return 1;
    }
    Image image;
    image.loadFromFile(iname);
    if(!outopt) {
        oname = relative_filepath(iname) + "-out.png";
    }
    Transformation tr(move(image));
    if (transopt) {
        const TransParams tp{{t[0],t[1]}, {t[2],t[3]}, {t[4],t[5]}, {t[6],t[7]}};
        tr.transform(tp);
    } else if (coordopt) {
        const Coord center(xyr[0],xyr[1]);
        const InvParams radius{absolute(xyr[2])};
        tr.invert(radius, center);
    } else {
        tr.invert();
    }
    if (showopt) {
        tr.draw().show(appname);
    } else {
        tr.draw().save(oname);
    }
    return 0;
}
