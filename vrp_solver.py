import subprocess
import os

def save_matrix(matrix, filename="input.txt"):
    with open(filename, "w") as f:
        f.write(str(len(matrix)) + "\n")
        for row in matrix:
            f.write(" ".join(map(str, row)) + "\n")

def load_output(filename="output.txt"):
    routes = []
    with open(filename) as f:
        for line in f:
            route = list(map(int, line.strip().split()))
            routes.append(route)
    return routes

def solve_vrp(matrix, algo_name):
    algo_execs = {
        "Brute Force": "./brute_force",
        "Nearest Neighbor": "./nearest_neighbor",
        "Clarke Wright": "./clarke_wright",
        "Genetic Algorithm": "./genetic_algorithm",
        "Simulated Annealing": "./simulated_annealing"
    }
    
    # Get number of vehicles from file or use default
    num_vehicles = 2
    if os.path.exists("num_vehicles.txt"):
        with open("num_vehicles.txt") as f:
            num_vehicles = int(f.read().strip())
    
    # Save input data
    save_matrix(matrix)
    
    # Run the selected algorithm with num_vehicles as argument
    cmd = [algo_execs[algo_name], str(num_vehicles)]
    subprocess.run(cmd, stdout=subprocess.PIPE)
    
    # Load and return routes
    return load_output()