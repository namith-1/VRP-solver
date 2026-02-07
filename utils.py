def parse_coordinates(coord_str):
    """
    Parses input string like: "12.97,77.59;13.01,77.60"
    to [(12.97, 77.59), (13.01, 77.60)]
    """
    return [tuple(map(float, pair.split(","))) for pair in coord_str.split(";")]

def index_to_coordinates(route_indices, depot, deliveries):
    """
    Converts list of index-based route paths to coordinate-based paths
    """
    coords = [depot] + deliveries
    return [[coords[i] for i in route] for route in route_indices]

