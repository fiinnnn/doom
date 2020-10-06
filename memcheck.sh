#!/bin/sh
valgrind --leak-check=full --show-leak-kinds=all --keep-debuginfo=yes --suppressions=valgrind_suppress bin/doom
