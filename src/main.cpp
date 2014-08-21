#include "transform.h"
#include "conf.h"

using namespace std;
using namespace fun;
using namespace tr;
using namespace sf;

const auto print_help = []() {
    printf("%s\n", help);
};

int main(int argc, char* argv[]) {
    string oname, iname;
    array<int, 3> xyr;
    array<float, 8> t;
    bool outopt{false}, showopt{false}, inopt{false}, invopt{false},
    cmplxopt{false};
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
            invopt = true;
            break;
        case 't':
            t = parse_coord<8, float>(string(optarg));
            cmplxopt = true;
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
        (showopt && outopt) || (cmplxopt && invopt) || optind != argc ) {
        print_help();
        return 1;
    }
    if(!outopt) {
        oname = filename_from_path(iname) + "-out.png";
    }
    Transformation tr(iname);
    if (cmplxopt) {
        tr.transform(TransParams().fromArray(t));
    } else if (invopt) {
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
