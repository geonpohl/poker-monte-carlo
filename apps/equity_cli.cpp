#include "poker/simulator.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    // Create one simulator object from the library code.
    const poker::MonteCarloSimulator simulator{};

    // This executable is intentionally small.
    // Its job is to call into the library and print results for the user.
    std::cout << "poker-monte-carlo scaffold\n";
    std::cout << simulator.status() << '\n';

    return EXIT_SUCCESS;
}
