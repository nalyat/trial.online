###############################################################################
#
# Copyright (C) 2016 Bjorn Reese <breese@users.sourceforge.net>
#
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################

cmake_minimum_required(VERSION 3.0)
project(trial.online)

add_subdirectory(cmake)
add_subdirectory(example)

enable_testing()
add_subdirectory(test)

add_subdirectory(benchmark EXCLUDE_FROM_ALL)
