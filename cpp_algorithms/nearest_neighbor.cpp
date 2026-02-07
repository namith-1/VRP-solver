#include "common.h"

// Nearest Neighbor heuristic for VRP
std::vector<std::vector<int>> nearestNeighborVRP(const std::vector<std::vector<int>>& matrix, int numVehicles) {
    int n = matrix.size();
    std::vector<bool> visited(n + 1, false);
    std::vector<std::vector<int>> routes(numVehicles);
    
    // Start all routes from depot (0)
    for (int v = 0; v < numVehicles; v++) {
        routes[v].push_back(0);
    }
    
    // Assign nodes to vehicles round-robin style using nearest neighbor
    int currentVehicle = 0;
    int nodesRemaining = n;
    
    while (nodesRemaining > 0) {
        // Get last node in current vehicle's route
        int lastNode = routes[currentVehicle].back();
        int nearestNode = -1;
        int minDistance = std::numeric_limits<int>::max();
        
        // Find nearest unvisited node
        for (int j = 1; j <= n; j++) {
            if (!visited[j]) {
                int distance;
                if (lastNode == 0) {
                    // Distance from depot to node j
                    distance = matrix[j-1][0];
                } else {
                    // Distance from lastNode to node j
                    distance = matrix[lastNode-1][j];
                }
                
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestNode = j;
                }
            }
        }
        
        // Add nearest node to route
        if (nearestNode != -1) {
            routes[currentVehicle].push_back(nearestNode);
            visited[nearestNode] = true;
            nodesRemaining--;
        }
        
        // Move to next vehicle
        currentVehicle = (currentVehicle + 1) % numVehicles;
    }
    
    // Add depot at the end of each route
    for (auto& route : routes) {
        route.push_back(0);
    }
    
    return routes;
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
    auto routes = nearestNeighborVRP(matrix, numVehicles);
    writeRoutes(routes);
    
    return 0;
}