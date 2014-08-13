#include "utils.h"

using namespace std;

namespace fun {

template<> void getnum<int>(const char* str, int& num) {
    num = atoi(str);
}

template<> void getnum<float>(const char* str, float& num) {
    num = atof(str);
}

template<>  void getnum<long>(const char* str, long& num) {
    num = atol(str);
}
} // namespace fun
