#!/bin/python
from PIL import Image
import numpy as np

from np2c import numpy2ccode

Wmax = 128
Hmax = 64


def binimg2byte(x):  # [h, w],bool -> [h//8, w],u8
    h, w = x.shape
    assert h%8 == 0
    assert x.dtype == np.bool
    x = x.reshape(-1, 8, w)   #[h/8, 8, w]
    x = x.transpose(1, 0, 2)  #[8, h/8, w]
    ret = np.zeros(x.shape[1:], dtype=np.uint8)
    for b in x:
         ret <<= 1
         ret += b
    return ret

fn = input('请输入图片文件名:')
img = Image.open(fn)
if img.mode == 'RGB':
    img = img.convert('L')
w, h = img.size
if w < h:
    img.rotate(90)
    w, h = img.size
if h > 64 or w > 128:
    s = input('图片太大，请输入新尺寸，格式"WxH":')
    w, h = map(int, s.split('x'))
    img = img.resize((w, h))
assert w<=Wmax, '太宽'
assert h<=Hmax, '太高'

#img.show()

im = np.array(img)
vals = set(im.reshape(-1))
vals = sorted(vals)

if len(vals) > 4:
    n = int(input('原图灰阶数大于4, 请输入新阶数:'))
    assert n <= 4
    img = img.convert('P', palette=1, colors=n)
else:
    n = len(vals)

if n == 1:
    raise ValueError('only one color')
elif n == 2:
    im0 = binimg2byte(im==vals[1])
    ims = [im0]
elif n == 3:
    im0 = binimg2byte(im==vals[2])
    im1 = binimg2byte(im>=vals[1])
    ims = [im0, im1]
elif n == 4:
    im0 = binimg2byte(im>=vals[2])
    im1 = binimg2byte((im==vals[1]) | (im==vals[3]))
    ims = [im0, im1]
ims = np.array(ims)

np2c = numpy2ccode('img.c', 'codetab', ims.reshape(-1))
np2c.conv_1d(Ncol=ims.shape[-1])
