#!/bin/bash
apt-get update && apt-get install -y build-essential
cd cpp_algorithms
g++ -o ../brute_force brute_force.cpp -std=c++17
g++ -o ../nearest_neighbor nearest_neighbor.cpp -std=c++17
g++ -o ../clarke_wright clarke_wright.cpp -std=c++17
g++ -o ../genetic_algorithm genetic_algorithm.cpp -std=c++17
g++ -o ../simulated_annealing simulated_annealing.cpp -std=c++17
cd ..
chmod +x brute_force nearest_neighbor clarke_wright genetic_algorithm simulated_annealing