#!/bin/python
import numpy as np
import time
from datetime import datetime, timedelta
import struct

import numpy

from usbd import USBDevice

usbd = USBDevice(idVendor=0xffff, idProduct=0xf103)

err = []
for i in range(16):
    pc1 = time.time()
    pc2 = time.time()
    rtc = usbd.read(8)
    pc3 = time.time()

    rtc, = struct.unpack('I', rtc)
    pc = pc2
    print(rtc, pc)
    err.append(rtc-pc)

err = np.array(err)
print(err.mean(), err.std())

ts = time.time()
pc = struct.pack('I', int(ts)+1)
time.sleep(1 - ts%1)
usbd.write(pc)
