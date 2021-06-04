#!/bin/python
import numpy as np
from PIL import Image, ImageDraw
from np2c import numpy2ccode

img = Image.open('smg8.png')
w, h = img.size
w, h = w//7-2, h//11-2

#img.show()

def pix_on(x, y):
    r = img.getpixel((x, y))[0]
    return r > 128

arr = []
for i in range(1, h+1):
    for j in range(1, w+1):
        x0 = j*7
        y0 = i*11
        a = pix_on(x0+2, y0+0)
        b = pix_on(x0+4, y0+2)
        c = pix_on(x0+4, y0+6)
        d = pix_on(x0+2, y0+8)
        e = pix_on(x0+0, y0+6)
        f = pix_on(x0+0, y0+2)
        g = pix_on(x0+2, y0+4)
        h = pix_on(x0+5, y0+9)
        lst = [a, b, c, d, e, f, g, h]
        hx = sum([i and(1<<ni) for ni,i in enumerate(lst)])
        arr.append(hx)

arr = np.array(arr, dtype=np.uint8)
np2c = numpy2ccode('../../common/fonts/font_smg8.h', 'fontsmg8', arr)
np2c.conv_1d(Ncol=w)
