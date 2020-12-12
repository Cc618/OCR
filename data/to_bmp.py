# Convert all pdfs to bmps

import os
from glob import glob
from pdf2image import convert_from_path


def bounds_traversal(im, visited, box, i, j, y, w, h):
    # Almost DFS to get other chars
    s = [(i, j)]
    while len(s) > 0:
        i, j = s.pop()
        visited[i * w + j] = True

        if im.getpixel((j, y + i))[0] > .9:
            continue

        box[0] = min(box[0], j)
        box[1] = min(box[1], i)
        box[2] = max(box[2], j)
        box[3] = max(box[3], i)

        if i > 0 and not visited[(i - 1) * w + j]:
            s.append((i - 1, j))

        if j > 0 and not visited[i * w + j - 1]:
            s.append((i, j - 1))

        if i < h - 1 and not visited[(i + 1) * w + j]:
            s.append((i + 1, j))

        if j < w - 1 and not visited[i * w + j + 1]:
            s.append((i, j + 1))


def bounds(im, y, w, h):
    '''
    Returns the bounds of the character
    '''
    box = []
    visited = [False] * w * h
    for i in range(h):
        for j in range(w):
            # Found
            if not visited[i * w + j] and im.getpixel((j, y + i))[0] < .9:
                visited[i * w + j] = True

                if box == []:
                    box = [j, i, j, i]

                bounds_traversal(im, visited, box, i, j, y, w, h)
                print('Traversal', j, i)
            else:
                visited[i * w + j] = True

    print(box)

    # box[2] = box[2] - box[0]
    # box[3] = box[3] - box[1]

    box[1] += y
    box[3] += y

    box[2] += 1
    box[3] += 1

    assert box[2] > 2 and box[3] > 2, f'Too small char detected : {box}'

    return box


# Whole section height
height = 73
width = 24
# Gap before
dy = 6
# Height of the char
img_height = 42

with open('content.txt') as f:
    content = f.read().rstrip()

impaths = glob('dataset_pdf/*')
for impath in impaths:
    name = os.path.splitext(os.path.basename(impath))[0]
    print('Converting to bmp', name)

    pages = convert_from_path(impath)
    maxy = pages[0].height
    # Chars per page
    nchars = (maxy - dy) // height + 1

    for i in range(len(content)):
        # Get page and index of the char
        im = pages[i // nchars]
        di = i % nchars

        # Retrieve the image of the char
        y = dy + di * height

        try:
            box = bounds(im, y, width, height)
        except Exception as e:
            raise Exception(f'Char {content[i]}, font {name} : {e}')

        print(box)

        char = im.crop(box)


        # TODO : Nearest neighbor
        char = char.resize((32, 32))

        # Save char
        char.save(f'dataset_bmp/{content[i]}_{name}.bmp')
        exit()
