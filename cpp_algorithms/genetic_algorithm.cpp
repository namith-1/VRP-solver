#include "common.h"

// Genetic Algorithm for VRP
std::vector<std::vector<int>> geneticAlgorithmVRP(const std::vector<std::vector<int>>& matrix, int numVehicles) {
    int n = matrix.size();
    
    // Parameters for Genetic Algorithm
    const int POPULATION_SIZE = 50;
    const int MAX_GENERATIONS = 100;
    const double MUTATION_RATE = 0.2;
    const double CROSSOVER_RATE = 0.8;
    
    // Random generator
    std::mt19937 gen(std::time(0));
    std::uniform_real_distribution<> realDist(0.0, 1.0);
    std::uniform_int_distribution<> intDist(1, n);
    
    // Generate a random individual (permutation of nodes 1 to n)
    auto generateIndividual = [&]() {
        std::vector<int> individual;
        for (int i = 1; i <= n; i++) {
            individual.push_back(i);
        }
        std::shuffle(individual.begin(), individual.end(), gen);
        return individual;
    };
    
    // Convert an individual to routes by splitting the permutation into numVehicles parts
    auto individualToRoutes = [&](const std::vector<int>& individual) {
        std::vector<std::vector<int>> routes(numVehicles);
        int nodesPerVehicle = n / numVehicles;
        int extraNodes = n % numVehicles;
        
        int index = 0;
        for (int v = 0; v < numVehicles; v++) {
            routes[v].push_back(0); // Start at depot
            
            int nodesToAdd = nodesPerVehicle + (v < extraNodes ? 1 : 0);
            for (int i = 0; i < nodesToAdd && index < n; i++) {
                routes[v].push_back(individual[index++]);
            }
            
            routes[v].push_back(0); // Return to depot
        }
        
        return routes;
    };
    
    // Calculate fitness (inverse of total distance)
    auto calculateFitness = [&](const std::vector<int>& individual) {
        auto routes = individualToRoutes(individual);
        int totalDistance = calculateTotalDistance(routes, matrix);
        return 1.0 / (totalDistance + 1); // Add 1 to avoid division by zero
    };
    
    // Crossover (Order Crossover - OX)
    auto crossover = [&](const std::vector<int>& parent1, const std::vector<int>& parent2) {
        if (realDist(gen) > CROSSOVER_RATE) {
            return parent1; // No crossover
        }
        
        std::vector<int> child(n, -1);
        
        // Select random subsequence from parent1
        int start = intDist(gen) % n;
        int end = intDist(gen) % n;
        if (start > end) std::swap(start, end);
        
        // Copy subsequence from parent1 to child
        for (int i = start; i <= end; i++) {
            child[i] = parent1[i];
        }
        
        // Fill remaining positions with values from parent2 in order
        int j = 0;
        for (int i = 0; i < n; i++) {
            if (child[i] == -1) {
                // Find next value from parent2 that's not already in child
                while (std::find(child.begin(), child.end(), parent2[j]) != child.end()) {
                    j++;
                }
                child[i] = parent2[j++];
            }
        }
        
        return child;
    };
    
    // Mutation (Swap Mutation)
    auto mutate = [&](std::vector<int>& individual) {
        if (realDist(gen) < MUTATION_RATE) {
            int pos1 = intDist(gen) % n;
            int pos2 = intDist(gen) % n;
            std::swap(individual[pos1], individual[pos2]);
        }
        return individual;
    };
    
    // Initialize population
    std::vector<std::vector<int>> population;
    for (int i = 0; i < POPULATION_SIZE; i++) {
        population.push_back(generateIndividual());
    }
    
    // Main GA loop
    std::vector<int> bestIndividual;
    double bestFitness = 0.0;
    
    for (int generation = 0; generation < MAX_GENERATIONS; generation++) {
        // Calculate fitness for each individual
        std::vector<double> fitness;
        for (const auto& individual : population) {
            double fit = calculateFitness(individual);
            fitness.push_back(fit);
            
            if (fit > bestFitness) {
                bestFitness = fit;
                bestIndividual = individual;
            }
        }
        
        // Create new population
        std::vector<std::vector<int>> newPopulation;
        
        // Elitism: keep the best individual
        newPopulation.push_back(bestIndividual);
        
        // Selection, crossover, and mutation
        std::discrete_distribution<> selection(fitness.begin(), fitness.end());
        
        while (newPopulation.size() < POPULATION_SIZE) {
            int idx1 = selection(gen);
            int idx2 = selection(gen);
            
            auto child = crossover(population[idx1], population[idx2]);
            child = mutate(child);
            
            newPopulation.push_back(child);
        }
        
        population = newPopulation;
    }
    
    // Convert best individual to routes
    return individualToRoutes(bestIndividual);
}

int main(int argc, char** argv) {
    auto matrix = readDistanceMatrix();
    
    // Default to 2 vehicles
    int numVehicles = 2;
    
    // Use command line argument if provided
    if (argc > 1) {
        numVehicles = std::atoi(argv[1]);
    }
    
    // For brute force, check if problem size is manageable
    if (matrix.size() > 10 && std::string(argv[0]).find("brute_force") != std::string::npos) {
        std::cerr << "Warning: Brute force is impractical for " << matrix.size() 
                  << " nodes. Using " << numVehicles << " vehicles and limiting to first 8 nodes." << std::endl;
        
        // Truncate the matrix to avoid excessive computation
        if (matrix.size() > 8) {
            matrix.resize(8);
        }
    }
    
    // Run the appropriate algorithm (already defined in each file)
    auto routes = geneticAlgorithmVRP(matrix, numVehicles);
    writeRoutes(routes);
    
    return 0;
}