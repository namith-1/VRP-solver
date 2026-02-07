#include "common.h"
#include<bits/stdc++.h>

// Simulated Annealing for VRP
std::vector<std::vector<int>> simulatedAnnealingVRP(const std::vector<std::vector<int>>& matrix, int numVehicles) {
    int n = matrix.size();
    
    // Parameters for Simulated Annealing
    const double INITIAL_TEMP = 1000.0;
    const double COOLING_RATE = 0.99;
    const double MIN_TEMP = 0.01;
    const int ITERATIONS_PER_TEMP = 100;
    
    // Random generator
    std::mt19937 gen(std::time(0));
    std::uniform_real_distribution<> realDist(0.0, 1.0);
    std::uniform_int_distribution<> intDist(0, n - 1);
    
    // Initialize solution (random assignment of nodes to vehicles)
    std::vector<std::vector<int>> currentSolution(numVehicles);
    std::vector<int> nodes;
    for (int i = 1; i < n; i++) {
        nodes.push_back(i);
    }
    std::shuffle(nodes.begin(), nodes.end(), gen);
    
    // Distribute nodes to vehicles
    int nodesPerVehicle = nodes.size() / numVehicles;
    int extraNodes = nodes.size() % numVehicles;
    
    int index = 0;
    for (int v = 0; v < numVehicles; v++) {
        currentSolution[v].push_back(0); // Start at depot
        
        int nodesToAdd = nodesPerVehicle + (v < extraNodes ? 1 : 0);
        for (int i = 0; i < nodesToAdd && index < nodes.size(); i++) {
            currentSolution[v].push_back(nodes[index++]);
        }
        
        currentSolution[v].push_back(0); // Return to depot
    }
    
    // Get total distance of a solution
    auto getTotalDistance = [&](const std::vector<std::vector<int>>& solution) {
        return calculateTotalDistance(solution, matrix);
    };
    
    // Use std::function to enable recursion in lambda
    std::function<std::vector<std::vector<int>>(const std::vector<std::vector<int>>&)> getNeighbor;
    
    getNeighbor = [&](const std::vector<std::vector<int>>& solution) {
        std::vector<std::vector<int>> neighbor = solution;
        
        // Try a limited number of times to generate a valid neighbor
        for (int attempts = 0; attempts < 10; attempts++) {
            // Randomly select type of move
            int moveType = intDist(gen) % 3;
            
            if (moveType == 0) {
                // Swap two nodes within the same route
                int routeIdx = intDist(gen) % numVehicles;
                if (neighbor[routeIdx].size() <= 3) {
                    // Route too short for swap, try again
                    continue;
                }
                
                // Exclude depot nodes (0th and last elements)
                int pos1 = 1 + (intDist(gen) % (neighbor[routeIdx].size() - 2));
                int pos2 = 1 + (intDist(gen) % (neighbor[routeIdx].size() - 2));
                
                std::swap(neighbor[routeIdx][pos1], neighbor[routeIdx][pos2]);
                return neighbor;
            }
            else if (moveType == 1) {
                // Move a node from one route to another
                int srcRouteIdx = intDist(gen) % numVehicles;
                int destRouteIdx = intDist(gen) % numVehicles;
                
                if (srcRouteIdx == destRouteIdx || neighbor[srcRouteIdx].size() <= 3) {
                    // Cannot move from route with only one node or same route
                    continue;
                }
                
                // Select a node to move (exclude depot nodes)
                int srcPos = 1 + (intDist(gen) % (neighbor[srcRouteIdx].size() - 2));
                int node = neighbor[srcRouteIdx][srcPos];
                
                // Remove node from source route
                neighbor[srcRouteIdx].erase(neighbor[srcRouteIdx].begin() + srcPos);
                
                // Insert node into destination route (before the last depot)
                neighbor[destRouteIdx].insert(neighbor[destRouteIdx].end() - 1, node);
                return neighbor;
            }
            else {
                // 2-opt: Reverse a segment within a route
                int routeIdx = intDist(gen) % numVehicles;
                if (neighbor[routeIdx].size() <= 4) {
                    // Route too short for 2-opt
                    continue;
                }
                
                // Exclude depot nodes
                int pos1 = 1 + (intDist(gen) % (neighbor[routeIdx].size() - 2));
                int pos2 = 1 + (intDist(gen) % (neighbor[routeIdx].size() - 2));
                
                if (pos1 > pos2) std::swap(pos1, pos2);
                
                // Reverse the segment
                std::reverse(neighbor[routeIdx].begin() + pos1, neighbor[routeIdx].begin() + pos2 + 1);
                return neighbor;
            }
        }
        
        // If we couldn't find a valid move after several attempts, just return the original
        return solution;
    };
    
    // Simulated Annealing main loop
    auto bestSolution = currentSolution;
    int currentDistance = getTotalDistance(currentSolution);
    int bestDistance = currentDistance;
    
    double temp = INITIAL_TEMP;
    
    while (temp > MIN_TEMP) {
        for (int i = 0; i < ITERATIONS_PER_TEMP; i++) {
            // Generate a neighbor solution
            auto neighbor = getNeighbor(currentSolution);
            int neighborDistance = getTotalDistance(neighbor);
            
            // Decide if we should accept the neighbor
            if (neighborDistance < currentDistance) {
                // Accept better solution
                currentSolution = neighbor;
                currentDistance = neighborDistance;
                
                if (currentDistance < bestDistance) {
                    bestSolution = currentSolution;
                    bestDistance = currentDistance;
                }
            } else {
                // Accept worse solution with some probability
                double p = exp((currentDistance - neighborDistance) / temp);
                if (realDist(gen) < p) {
                    currentSolution = neighbor;
                    currentDistance = neighborDistance;
                }
            }
        }
        
        // Cool down
        temp *= COOLING_RATE;
    }
    
    return bestSolution;
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
    auto routes = simulatedAnnealingVRP(matrix, numVehicles);
    writeRoutes(routes);
    
    return 0;
}