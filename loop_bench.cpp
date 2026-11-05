#include <iostream>

int main() {
    int count = 0;

    for (int i = 0; i < 1000000; i++) {
        count++;
    }

    std::cout << count << '\n';

    return 0;
}