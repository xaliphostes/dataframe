import pytest

def assert_equal(a, b):
    assert abs(a - b) == 0
    
def assert_array_equal(arr1, arr2):
    assert len(arr1) == len(arr2), f"Arrays have different lengths: {len(arr1)} != {len(arr2)}"
    for a, b in zip(arr1, arr2):
        assert abs(a - b) == 0, f"Array elements differ: {a} != {b}"
        
def assert_double_equal(a, b, tolerance=1e-4):
    assert abs(a - b) <= tolerance
    
def assert_array_equal(arr1, arr2, tolerance=1e-4):
    assert len(arr1) == len(arr2), f"Arrays have different lengths: {len(arr1)} != {len(arr2)}"
    for a, b in zip(arr1, arr2):
        assert abs(a - b) <= tolerance, f"Array elements differ: {a} != {b}"
