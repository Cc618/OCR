#!/bin/bash

# Run this script from this directory

rm -rf dataset_tex dataset_pdf
mkdir -p dataset_tex dataset_pdf

# Generate tex files
python3 generate.py

# Generate pdf ones
cd dataset_tex
for file in *.tex
do
    latexmk -pdf -xelatex $file
    path=${file%.tex}.pdf
    mv $path ../dataset_pdf
done
cd ..

echo Done
