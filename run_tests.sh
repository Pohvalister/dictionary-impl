# !/bin/bash

project_name=Dictionary
files_folder=build

if ! [ -d ./$files_folder/ ]; then
echo 'Files will be generated in '$files_folder
mkdir $files_folder
fi

cd $files_folder
cmake ..
make

./$project_name