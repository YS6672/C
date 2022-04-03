#!/bin/bash
cmake .. -DBUILD_SHARED_LIBS=ON && make && ctest --output-on-failure
