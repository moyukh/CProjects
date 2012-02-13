#!/usr/local/bin/bash

for i in $(find verbose -type f) ; do echo user: $(basename $i) ; cat $i | awk -f reference/compare-grades.awk  ; done