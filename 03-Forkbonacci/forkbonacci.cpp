#include <iostream>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int64_t factorial(int64_t n) {
    int64_t result = 1;
    for (; n > 1; --n) {
        result *= n;
    }
    return result;
}

int64_t fibonacci(int64_t n) {
    int64_t a = 0;
    int64_t b = 1;
    
    while (n--) {
        int64_t copy = a;
        a = b;
        b = copy + b;
    }
    
    return a;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    int64_t n;
    try {
        n = std::stoll(argv[1]);
        if (n < 0) {
            std::cout << "Error: " << n << " is not a positive number" << std::endl;
            return 1;
        }
    } catch (const std::invalid_argument& e) { 
        std::cout << "Error: " << argv[1] << " is not a number" << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cout << "Error: " << argv[1] << " is out of range of valid numbers" << std::endl;
        return 1;
    }
    
    const pid_t pid = getpid();
    if (fork() < 0) {
        std::cout << "Error: fork() failed: " << strerror(errno) << std::endl;
        return 1;
    }

    if (getppid() == pid) {
        // In the child process - compute factorial.
        std::cout << "[Child]: " << factorial(n) << std::endl;
    } else {
        // In the parent process - compute fibonacci.
        std::cout << "[Parent]: " << fibonacci(n) << std::endl;
    }

    return 0;
}