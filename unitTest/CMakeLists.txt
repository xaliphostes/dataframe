# Copyright (c) 2023 fmaerten@gmail.com
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
project(unitTest)

add_executable(weigthedSum weigthedSum.cxx)
target_link_libraries(weigthedSum PRIVATE dataframe-shared)
add_test(
  NAME weigthedSum
  COMMAND $<TARGET_FILE:weigthedSum>
)

add_executable(addDotNegate addDotNegate.cxx)
target_link_libraries(addDotNegate PRIVATE dataframe-shared)
add_test(
  NAME addDotNegate
  COMMAND $<TARGET_FILE:addDotNegate>
)

add_executable(scalar scalar.cxx)
target_link_libraries(scalar PRIVATE dataframe-shared)
add_test(
  NAME scalar
  COMMAND $<TARGET_FILE:scalar>
)

add_executable(except except.cxx)
target_link_libraries(except PRIVATE dataframe-shared)
add_test(
  NAME except
  COMMAND $<TARGET_FILE:except>
)

add_executable(apply apply.cxx)
target_link_libraries(apply PRIVATE dataframe-shared)
add_test(
  NAME apply
  COMMAND $<TARGET_FILE:apply>
)

add_executable(transpose transpose.cxx)
target_link_libraries(transpose PRIVATE dataframe-shared)
add_test(
  NAME transpose
  COMMAND $<TARGET_FILE:transpose>
)

add_executable(div div.cxx)
target_link_libraries(div PRIVATE dataframe-shared)
add_test(
  NAME div
  COMMAND $<TARGET_FILE:div>
)