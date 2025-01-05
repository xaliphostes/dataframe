[[Go back]](../README.md)

## Installation (only one time)
In this `py` directory, type
```
yarn install
```
(will also call yarn setup)

## Setup (each time you go there)
```
yarn setup
```

## Compile (as many time as necessary)
```sh
yarn build
```
Python library will be located in the `../bin` and `./tests` folders (for now).

## Testing the generated code

Go to the `dataframe/python/tests` folder and type

```sh
python3 NAME-OF-THE-TEST.py
```

## Packaging

### Being uptodate for Python
```sh
python -m pip install --upgrade pip build
```

### Creating the wheel
```sh
python3 -m build --wheel
```

### Installing the wheel
```sh
pip install dist/dataframe-0.1.0-*.whl
```

### Listing the wheel contents
```sh
unzip -l dist/dataframe-0.1.0-*.whl
```

### Testing the installation
If your are still in the `py` folder
```sh
python3 tests/attributes.py
```

### Displaying package info
Informations are gather from the pyproject.toml file.
```sh
pip3 show pyalgo
```

See [this link](https://pybind11.readthedocs.io/en/stable/compiling.html#modules-with-cmake) and [this link](https://scikit-build-core.readthedocs.io/en/latest/) for more informations.