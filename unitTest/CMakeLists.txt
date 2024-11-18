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

add_executable(basics basics.cxx)
target_link_libraries(basics PRIVATE dataframe-shared)
add_test(
  NAME basics
  COMMAND $<TARGET_FILE:basics>
)

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

add_executable(decomposer decomposer.cxx)
target_link_libraries(decomposer PRIVATE dataframe-shared)
add_test(
  NAME decomposer
  COMMAND $<TARGET_FILE:decomposer>
)

add_executable(reduce reduce.cxx)
target_link_libraries(reduce PRIVATE dataframe-shared)
add_test(
  NAME reduce
  COMMAND $<TARGET_FILE:reduce>
)

add_executable(filter filter.cxx)
target_link_libraries(filter PRIVATE dataframe-shared)
add_test(
  NAME filter
  COMMAND $<TARGET_FILE:filter>
)

add_executable(eigen eigen.cxx)
target_link_libraries(eigen PRIVATE dataframe-shared)
add_test(
  NAME eigen
  COMMAND $<TARGET_FILE:eigen>
)

add_executable(stats stats.cxx)
target_link_libraries(stats PRIVATE dataframe-shared)
add_test(
  NAME stats
  COMMAND $<TARGET_FILE:stats>
)

add_executable(serie serie.cxx)
target_link_libraries(serie PRIVATE dataframe-shared)
add_test(
  NAME serie
  COMMAND $<TARGET_FILE:serie>
)

add_executable(cross cross.cxx)
target_link_libraries(cross PRIVATE dataframe-shared)
add_test(
  NAME cross
  COMMAND $<TARGET_FILE:cross>
)

add_executable(view view.cxx)
target_link_libraries(view PRIVATE dataframe-shared)
add_test(
  NAME view
  COMMAND $<TARGET_FILE:view>
)

add_executable(zip zip.cxx)
target_link_libraries(zip PRIVATE dataframe-shared)
add_test(
  NAME zip
  COMMAND $<TARGET_FILE:zip>
)

add_executable(pipe pipe.cxx)
target_link_libraries(pipe PRIVATE dataframe-shared)
add_test(
  NAME pipe
  COMMAND $<TARGET_FILE:pipe>
)

add_executable(map map.cxx)
target_link_libraries(map PRIVATE dataframe-shared)
add_test(
  NAME map
  COMMAND $<TARGET_FILE:map>
)
