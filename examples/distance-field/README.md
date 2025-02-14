# Usage
```
usage: plot.py [-h] [--output OUTPUT] grid_file ref_file

Visualize distance field from grid and reference points

positional arguments:
  grid_file             CSV file containing grid points and distances
  ref_file              CSV file containing reference points

options:
  -h, --help            show this help message and exit
  --output OUTPUT, -o OUTPUT
                        Base name for output files (default: distance_field)
```

# Comment
Since the generated files are in the build directory, here is how to launch the python script:
```bash
python3 plot.py ../../build/grid_points.csv ../../build/reference_points.csv
```

or to have some help about the python script:
```bash
python3 plot.py -h 
```