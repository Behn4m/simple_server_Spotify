#!/bin/bash

# Define your main folder
main_folder=$(pwd)


# Print header
echo "idf_component_register(
    SRCS
    "


# Use find command to search for .c files in the main folder and its immediate subdirectories
# Print the path of each .c file relative to the main folder, enclosed in double quotes
find "$main_folder" -mindepth 0 -maxdepth 20 -type f -name '*.c' -printf "\"%P\"\n"
echo " INCLUDE_DIRS 
"
find "$main_folder" -mindepth 0 -maxdepth 20 -type f -name '*.h' -printf "\"%P\"\n" | sed 's|/[^/]*$||' | sort -u
echo "     REQUIRES lvgl__lvgl lvgl_esp32_drivers  driver esp_timer GlobalInit driver espressif__button) "