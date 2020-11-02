#!/usr/bin/env python3

# Generates all tex files
# Fonts location (arch) : /usr/share/fonts/*
# https://wiki.archlinux.org/index.php/Fonts

with open('fonts.txt') as f:
    fonts = f.read().strip().split('\n')

with open('content.txt') as f:
    content = f.read().rstrip()

with open('dataset.template.tex') as f:
    template = f.readlines()

i_setfont = next(i for i, ln in enumerate(template) if 'SETFONT' in ln)
i_content = next(i for i, ln in enumerate(template) if 'CONTENT' in ln)

content_raw = '\n\n'.join(content)
template[i_content] = content_raw + '\n'

# Generate tex
for font in fonts:
    path = 'dataset_tex/' + font + '.tex'
    print('Generating', path)

    template[i_setfont] = r'\setmainfont{' + font + '}\n'
    data = ''.join(template)

    with open(path, 'w') as f:
        f.writelines(template)