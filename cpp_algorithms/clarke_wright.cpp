#include "common.h"

// Clarke-Wright savings algorithm for VRP
std::vector<std::vector<int>> clarkeWrightVRP(const std::vector<std::vector<int>>& matrix, int numVehicles) {
    int n = matrix.size();
    
    // Calculate savings for each pair of nodes
    std::vector<std::tuple<int, int, int>> savings; // (savings, i, j)
    for (int i = 1; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            // Savings = dist(0,i) + dist(0,j) - dist(i,j)
            int save = matrix[i-1][0] + matrix[j-1][0] - matrix[i-1][j];
            savings.push_back(std::make_tuple(save, i, j));
        }
    }
    
    // Sort savings in descending order
    std::sort(savings.begin(), savings.end(), 
              [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });
    
    // Initialize routes - one vehicle per node initially
    std::vector<std::vector<int>> routes;
    for (int i = 1; i <= n; i++) {
        routes.push_back({0, i, 0});
    }
    
    // Map to track which route contains each node
    std::vector<int> nodeToRoute(n + 1, -1);
    for (int i = 1; i <= n; i++) {
        nodeToRoute[i] = i - 1; // Node i is in route i-1
    }
    
    // Track start and end nodes of each route (excluding depot)
    std::vector<int> routeStarts(n);
    std::vector<int> routeEnds(n);
    for (int i = 0; i < n; i++) {
        routeStarts[i] = i + 1;
        routeEnds[i] = i + 1;
    }
    
    // Merge routes based on savings
    for (const auto& [saving, i, j] : savings) {
        if (routes.size() <= numVehicles) {
            break; // Stop if we've reached desired number of vehicles
        }
        
        int routeI = nodeToRoute[i];
        int routeJ = nodeToRoute[j];
        
        // Skip if nodes are in the same route
        if (routeI == routeJ) {
            continue;
        }
        
        // Merge only if i is end of its route and j is start of its route
        if (routeEnds[routeI] == i && routeStarts[routeJ] == j) {
            // Remove depot between routes
            auto& routeIRef = routes[routeI];
            auto& routeJRef = routes[routeJ];
            
            routeIRef.pop_back(); // Remove depot at end of route i
            routeIRef.insert(routeIRef.end(), routeJRef.begin() + 1, routeJRef.end()); // Skip depot at start of route j
            
            // Update routeStarts and routeEnds
            routeEnds[routeI] = routeEnds[routeJ];
            
            // Update nodeToRoute for all nodes in route j
            for (size_t k = 1; k < routeJRef.size() - 1; k++) {
                nodeToRoute[routeJRef[k]] = routeI;
            }
            
            // Remove route j
            routes.erase(routes.begin() + routeJ);
            
            // Update routeStarts, routeEnds, and nodeToRoute due to removed route
            for (int k = routeJ; k < routes.size(); k++) {
                routeStarts[k] = routeStarts[k + 1];
                routeEnds[k] = routeEnds[k + 1];
            }
            
            for (int k = 1; k <= n; k++) {
                if (nodeToRoute[k] > routeJ) {
                    nodeToRoute[k]--;
                }
            }
        }
        // Try the reverse merge: j is end of its route and i is start of its route
        else if (routeEnds[routeJ] == j && routeStarts[routeI] == i) {
            auto& routeIRef = routes[routeI];
            auto& routeJRef = routes[routeJ];
            
            routeJRef.pop_back(); // Remove depot at end of route j
            routeJRef.insert(routeJRef.end(), routeIRef.begin() + 1, routeIRef.end()); // Skip depot at start of route i
            
            // Update routeStarts and routeEnds
            routeEnds[routeJ] = routeEnds[routeI];
            
            // Update nodeToRoute for all nodes in route i
            for (size_t k = 1; k < routeIRef.size() - 1; k++) {
                nodeToRoute[routeIRef[k]] = routeJ;
            }
            
            // Remove route i
            routes.erase(routes.begin() + routeI);
            
            // Update arrays due to removed route
            for (int k = routeI; k < routes.size(); k++) {
                routeStarts[k] = routeStarts[k + 1];
                routeEnds[k] = routeEnds[k + 1];
            }
            
            for (int k = 1; k <= n; k++) {
                if (nodeToRoute[k] > routeI) {
                    nodeToRoute[k]--;
                }
            }
        }
    }
    
    // If we have too many routes, merge until we have numVehicles
    while (routes.size() > numVehicles) {
        // Find the shortest route to merge with another
        int shortestRouteIndex = 0;
        int minLength = std::numeric_limits<int>::max();
        
        for (size_t i = 0; i < routes.size(); i++) {
            int routeLength = calculateRouteDistance(routes[i], matrix);
            if (routeLength < minLength) {
                minLength = routeLength;
                shortestRouteIndex = i;
            }
        }
        
        // Find best route to merge with
        int bestMergeIndex = -1;
        int minMergedLength = std::numeric_limits<int>::max();
        
        for (size_t i = 0; i < routes.size(); i++) {
            if (i == shortestRouteIndex) continue;
            
            // Create a temporary merged route
            std::vector<int> mergedRoute = routes[i];
            mergedRoute.pop_back(); // Remove depot at end
            
            // Add shortest route (excluding start depot)
            mergedRoute.insert(mergedRoute.end(), routes[shortestRouteIndex].begin() + 1, routes[shortestRouteIndex].end());
            
            int mergedLength = calculateRouteDistance(mergedRoute, matrix);
            if (mergedLength < minMergedLength) {
                minMergedLength = mergedLength;
                bestMergeIndex = i;
            }
        }
        
        // Merge the routes
        routes[bestMergeIndex].pop_back(); // Remove depot at end
        routes[bestMergeIndex].insert(
            routes[bestMergeIndex].end(), 
            routes[shortestRouteIndex].begin() + 1, 
            routes[shortestRouteIndex].end()
        );
        
        // Remove the shortest route
        routes.erase(routes.begin() + shortestRouteIndex);
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
    auto routes = clarkeWrightVRP(matrix, numVehicles);
    writeRoutes(routes);
    
    return 0;
}