#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <random>
#include <ctime>

// Read distance matrix from input.txt
std::vector<std::vector<int>> readDistanceMatrix(const std::string& filename = "input.txt") {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening input file: " << filename << std::endl;
        exit(1);
    }

    int n;
    file >> n;

    std::vector<std::vector<int>> matrix;
    matrix.resize(n);
    
    for (int i = 0; i < n; i++) {
        matrix[i].resize(n + 1);
        for (int j = 0; j < n + 1; j++) {
            file >> matrix[i][j];
        }
    }
    
    file.close();
    return matrix;
}

// Write routes to output.txt
void writeRoutes(const std::vector<std::vector<int>>& routes, const std::string& filename = "output.txt") {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening output file: " << filename << std::endl;
        exit(1);
    }
    
    for (const auto& route : routes) {
        for (size_t i = 0; i < route.size(); i++) {
            file << route[i];
            if (i < route.size() - 1) {
                file << " ";
            }
        }
        file << std::endl;
    }
    
    file.close();
}

// Calculate total distance of a route
int calculateRouteDistance(const std::vector<int>& route, const std::vector<std::vector<int>>& matrix) {
    int totalDistance = 0;
    int prev = 0; // Start from depot (0)
    
    for (size_t i = 0; i < route.size(); i++) {
        int current = route[i];
        // If current is depot (0), use special case
        if (current == 0) {
            // If prev is also depot, distance is 0
            if (prev == 0) {
                totalDistance += 0;
            } else {
                // Use the first column of matrix (distances back to depot)
                totalDistance += matrix[prev-1][0];
            }
        } else if (prev == 0) {
            // Distance from depot to current
            totalDistance += matrix[current-1][0];
        } else {
            // Distance between two delivery points
            totalDistance += matrix[prev-1][current];
        }
        prev = current;
    }
    
    // If the route doesn't end at depot, add return to depot
    if (route.back() != 0) {
        totalDistance += matrix[route.back()-1][0];
    }
    
    return totalDistance;
}

// Calculate total distance for multiple routes
int calculateTotalDistance(const std::vector<std::vector<int>>& routes, const std::vector<std::vector<int>>& matrix) {
    int totalDistance = 0;
    for (const auto& route : routes) {
        totalDistance += calculateRouteDistance(route, matrix);
    }
    return totalDistance;
}

#endif