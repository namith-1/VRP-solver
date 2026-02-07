import folium
import requests
import uuid


def get_route_polyline(start, end):
    url = f"http://router.project-osrm.org/route/v1/driving/{start[1]},{start[0]};{end[1]},{end[0]}?overview=full&geometries=geojson"
    r = requests.get(url).json()
    return r['routes'][0]['geometry']['coordinates']

def plot_routes(routes, depot, deliveries, save_html=False):
    m = folium.Map(location=depot, zoom_start=13)
    coords = [depot] + deliveries

    for idx, route in enumerate(routes):
        color = f"hsl({(idx * 70) % 360}, 100%, 40%)"
        for i in range(len(route) - 1):
            start = coords[route[i]]
            end = coords[route[i+1]]
            line = get_route_polyline(start, end)
            folium.PolyLine(locations=[(lat, lon) for lon, lat in line], color=color, weight=5).add_to(m)

    for i, (lat, lon) in enumerate(coords):
        folium.Marker(location=(lat, lon), popup=f"{i} ({'Depot' if i == 0 else 'Node'})").add_to(m)

    # Optional Save to HTML
    if save_html:
        unique_filename = f"route_map_{uuid.uuid4().hex[:8]}.html"
        m.save(unique_filename)
        return m, unique_filename

    return m, None