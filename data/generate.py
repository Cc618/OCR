#!/usr/bin/env python3

# * Generates all tex files
# * Fonts location (arch) : /usr/share/fonts/*
# https://wiki.archlinux.org/index.php/Fonts
# * Fonts with texlive :
# https://tex.stackexchange.com/questions/59403/ \
# what-font-packages-are-installed-in-tex-live
# * We can add fontspec fonts like these :
# Hack
# Roboto
# DejaVuSansMono
# FreeMono
# FreeSerif

with open('fonts.txt') as f:
    fonts = f.read().strip().split('\n')

with open('content.txt') as f:
    content = f.read().rstrip()

with open('dataset.template.tex') as f:
    template = f.readlines()

i_setfont = next(i for i, ln in enumerate(template) if 'SETFONT' in ln)
i_content = next(i for i, ln in enumerate(template) if 'CONTENT' in ln)

bold = True
if bold:
    content = ['\\textbf{' + c + '}' for c in content]

content_raw = '\n\n'.join(content)
template[i_content] = content_raw + '\n'

# Generate tex
for font in fonts:
    path = 'dataset_tex/' + font + '.tex'
    print('Generating', path)

    # If using fontspec : template[i_setfont] = r'\setmainfont{' + font + '}\n'
    template[i_setfont] = r'\usepackage{' + font + '}\n'
    data = ''.join(template)

    with open(path, 'w') as f:
        f.writelines(template)
