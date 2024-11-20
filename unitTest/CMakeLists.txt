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

function(add_test_executable NAME)
    add_executable(${NAME} ${NAME}.cxx)
    target_link_libraries(${NAME} PRIVATE dataframe-shared)
    add_test(
        NAME ${NAME}
        COMMAND $<TARGET_FILE:${NAME}>
    )
endfunction()

add_test_executable(basics)
add_test_executable(weigthedSum)
add_test_executable(addDotNegate)
add_test_executable(scalar)
add_test_executable(except)
add_test_executable(apply)
add_test_executable(transpose)
add_test_executable(div)
add_test_executable(decomposer)
add_test_executable(reduce)
add_test_executable(filter)
add_test_executable(eigen)
add_test_executable(stats)
add_test_executable(serie)
add_test_executable(cross)
add_test_executable(view)
add_test_executable(zip)
add_test_executable(pipe)
add_test_executable(map)
add_test_executable(equals)
add_test_executable(forEach)
add_test_executable(inv)

