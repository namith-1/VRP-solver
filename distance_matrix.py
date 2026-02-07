import requests

def get_distance_matrix(depot, deliveries):
    """
    depot: tuple (lat, lon)
    deliveries: list of tuples [(lat, lon), ...]
    returns: N x (N+1) matrix
    """
    coords = [depot] + deliveries
    n = len(deliveries)
    matrix = []

    for i in range(1, len(coords)):
        row = []
        # depot to delivery i
        url = f"http://router.project-osrm.org/route/v1/driving/{depot[1]},{depot[0]};{coords[i][1]},{coords[i][0]}?overview=false"
        r = requests.get(url).json()
        row.append(int(r['routes'][0]['distance'] // 1))

        for j in range(1, len(coords)):
            if i == j:
                row.append(0)
                continue
            url = f"http://router.project-osrm.org/route/v1/driving/{coords[i][1]},{coords[i][0]};{coords[j][1]},{coords[j][0]}?overview=false"
            r = requests.get(url).json()
            row.append(int(r['routes'][0]['distance'] // 1))

        matrix.append(row)

    return matrix
