import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.tri import Triangulation
from matplotlib.animation import FuncAnimation
import glob

# Read the mesh data
vertices = pd.read_csv('../../build/vertices.csv')
triangles = pd.read_csv('../../build/triangles.csv')
temperature = pd.read_csv('../../build/temperature.csv')

# Create triangulation
triang = Triangulation(vertices['x'], vertices['y'], triangles.values)

# Set up the figure
plt.style.use('dark_background')
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
fig.suptitle('Harmonic Diffusion Results')

# Plot the mesh on the left
ax1.set_title('Mesh')
ax1.triplot(triang, 'w-', alpha=0.2)
ax1.set_aspect('equal')
ax1.set_xlabel('X')
ax1.set_ylabel('Y')

# Plot the temperature field on the right
ax2.set_title('Temperature Field with Contours')
temp_plot = ax2.tripcolor(triang, temperature['temperature'], cmap='coolwarm', shading='gouraud')

# Add contour lines
levels = np.linspace(temperature['temperature'].min(), temperature['temperature'].max(), 20)
contour = ax2.tricontour(triang, temperature['temperature'], levels=levels, colors='white', alpha=0.5, linewidths=0.5)
ax2.clabel(contour, inline=True, fmt='%.2f', fontsize=8)

ax2.set_aspect('equal')
ax2.set_xlabel('X')
ax2.set_ylabel('Y')
plt.colorbar(temp_plot, ax=ax2, label='Temperature')

plt.tight_layout()
plt.savefig('final_result.png', dpi=300, bbox_inches='tight')
plt.close()

# Create animation if intermediate steps exist
step_files = sorted(glob.glob('../../build/temperature_step*.csv'))
if step_files:
    print(f"Creating animation from {len(step_files)} intermediate steps...")
    
    fig, ax = plt.subplots(figsize=(4, 4), dpi=72)
    plt.style.use('dark_background')
    
    def init():
        ax.set_title('Temperature Field Evolution')
        ax.set_aspect('equal')
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        return []
    
    def animate(frame):
        ax.clear()
        init()
        
        # Read temperature data for this frame
        temp_data = pd.read_csv(step_files[frame])
        
        # Plot temperature field
        temp_plot = ax.tripcolor(triang, temp_data['temperature'], 
                               cmap='coolwarm', shading='gouraud',
                               vmin=-1, vmax=1)
        
        # Add contour lines
        levels = np.linspace(-1, 1, 20)  # Fixed levels for consistent animation
        contour = ax.tricontour(triang, temp_data['temperature'], 
                              levels=levels, colors='white', 
                              alpha=0.5, linewidths=0.5)
        ax.clabel(contour, inline=True, fmt='%.2f', fontsize=8)
        
        if frame == 0:
            plt.colorbar(temp_plot, ax=ax, label='Temperature')
        
        ax.set_title(f'Step {frame+1}/{len(step_files)}')
        return [temp_plot]
    
    anim = FuncAnimation(fig, animate, init_func=init, 
                        frames=len(step_files), 
                        interval=100, blit=True)
    
    # Save animation
    anim.save('diffusion_evolution.gif', writer='pillow')
    plt.close()

print("Visualization complete!")