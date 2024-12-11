
import pytest
from dataframe import Serie
from common import assert_array_equal, assert_double_equal

def test_serie_get():
    # Test scalar Serie
    s1 = Serie(1, [1.0, 2.0, 3.0])
    assert abs(s1.get(0) - 1.0) < 1e-10
    assert abs(s1.get(1) - 2.0) < 1e-10
    assert abs(s1.get(2) - 3.0) < 1e-10

    # Test vector Serie
    s2 = Serie(3, [1.0, 2.0, 3.0, 4.0, 5.0, 6.0])
    
    v1 = s2.get(0)
    assert_array_equal(v1, [1.0, 2.0, 3.0])
    
    v2 = s2.get(1)
    assert_array_equal(v2, [4.0, 5.0, 6.0])

# def test_serie_get_errors():
#     s1 = Serie(1, [1.0, 2.0, 3.0])
#     s2 = Serie(3, [1.0, 2.0, 3.0, 4.0, 5.0, 6.0])

#     # Test out of bounds
#     with pytest.raises(IndexError):
#         s1.get(3)  # Index out of bounds

#     with pytest.raises(IndexError):
#         s2.get(2)  # Index out of bounds

#     # Test optional explicit methods if implemented
#     if hasattr(Serie, 'getScalar'):
#         assert abs(s1.getScalar(0) - 1.0) < 1e-10
#         with pytest.raises(RuntimeError):
#             s2.getScalar(0)  # Cannot get scalar from vector Serie

#     if hasattr(Serie, 'getArray'):
#         assert_array_equal(s2.getArray(0), [1.0, 2.0, 3.0])
#         with pytest.raises(RuntimeError):
#             s1.getArray(0)  # Cannot get array from scalar Serie

def test_serie_get_edge_cases():
    # Empty Serie
    s_empty = Serie(1, [])
    # with pytest.raises(IndexError):
    #     s_empty.get(0)

    # Single element Serie
    s_single = Serie(2, [1.0, 2.0])
    v = s_single.get(0)
    assert_array_equal(v, [1.0, 2.0])

    # Large indices
    s = Serie(1, [1.0])
    # with pytest.raises(IndexError):
    #     s.get(1000000)  # Very large index
    
def test_serie_set():
    # Test scalar Serie
    s1 = Serie(1, 3)  # Serie with itemSize=1 and count=3
    
    # Set scalar values
    s1.set(0, 1.0)
    s1.set(1, 2.0)
    s1.set(2, 3.0)
    
    assert_double_equal(s1.get(0), 1.0, 1e-10)
    assert_double_equal(s1.get(1), 2.0, 1e-10)
    assert_double_equal(s1.get(2), 3.0, 1e-10)

    # Test vector Serie
    s2 = Serie(3, 2)  # Serie with itemSize=3 and count=2
    
    # Set vector values
    s2.set(0, [1.0, 2.0, 3.0])
    s2.set(1, [4.0, 5.0, 6.0])
    assert_array_equal(s2.get(0), [1.0, 2.0, 3.0])
    assert_array_equal(s2.get(1), [4.0, 5.0, 6.0])
        
test_serie_get()
# test_serie_get_errors()
test_serie_get_edge_cases()

test_serie_set()
