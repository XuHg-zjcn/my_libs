#!/bin/python
import numpy as np
from imageio import imread

from np2c import numpy2ccode

x0, y0 = 37, 8
dx, dy = 4, 6
nX = nY = 16

"""
image format:
 0 1 2 . . . nX-1
| | | |. . .| |  0
| | | |. . .| |  1
 . . . . . .| |  .
 . . . . . .| |  .
| | | | | | | |  nY-1

each "| |" is a char, the example is 3x5
      / \
    /     \
    0 1 2=dx-1
  ▒▒▒▒▒▒▒▒▒▒▒▒
  ▒▒  ██  ▒▒██  0
  ▒▒██████▒▒██  1
  ▒▒██  ██▒▒██  2
  ▒▒██████▒▒██  3
  ▒▒██  ██▒▒██  4=dy-1
  ▒▒▒▒▒▒▒▒▒▒▒▒ < split line
  ▒▒  ██  ▒▒██  next chars row
          ^  ^
 split line | next char

(x0, y0) are coordinate of split line cross on left-top corner
"""


img = imread('ASCII_3x5.png')
img = img[y0:y0+dy*nY, x0:x0+dx*nX, 2]  # cut image
img = img.reshape(nX, dy, nX, dx)       # Y,y,X,x
img = img.transpose(0, 2, 3, 1)         # Y,X,x,y
img = img.reshape(nX*nY, dx, dy)        # X*Y,x,y

img = img[:, 1:, :0:-1]                 # cut edge, flip y
img = img.reshape(-1, (dx-1)*(dy-1))    # filat
img = img >= 128                        # bin
img = img.astype(np.uint8)

# show example
#plt.imshow(img.reshape(-1, 3)[(dy-1)*0x30:(dx-1)*0x40])
#plt.show()

# convert to 16bit
"""
c0   c2
|0|5|A|  row0
|1|6|B|
|2|7|C|
|3|8|D|
|4|9|E|  row4

MSB                           LSB
|F|E|D|C|B|A|9|8|7|6|5|4|3|2|1|0|
  | column2 | column1 | column0 |
"""
out = np.zeros(img.shape[:-1], dtype=np.uint16)
for i in range(3*5):
    out += img[..., i] * (1 << i)


# generate commit
def my_chr(x):
    if 0x20 <= x <= 0x7E:
        return chr(x)
    else:
        return ' '


commit = []
commit.append('0x.. 0123456789ABCDEF')
for i in range(nY):
    chrs = [my_chr(0x10*i+j) for j in range(0x10)]
    chrs = ''.join(chrs)
    commit.append('0x{:x}. {}'.format(i, chrs))

# write to .c file, arr shape must 2d
np2c = numpy2ccode('fonts/font_3x5.h', 'font3x5', out, commit)
np2c.conv_1d(Ncol=16)
