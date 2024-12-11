

import pytest
import multiprocessing
from dataframe import Serie
from dataframe.utils import concat, parallel_execute
from common import assert_array_equal, assert_double_equal

def test_concat():
    # Create test series
    s1 = Serie(2, [1.0, 2.0, 3.0, 4.0])      # 2 items of size 2
    s2 = Serie(2, [5.0, 6.0, 7.0, 8.0])      # 2 items of size 2
    s3 = Serie(2, [9.0, 10.0, 11.0, 12.0])   # 2 items of size 2
    s4 = Serie(2, [13.0, 14.0, 15.0, 16.0])  # 2 items of size 2

    def assert_array_equal(arr1, arr2, tolerance=1e-10):
        assert len(arr1) == len(arr2)
        for a, b in zip(arr1, arr2):
            assert abs(a - b) <= tolerance

    # Test with different numbers of series
    result2 = concat([s1, s2])
    assert result2.count() == 4
    assert result2.itemSize() == 2
    assert_array_equal(result2.get(0), [1.0, 2.0])
    assert_array_equal(result2.get(2), [5.0, 6.0])

    result3 = concat([s1, s2, s3])
    assert result3.count() == 6
    assert result3.itemSize() == 2
    assert_array_equal(result3.get(4), [9.0, 10.0])

    result4 = concat([s1, s2, s3, s4])
    assert result4.count() == 8
    assert result4.itemSize() == 2
    assert_array_equal(result4.get(6), [13.0, 14.0])

    # Test error cases
    with pytest.raises(ValueError):
        # Try to concat series with different itemSize
        s_diff = Serie(3, [1.0, 2.0, 3.0])
        concat([s1, s_diff])

    with pytest.raises(ValueError):
        # Test empty list
        concat([])

    # Test single series
    result1 = concat([s1])
    assert result1.count() == 2
    assert result1.itemSize() == 2
    assert_array_equal(result1.get(0), [1.0, 2.0])

def test_concat_large_number_series():
    # Test with many series
    series_list = []
    for i in range(10):  # Create 10 series
        series_list.append(Serie(2, [float(i*2), float(i*2+1)]))
    
    result = concat(series_list)
    assert result.count() == 10
    assert result.itemSize() == 2
    assert_array_equal(result.get(0), [0.0, 1.0])
    assert_array_equal(result.get(9), [18.0, 19.0])
    
def test_parallel_execute():
    # Create a test serie
    input_serie = Serie(2, [1.0, 2.0, 3.0, 4.0, 5.0, 6.0])  # 3 items of size 2
    
    # Test callback that doubles all values
    def double_values(serie):
        result = Serie(serie.itemSize(), serie.count())
        for i in range(serie.count()):
            values = serie.get(i)
            doubled = [v * 2 for v in values]
            result.set(i, doubled)
        return result
    
    # Get number of cores (for testing different configurations)
    max_cores = multiprocessing.cpu_count()
    
    # Test with different numbers of cores
    for cores in [1, 2, min(4, max_cores)]:
        result = parallel_execute(double_values, input_serie, cores)
        
        # Verify result
        assert result.count() == input_serie.count()
        assert result.itemSize() == input_serie.itemSize()
        
        # Check all values were doubled
        for i in range(result.count()):
            original = input_serie.get(i)
            doubled = result.get(i)
            assert len(doubled) == len(original)
            for orig, doub in zip(original, doubled):
                assert_double_equal(doub, orig * 2, 1e-10)

def test_parallel_execute_errors():
    input_serie = Serie(2, [1.0, 2.0, 3.0, 4.0])
    
    # Test invalid number of cores
    with pytest.raises(ValueError):
        parallel_execute(lambda s: s, input_serie, 0)
    
    # Test callback that returns wrong size
    def bad_callback(serie):
        return Serie(2, [1.0, 2.0])  # Always returns size 1
    
    with pytest.raises(RuntimeError):
        parallel_execute(bad_callback, input_serie, 2)
    
    # Test callback that raises an exception
    def failing_callback(serie):
        raise ValueError("Test error")
    
    with pytest.raises(RuntimeError):
        parallel_execute(failing_callback, input_serie, 2)

def test_parallel_execute_large_data():
    # Create a larger test serie
    large_serie = Serie(3, [float(i) for i in range(1000)])
    
    def process_large_data(serie):
        result = Serie(serie.itemSize(), serie.count())
        for i in range(serie.count()):
            values = serie.get(i)
            processed = [v + 1 for v in values]  # Simple operation
            result.set(i, processed)
        return result
    
    # Test with multiple cores
    result = parallel_execute(process_large_data, large_serie, 4)
    
    # Verify result
    assert result.count() == large_serie.count()
    assert result.itemSize() == large_serie.itemSize()
    
    # Check random samples
    import random
    for _ in range(10):
        i = random.randint(0, result.count() - 1)
        original = large_serie.get(i)
        processed = result.get(i)
        assert all(abs(p - (o + 1)) < 1e-10 for p, o in zip(processed, original))
    
test_concat()
test_concat_large_number_series()

test_parallel_execute()
# test_parallel_execute_large_data()