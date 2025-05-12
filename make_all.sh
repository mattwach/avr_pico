#!/bin/bash

set -e

for file in $(find . -name Makefile | grep -v '^./Makefile$'); do
  echo ${file}
  pushd $(dirname ${file})
		make
		popd
done


