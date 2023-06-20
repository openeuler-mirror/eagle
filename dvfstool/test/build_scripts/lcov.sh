#!/bin/bash

rm -rf report
mkdir report

lcov --rc lcov_branch_coverage=1 -c -o ./report/demotest.info -d .

lcov --rc lcov_branch_coverage=1 -e ./report/demotest.info "*dvfstool/*.c" -o ./report/demotest_extract.info

genhtml --rc lcov_branch_coverage=1 ./report/demotest_extract.info -o ./report/gen_html

