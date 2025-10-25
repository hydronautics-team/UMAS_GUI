import sys
import warnings

# Игнорировать предупреждения Matplotlib
warnings.filterwarnings("ignore", category=UserWarning, module='matplotlib')

sys.path.append('./algorithm_cpp')
from coveragePathPlanningAlgorithm import coverage_path_planning_algorithm

def parse_points(points_str):
    points = []
    pairs = points_str.split(" ")
    for pair in pairs:
        x, y = map(float, pair.split(","))
        points.append((x, y))  # Округление до целых чисел
    return points

def execute_algorithm(points, distance_tack):
    # print(f"Executing algorithm with points: {points} and distance_tack: {distance_tack}")
    result, ax = coverage_path_planning_algorithm(points, distance_tack)
    return result

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python execute_algorithm.py <points_string> <distance_tack>")
        sys.exit(1)

    points_string = sys.argv[1]
    distance_tack = float(sys.argv[2])
    points = parse_points(points_string)
    result = execute_algorithm(points, distance_tack)
    print(f"{result}")
