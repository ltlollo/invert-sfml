#include "invert.h"

#include <unistd.h>
#include <array>

using namespace std;
using namespace inv;

array<int, 3> parse_coord(const string& opt) {
    array<int, 3> res;
    char sepa{','};
    auto it = begin(opt);
    for (unsigned i = 0; i < res.size()-1; ++i) {
        auto it_ = find(it, end(opt), sepa);
        if (it_ == end(opt)) {
            throw runtime_error("not enough args");
        }
        auto str = string(it, it_);
        if (str.empty()) {
            throw runtime_error("empty arg "+to_string(i));
        }
        res[i] = atoi(str.c_str());
        it = it_+1;
    }
    if (find(it, end(opt), sepa) != end(opt)) {
        throw runtime_error("too much args");
    }
    if (it == end(opt)) {
        throw runtime_error("not enough args");
    }
    auto pos = res.size()-1;
    auto str = string(it, end(opt));
    if (str.empty()) {
        throw runtime_error("empty arg "+to_string(pos));
    }
    res[pos] = atoi(str.c_str());
    return res;
}

int main(int argc, char* argv[]) {
    const auto print_help=[&]() {
        cerr << "Usage:\t" << argv[0]
             << " -i input [-o output] [-s] [-h] [-c coord]\n"
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
                " the radius of the circle of inversion"
             << endl;
    };

    string oname, iname;
    array<int, 3> xyr;
    bool outopt{false}, showopt{false}, inopt{false}, coordopt{false};
    int opt;
    while ((opt = getopt(argc, argv, "hso:i:c:")) != -1) {
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
            xyr = parse_coord(string(optarg));
            coordopt = true;
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
        (showopt && outopt) || optind != argc ) {
        print_help();
        return 1;
    }
    if(!outopt) {
        oname = iname + "-out.png";
    }
    if (coordopt) {
        const Coord inv_center(xyr[0],xyr[1]);
        Inverter(iname, oname, inv_center, xyr[2], showopt).run();
    } else {
        Inverter(iname, oname, showopt).run();
    }
    return 0;
}
