#include "common.h"
#include<bits/stdc++.h>

// Brute force implementation for VRP
// Only practical for very small problems (≤10 nodes)
std::vector<std::vector<int>> bruteForceVRP(const std::vector<std::vector<int>>& matrix, int numVehicles) {
    int n = matrix.size();
    
    // Create nodes 1 to n (excluding depot 0)
    std::vector<int> nodes;
    for (int i = 1; i <= n; i++) {
        nodes.push_back(i);
    }
    
    // Best solution found so far
    std::vector<std::vector<int>> bestSolution;
    int bestDistance = std::numeric_limits<int>::max();
    
    // Function to partition nodes into numVehicles groups
    std::function<void(std::vector<std::vector<int>>&, std::vector<int>&, int)> partition;
    partition = [&](std::vector<std::vector<int>>& current, std::vector<int>& remaining, int vehicleIndex) {
        // Base case: all nodes assigned
        if (remaining.empty()) {
            // For each route, try all permutations
            std::vector<std::vector<int>> allRoutes = current;
            for (auto& route : allRoutes) {
                // Add depot at the beginning of each route
                route.insert(route.begin(), 0);
                // Add depot at the end of each route
                route.push_back(0);
            }
            
            int distance = calculateTotalDistance(allRoutes, matrix);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestSolution = allRoutes;
            }
            return;
        }
        
        // Recursively assign nodes to vehicles
        for (size_t i = 0; i < remaining.size(); i++) {
            int node = remaining[i];
            remaining.erase(remaining.begin() + i);
            
            // Try assigning to each vehicle
            for (int v = 0; v < std::min(vehicleIndex + 1, numVehicles); v++) {
                // Ensure we have enough routes
                while (current.size() <= v) {
                    current.push_back({});
                }
                
                current[v].push_back(node);
                partition(current, remaining, (v == vehicleIndex) ? vehicleIndex : vehicleIndex + 1);
                current[v].pop_back();
                
                // If this route is empty after popping, remove it
                if (current[v].empty() && v == current.size() - 1) {
                    current.pop_back();
                }
            }
            
            // Restore for backtracking
            remaining.insert(remaining.begin() + i, node);
        }
    };
    
    std::vector<std::vector<int>> current;
    partition(current, nodes, 0);
    
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
    auto routes = bruteForceVRP(matrix, numVehicles);
    writeRoutes(routes);
    
    return 0;
}