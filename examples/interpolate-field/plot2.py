import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import argparse
import sys

def real_field(x, y):
    """The actual scalar field function"""
    return np.sin(2*x) * np.cos(2*y)

def main():
    parser = argparse.ArgumentParser(description='Compare real and interpolated fields')
    parser.add_argument('grid_file', help='CSV file containing grid points and distances')
    parser.add_argument('ref_file', help='CSV file containing reference points')
    parser.add_argument('disc_file', help='CSV file containing discontinuity curve')
    parser.add_argument('--output', '-o', default='field_comparison', help='Base name for output files')
    args = parser.parse_args()

    try:
        grid_data = pd.read_csv(args.grid_file)
        ref_points = pd.read_csv(args.ref_file)
        disc_points = pd.read_csv(args.disc_file)
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
    n = int(np.sqrt(len(grid_data)))
    X = grid_data['x'].values.reshape(n, n)
    Y = grid_data['y'].values.reshape(n, n)
    Z_interpolated = grid_data['distance'].values.reshape(n, n)

    # Compute real field values
    Z_real = real_field(X, Y)

    # Create custom colormap
    colors = ['darkblue', 'blue', 'lightblue', 'white', 'yellow', 'orange', 'red']
    n_bins = 100
    cmap = LinearSegmentedColormap.from_list("custom", colors, N=n_bins)

    # Create figure with two subplots side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(20, 8))

    # Plot 1: Real field
    c1 = ax1.contourf(X, Y, Z_real, levels=50, cmap=cmap)
    plt.colorbar(c1, ax=ax1, label='Value')
    
    # Plot reference points by group
    group1 = ref_points[ref_points['group'] == 1]
    group2 = ref_points[ref_points['group'] == 2]
    ax1.scatter(group1['x'], group1['y'], color='red', marker='o', s=30, label='Group 1')
    ax1.scatter(group2['x'], group2['y'], color='blue', marker='o', s=30, label='Group 2')
    
    # Plot discontinuity
    ax1.plot(disc_points['x'], disc_points['y'], 'k--', linewidth=2, label='Discontinuity')
    
    ax1.set_title('Real Field (sin(2x)cos(2y))')
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.axis('equal')
    ax1.grid(True)
    ax1.legend()

    # Plot 2: Interpolated field
    c2 = ax2.contourf(X, Y, Z_interpolated, levels=50, cmap=cmap)
    plt.colorbar(c2, ax=ax2, label='Value')
    
    # Plot reference points by group
    ax2.scatter(group1['x'], group1['y'], color='red', marker='o', s=30, label='Group 1')
    ax2.scatter(group2['x'], group2['y'], color='blue', marker='o', s=30, label='Group 2')
    
    # Plot discontinuity
    ax2.plot(disc_points['x'], disc_points['y'], 'k--', linewidth=2, label='Discontinuity')
    
    ax2.set_title('Interpolated Field')
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.axis('equal')
    ax2.grid(True)
    ax2.legend()

    plt.tight_layout()
    plt.savefig(f'{args.output}.png', dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == '__main__':
    main()