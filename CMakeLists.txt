# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)

target_sources(app PRIVATE src/main.c)

# To include iis2dh.h:
zephyr_include_directories($ENV{ZEPHYR_BASE}/drivers/sensor/iis2dh)
