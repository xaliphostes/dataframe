import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import argparse
import sys

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Visualize distance field from grid and reference points')
    parser.add_argument('grid_file', help='CSV file containing grid points and distances')
    parser.add_argument('ref_file', help='CSV file containing reference points')
    parser.add_argument('--output', '-o', default='interpolated_field', help='Base name for output files (default: interpolated_field)')
    args = parser.parse_args()

    try:
        # Read the data
        grid_data = pd.read_csv(args.grid_file)
        ref_points = pd.read_csv(args.ref_file)
    except FileNotFoundError as e:
        print(f"Error: Could not find input file: {e.filename}")
        sys.exit(1)
    except pd.errors.EmptyDataError:
        print(f"Error: File is empty")
        sys.exit(1)
    except pd.errors.ParserError:
        print(f"Error: File is not in the expected CSV format")
        sys.exit(1)

    # Reshape the data into a grid
    n = int(np.sqrt(len(grid_data)))  # Assuming square grid
    X = grid_data['x'].values.reshape(n, n)
    Y = grid_data['y'].values.reshape(n, n)
    Z = grid_data['distance'].values.reshape(n, n)

    # Create custom colormap
    colors = ['yellow', 'blue', 'lightblue', 'white', 'yellow', 'orange', 'red']
    n_bins = 100
    cmap = LinearSegmentedColormap.from_list("custom", colors, N=n_bins)

    # Create the 2D plot
    plt.figure(figsize=(12, 10))

    # Plot the distance field
    plt.contourf(X, Y, Z, levels=50, cmap=cmap)
    plt.colorbar(label='Distance')

    # Plot the reference points
    plt.scatter(ref_points['x'], ref_points['y'], 
               color='black', marker='o', s=30, 
               label='Reference Points')

    plt.title('Distance Field with Reference Points')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.axis('equal')
    plt.legend()
    plt.grid(True)

    # Save the 2D plot
    plt.savefig(f'{args.output}_2d.png', dpi=300, bbox_inches='tight')
    plt.close()

    # Create a 3D surface plot
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')

    surf = ax.plot_surface(X, Y, Z, cmap=cmap)
    ax.scatter(ref_points['x'], ref_points['y'], 
              np.zeros_like(ref_points['x']), 
              color='black', marker='o', s=30)

    plt.colorbar(surf, label='Distance')
    ax.set_title('3D View of Distance Field')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Distance')

    # Save the 3D plot
    plt.savefig(f'{args.output}_3d.png', dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == '__main__':
    main()