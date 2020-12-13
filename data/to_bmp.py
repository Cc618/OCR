# Convert all pdfs to bmps

import os
from glob import glob
from pdf2image import convert_from_path
from PIL import Image


def bounds_traversal(im, visited, box, i, j, y, w, h):
    # Almost DFS to get other chars
    s = [(i, j)]
    while len(s) > 0:
        i, j = s.pop()
        visited[i * w + j] = True

        if im.getpixel((j, y + i))[0] > .5:
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
    if y + h > im.height:
        h = im.height - y

    box = []
    visited = [False] * w * h
    for i in range(h):
        for j in range(w):
            # Found
            if not visited[i * w + j] and im.getpixel((j, y + i))[0] < .5:
                visited[i * w + j] = True

                if box == []:
                    box = [j, i, j, i]

                bounds_traversal(im, visited, box, i, j, y, w, h)
            else:
                visited[i * w + j] = True

    box[1] += y
    box[3] += y

    box[2] += 1
    box[3] += 1

    assert box[2] > 2 and box[3] > 2, f'Too small char detected : {box}'

    return box


def bounds2(im, y, w, h):
    if y + h > im.height:
        h = im.height - y

    box = []
    for i in range(h):
        for j in range(w):
            # Found
            if im.getpixel((j, y + i))[0] < .9 or \
                    im.getpixel((j, y + i))[1] < .9 or \
                    im.getpixel((j, y + i))[2] < .9 :
                if box == []:
                    box = [j, i, j + 1, i + 1]
                else:
                    box[0] = min(box[0], j)
                    box[1] = min(box[1], i)
                    box[2] = max(box[2], j)
                    box[3] = max(box[3], i)

    box[0] = max(0, box[0] - 4)
    box[1] += y - 4
    box[2] += 4
    box[3] += y + 4

    assert box[2] - box[0] > 2 and box[3] - box[1] > 2, \
            f'Too small char detected : {box}'

    return box


# Whole section height
height = 73
width = 24
# width = 100
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
            box = bounds2(im, y, width, height)
            # box = 0, y, width, y + img_height
        except Exception as e:
            raise Exception(f'!!! Char {content[i]}, font {name} : {e}')

        char = im.crop(box)

        w = box[2] - box[0]
        h = box[3] - box[1]
        if w > h:
            nwidth = 32
            nheight = h * 32 // w
        else:
            nheight = 32
            nwidth = w * 32 // h

        char = char.resize((nwidth, nheight), resample=Image.NEAREST)
        newimg = Image.new('RGB', (32, 32), color='#ffffff')
        newimg.paste(char)

        # Save char
        newimg.save(f'dataset_bmp/{content[i]}_{name}.bmp')
