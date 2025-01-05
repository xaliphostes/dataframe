# Copyright (c) 2024-now fmaerten@gmail.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
import pytest

def assert_equal(a, b):
    assert abs(a - b) == 0
    
def assert_array_equal(arr1, arr2):
    assert len(arr1) == len(arr2), f"Arrays have different lengths: {len(arr1)} != {len(arr2)}"
    if type(arr1[0]) == str:
        for a, b in zip(arr1, arr2):
            assert a == b, f"Array elements differ: {a} != {b}"
    else:
        for a, b in zip(arr1, arr2):
            assert abs(a - b) == 0, f"Array elements differ: {a} != {b}"
        
def assert_double_equal(a, b, tolerance=1e-4):
    assert abs(a - b) <= tolerance
    
def assert_array_equal(arr1, arr2, tolerance=1e-4):
    assert len(arr1) == len(arr2), f"Arrays have different lengths: {len(arr1)} != {len(arr2)}"
    if type(arr1[0]) == str:
        for a, b in zip(arr1, arr2):
            assert a == b, f"Array elements differ: {a} != {b}"
    else:
        for a, b in zip(arr1, arr2):
            assert abs(a - b) <= tolerance, f"Array elements differ: {a} != {b}"
