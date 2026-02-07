import streamlit as st
from distance_matrix import get_distance_matrix
from utils import parse_coordinates, index_to_coordinates
from vrp_solver import solve_vrp
from map_visualizer import plot_routes
from streamlit_folium import folium_static
import os

st.set_page_config(page_title="VRP Solver", layout="wide")
st.title(" Vehicle Routing Problem Solver with Real Routes")

st.markdown("""
This app solves the Vehicle Routing Problem (VRP) using various algorithms:
- **Brute Force**: Exact solution (only for small problems)
- **Nearest Neighbor**: Fast heuristic based on greedy selection
- **Clarke Wright**: Savings-based heuristic
- **Genetic Algorithm**: Evolutionary metaheuristic
- **Simulated Annealing**: Probabilistic metaheuristic
""")

# Input form
with st.form(key="vrp_form"):
    col1, col2 = st.columns(2)
    
    with col1:
        depot_str = st.text_input("Depot Coordinates (lat,lon)", "12.9716,77.5946")
        delivery_str = st.text_input(
            "Delivery Coordinates (semicolon separated lat,lon)", 
            "12.9352,77.6146;12.9279,77.6271;13.0352,77.5970"
        )
    
    with col2:
        num_vehicles = st.number_input("Number of Vehicles", min_value=1, max_value=10, value=2)
        algo = st.selectbox("Select Algorithm", [
            "Brute Force", 
            "Nearest Neighbor", 
            "Clarke Wright", 
            "Genetic Algorithm", 
            "Simulated Annealing"
        ])
        
    submit = st.form_submit_button("Solve VRP")

if submit:
    try:
        # Parse coordinates
        depot = parse_coordinates(depot_str)[0]
        deliveries = parse_coordinates(delivery_str)
        
        # Validate number of nodes for brute force
        if algo == "Brute Force" and len(deliveries) > 8:
            st.warning(f"⚠️ Brute Force algorithm is impractical for {len(deliveries)} nodes. Consider using a different algorithm or reducing the number of delivery points.")
            if not st.button("Proceed Anyway (may take a long time)"):
                st.stop()
        
        # Display coordinates
        with st.expander("📍 Coordinates"):
            st.write("Depot:", depot)
            st.write("Deliveries:", deliveries)
            st.write(f"Number of vehicles: {num_vehicles}")
        
        # Generate distance matrix
        with st.spinner("Generating distance matrix using OSRM..."):
            matrix = get_distance_matrix(depot, deliveries)
        
        # Display matrix
        with st.expander("📊 Distance Matrix"):
            st.text("Matrix Preview (distances in meters):")
            st.text("\n".join(["\t".join(map(str, row)) for row in matrix]))
        
        # Solve VRP
        with st.spinner(f"Solving VRP using {algo} algorithm..."):
            # Create args for C++ solver
            with open("num_vehicles.txt", "w") as f:
                f.write(str(num_vehicles))
                
            route_indices = solve_vrp(matrix, algo)
        
        # Visualize routes
        st.subheader("🛣️ Route Visualization")
        with st.spinner("Generating route visualization..."):
            route_map, html_file = plot_routes(route_indices, depot, deliveries, save_html=True)
            folium_static(route_map, width=1400, height=600)
            
            if html_file:
                with open(html_file, "rb") as f:
                    st.download_button(
                        label="Download Map as HTML",
                        data=f,
                        file_name="vrp_routes.html",
                        mime="text/html"
                    )
        
        # Display route information
        st.subheader("📋 Routes Information")
        total_distance = 0
        
        for i, route in enumerate(route_indices):
            # Calculate distance for this route
            route_distance = 0
            coords = [depot] + deliveries
            for j in range(len(route) - 1):
                if route[j] == 0:
                    idx1 = 0  # Depot
                else:
                    idx1 = route[j]
                
                if route[j+1] == 0:
                    idx2 = 0  # Depot
                else:
                    idx2 = route[j+1]
                
                if idx1 == 0 and idx2 == 0:
                    # Both depot, no distance
                    continue
                elif idx1 == 0:
                    # From depot to delivery
                    route_distance += matrix[idx2-1][0]
                elif idx2 == 0:
                    # From delivery to depot
                    route_distance += matrix[idx1-1][0]
                else:
                    # Between deliveries
                    route_distance += matrix[idx1-1][idx2]
            
            # Convert to km
            route_distance_km = route_distance / 1000
            total_distance += route_distance
            
            col1, col2 = st.columns([3, 1])
            with col1:
                st.write(f"**Vehicle {i+1}**: {' → '.join(['Depot' if n == 0 else f'Node {n}' for n in route])}")
            with col2:
                st.write(f"Distance: {route_distance_km:.2f} km")
        
        st.success(f"✅ Total distance: {total_distance/1000:.2f} km")
        
    except Exception as e:
        st.error(f"❌ Error: {e}")
        st.exception(e)