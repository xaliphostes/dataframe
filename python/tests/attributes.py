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

from dataframe.attributes import Manager
from dataframe import Serie, Dataframe
from common import assert_array_equal

dataframe = Dataframe()
dataframe.add("a", Serie(1, [1,2]))
dataframe.add("U", Serie(3, [1,2,3, 4,5,6]))
dataframe.add("S", Serie(6, [1,2,3,4,5,6, 7,8,9,0,1,2]))

manager = Manager(dataframe, [
    "Coordinates", 
    "Components", 
    "EigenValues", 
    "EigenVectors"
], 3)

assert_array_equal(manager.names(1), ["S1", "S2", "S3", "Sxx", "Sxy", "Sxz", "Syy", "Syz", "Szz", "Ux", "Uy", "Uz", "a"])
assert_array_equal(manager.names(3), ["S1", "S2", "S3", "U"])
assert_array_equal(manager.names(6), ["S"])
