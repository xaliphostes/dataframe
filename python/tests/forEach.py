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

from dataframe import Serie, forEach
import pytest

def assert_equal(a, b):
    assert abs(a - b) == 0
    
def assert_array_equal(arr1, arr2):
    assert len(arr1) == len(arr2), f"Arrays have different lengths: {len(arr1)} != {len(arr2)}"
    for a, b in zip(arr1, arr2):
        assert abs(a - b) == 0, f"Array elements differ: {a} != {b}"


# For 1 Serie
s1Vals = [1, 2, 3]
s1 = Serie(1, s1Vals)
forEach(lambda v1, index: assert_equal(v1, s1Vals[index]), s1)

# For 2 Series
s2Vals = [[10,20], [30,40], [50,60]]
s2 = Serie(2, [10,20, 30,40, 50,60])
def check2(v1, v2, index):
    assert_equal(v1, s1Vals[index])
    assert_array_equal(v2, s2Vals[index])
forEach(lambda v1, v2, index: check2(v1, v2, index), [s1, s2])

# For 3 Series
s3Vals = [[1,2,3], [4,5,6], [7,8,9]]
s3 = Serie(3, [1,2,3, 4,5,6, 7,8,9])
def check3(v1, v2, v3, index):
    assert_equal(v1, s1Vals[index])
    assert_array_equal(v2, s2Vals[index])
    assert_array_equal(v3, s3Vals[index])
forEach(lambda v1, v2, v3, index: check3(v1, v2, v3, index), [s1, s2, s3])

