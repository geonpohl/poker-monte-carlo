#include "poker/simulator.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    const poker::MonteCarloSimulator simulator{};

    std::cout << "poker-monte-carlo scaffold\n";
    std::cout << simulator.status() << '\n';

    return EXIT_SUCCESS;
}
