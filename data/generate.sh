#!/bin/bash

# Run this script from this directory

rm -rf dataset_tex dataset_pdf dataset_png
mkdir -p dataset_tex dataset_pdf dataset_png

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

# Convert lastly to png
python3 to_png.py

# Remove tmp files
# rm -rf dataset_{tex,pdf}

echo Done
