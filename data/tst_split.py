import PIL.Image as img

path = 'dataset_png/0_Hack.png'
im = img.open(path)

# Whole section height
height = 72
width = 24
# Gap before
dy = 12
# Height of the char
img_height = 48
for i in range(9, 12):
    # maxy = dy + i * height <=> maxy - dy = i * height <=>
    # i = (maxy - dy) // height
    y = dy + i * height
    char = im.crop((0, y, width, y + img_height))
    char = char.resize((32, 32))
    char.show()
