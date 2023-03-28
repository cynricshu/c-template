//
// Created by shulingjie on 2023/3/27.
//
#include <iostream>

class A {
public:
    A() {
        std::cout << "[C] constructor fired." << std::endl;
    }

    A(const A &a) {
        std::cout << "[C] copying constructor fired." << std::endl;
    }

//    A(A &&a) {
//        std::cout << "[C] moving constructor fired." << std::endl;
//    }

    ~A() {
        std::cout << "[C] destructor fired." << std::endl;
    }
};

A getTempA() {
    return A();
}

int main(int argc, char **argv) {
    auto x = getTempA();

    return 0;
}
