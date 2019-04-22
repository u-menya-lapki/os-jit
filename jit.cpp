#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

typedef int (*func)(int);

unsigned char code[] = {
            0x55,
        	0x48, 0x89, 0xe5,
            0x89, 0x7d, 0xec,
        	0xc7, 0x45, 0xfc, 0x07, 0x00, 0x00, 0x00,
        	0x8b, 0x45, 0xec,
        	0x0f, 0xaf, 0x45, 0xfc,
        	0x5d,
        	0xc3
};

const size_t SIZE = sizeof(code);
const size_t OFFSET = 10;

void free_up(void* ptr)
{
    if (munmap(ptr, SIZE) != 0)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
}

void modify(void* ptr, int mod)
{
    if (mprotect(ptr, SIZE, PROT_READ | PROT_WRITE) != 0)
    {
        perror("mprotect");
        free_up(ptr);
        exit(EXIT_FAILURE);
    }
    ((unsigned char*)ptr)[OFFSET] = mod;
    if (mprotect(ptr, SIZE, PROT_READ | PROT_EXEC) != 0)
    {
        perror("mprotect");
        free_up(ptr);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    std::cout << "The function that I use multiplies 7 by your input number or originally by 42.\n";

    std::cout << "Enter \"run\" to run without changing or\n\"modify\" to change function code in runtime\n";
    std::string input;
    std::cin >> input;
    if (input != "run" && input != "modify")
    {
        std::cout << "invalid input error\n";
        exit(EXIT_FAILURE);
    }
    int mod;
    void* ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memcpy(ptr, code, SIZE);
    if (mprotect(ptr, SIZE, PROT_READ | PROT_EXEC) != 0)
    {
        perror("mprotect");
        free_up(ptr);
        exit(EXIT_FAILURE);
    }

    func myFunc = (func) ptr;
    if (input == "modify")
    {
        std::cout << "Now enter your multiplier\n";
        std::cin >> mod;

        modify(ptr, mod);
    }
    std::cout << myFunc(42) << '\n';

    free_up(ptr);
    return 0;
}
