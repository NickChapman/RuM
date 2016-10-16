#include <iostream>

int main() {
    char* input = new char[1000];
    std::cin.getline(input, 1000);
    char* inputPosition = input;
    while (*inputPosition != NULL) {
        std::cout << *inputPosition;
        ++inputPosition;
    }
    std::cout << std::endl;
    return 0;
}