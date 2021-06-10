#!/bin/python
import csv
import struct
import time
from datetime import datetime, timedelta

import numpy as np

from usbd import USBDevice

N_READ = 16
usbd = USBDevice(idVendor=0xffff, idProduct=0xf103)

t0 = datetime.now()
def read_time():
    rtc = usbd.read(6)
    pc = time.time()
    cnt, prl = struct.unpack('IH', rtc)
    rtc = cnt + (32767-prl)/32768
    return rtc-pc

def Nread_time(n):
    lst = []
    for i in range(n):
        lst.append(read_time())
    return np.array(lst)

def send():
    ts = time.time()
    pc = struct.pack('I', int(ts)+1)
    time.sleep(1 - ts%1)
    usbd.write(pc)
    return int(ts)+1


arr1 = Nread_time(N_READ)
mean1 = arr1.mean()
std1 = arr1.std()
mM1 = arr1.max() - arr1.min()

ts_send = send()

arr2 = Nread_time(N_READ)
mean2 = arr2.mean()
std2 = arr2.std()
mM2 = arr2.max() - arr2.min()

with open('log.csv', 'a+') as f:
    csvf = csv.writer(f)
    if f.tell() == 0:
        csvf.writerow(['calib_time', 'N_READ', 'err_mean', 'err_std', 'mM', 'send_time', 'err_mean2', 'err_std2', 'mM'])
    st0 = t0.strftime("%Y-%m-%d %H:%M:%S")
    st1 = datetime.fromtimestamp(ts_send).strftime("%Y-%m-%d %H:%M:%S")
    csvf.writerow([st0, N_READ, mean1, std1, mM1, st1, mean2, std2, mM2])

