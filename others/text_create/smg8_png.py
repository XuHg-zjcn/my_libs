#!/bin/python
from PIL import Image, ImageDraw

#s = input('待输出图片数码管个数AxB:')
s = '10x10'
w,h = s.split('x')
w,h = int(w), int(h)

img = Image.new('RGB', ((w+2)*7, (h+2)*11))
draw = ImageDraw.Draw(img)
fill = 0x202020

for i in range(1, h+1):
    for j in range(1, w+1):
        x0 = j*7
        y0 = i*11
        draw.line((x0+1, y0+0, x0+3, y0+0), fill=fill, width=1) #a
        draw.line((x0+4, y0+1, x0+4, y0+3), fill=fill, width=1) #b
        draw.line((x0+4, y0+5, x0+4, y0+7), fill=fill, width=1) #c
        draw.line((x0+1, y0+8, x0+3, y0+8), fill=fill, width=1) #d
        draw.line((x0+0, y0+5, x0+0, y0+7), fill=fill, width=1) #e
        draw.line((x0+0, y0+1, x0+0, y0+3), fill=fill, width=1) #f
        draw.line((x0+1, y0+4, x0+3, y0+4), fill=fill, width=1) #g
        draw.point((x0+5, y0+9), fill=fill)             #h

img.save('smg8.png')
