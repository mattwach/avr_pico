#!/bin/bash

for file in $(find . -name Makefile | grep -v '^./Makefile$'); do
  echo ${file}
  pushd $(dirname ${file})
		make clean
		popd
done


