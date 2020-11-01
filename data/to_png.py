# Convert all pdfs to pngs

import os
from glob import glob
from pdf2image import convert_from_path

# Whole section height
height = 72
width = 24
# Gap before
dy = 12
# Height of the char
img_height = 48

with open('content.txt') as f:
    content = f.read().rstrip()

impaths = glob('dataset_pdf/*')
for impath in impaths:
    name = os.path.splitext(os.path.basename(impath))[0]
    print('Converting to png', name)

    pages = convert_from_path(impath)
    maxy = pages[0].height
    # Chars per page
    nchars = (maxy - dy) // height
    # print('#chars =', nchars, '2nd page first char =', content[nchars])

    for i in range(len(content)):
        # Get page and index of the char
        im = pages[i // nchars]
        di = i % nchars

        # Retrieve the image of the char
        y = dy + di * height
        char = im.crop((0, y, width, y + img_height))
        char = char.resize((32, 32))

        # Save char
        char.save(f'dataset_png/{content[i]}_{name}.png')

    # for i, img in enumerate(pages):
    #     img.save(f'dataset_png/{i}_{name}.png')
