#!/bin/python
import time
from datetime import datetime, timedelta
import struct
import sqlite3

import numpy as np

from usbd import USBDevice

N_READ = 16
N_READ_MS = 20
TIMESTAMP_BIAS = 946684800

pack_ts = 0x00
getN_ts = 0x01
getN_stop = 0x02
get_clock = 0x03
set_clock = 0x04
prep_CNT = 0x05
set_prep = 0x06
rtc_exit = 0x07

class RTC:
    def __init__(self, usbd, name='rtc0', conn=None):
        self.usbd = usbd
        self.name = name
        self.conn = conn

    def read_time(self):
        rtc = self.usbd.read(6)
        pc = time.time()
        prl, cnt = struct.unpack('<HI', rtc)
        rtc = cnt + (32767-prl)/32768 + TIMESTAMP_BIAS
        return rtc-pc

    def Nread_time(self, n=N_READ, ms=N_READ_MS):
        arr = []
        ts = datetime.now()
        usbd.write(struct.pack('<BHH', getN_ts, n, ms))
        for i in range(n):
            arr.append(self.read_time())
        usbd.write(struct.pack('<B', getN_stop))
        arr = np.array(arr)
        dt = timedelta(seconds=arr.mean())
        print("电脑时间:", ts.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3])
        print("RTC 时间:", (ts+dt).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3])
        print("  偏差  : {:.6f}±{:.6f}s".format(arr.mean(), arr.std()))
        return arr

    def update_CNT(self):
        ts = time.time()
        cnts = int(ts)+1
        cnt = cnts - TIMESTAMP_BIAS
        self.usbd.write(struct.pack('<BI', prep_CNT, cnt))
        pack2 = struct.pack('<B', set_prep)
        time.sleep(1 - ts%1)
        self.usbd.write(pack2)
        print("已设置RTC时间", datetime.fromtimestamp(cnts).strftime("%Y-%m-%d %H:%M:%S"))
        return cnt

    def set_clock_cfg(self, psc, cal):
        self.usbd.write(struct.pack('<BIB', set_clock, psc, cal))

    def get_clock_cfg(self):
        self.usbd.write(struct.pack('<B', get_clock))
        pack = self.usbd.read(1)
        cal = struct.unpack('<B', pack)[0]
        return cal

    def exit(self):
        self.usbd.write(struct.pack('<B', rtc_exit))

    @staticmethod
    def _param2freq(psc, cal):
        return (psc+1)/(1-cal/2**20)

    @classmethod
    def _freq2param(cls, freq):
        psc = int(freq)-1
        cal = 2**20*(1-(psc+1)/freq)
        cal = round(cal)
        return psc, int(cal), cls._param2freq(psc, cal)-freq

    def update_clock_cfg2(self, l_cnt, psc0, cal0, t1, dt_mean, dt_std):
        def fast_slow_chinese(x):
            return '快' if x >=0 else '慢'
        # TODO: check psc0, cal0 is change?
        f0 = self._param2freq(psc0, cal0)
        t0 = l_cnt + TIMESTAMP_BIAS
        pc_sec = t1 - t0
        rtc_sec = dt_mean + pc_sec
        ppm_sec = (rtc_sec/pc_sec - 1) * 1e6
        sec_day = (rtc_sec/pc_sec - 1) * 86400
        pulse = f0 * rtc_sec
        f1 = pulse / pc_sec
        f1_std = dt_std * f0 / pc_sec
        f1_ppm = f1_std / f1 * 1e6
        print("==============================")
        print("上次校准:", datetime.fromtimestamp(t0).strftime("%Y-%m-%d %H:%M:%S"))
        print("RTC走了{}, 电脑走了{}".format(str(timedelta(seconds=rtc_sec))[:-3], str(timedelta(seconds=pc_sec))[:-3]))
        print("走时偏差:{:.2f}ppm, 平均每天{}{:.3f}s".format(ppm_sec, fast_slow_chinese(sec_day), abs(sec_day)))
        print("RTC的原设置: PSC={}, CR={}, F={:.3f}Hz".format(psc0, cal0, f0))
        print("测量到RTC频率={:.3f}±{:.4f}Hz, 误差{:.2f}ppm".format(f1, f1_std, f1_ppm))
        print("==============================")
        if f1_ppm > 1:
            if input("测量误差太大了, 是否要校准频率[y/n]") != 'y':
                return psc0, cal0
        if pc_sec < 3600:
            if input("距离上次校准时间太短了, 只有{:.1f}秒, 是否要校准频率[y/n]".format(pc_sec)) != 'y':
                return psc0, cal0
        psc1, cal1, fe = self._freq2param(f1)
        self.set_clock_cfg(psc1, cal1)
        print("已校准频率 PSC={}, CR={}, f_err={:.3f}".format(psc1, cal1, fe))
        return psc1, cal1

    def sync_and_db(self):
        cur = self.conn.cursor()
        sql = 'CREATE TABLE IF NOT exists sync_log('\
              'id INTEGER PRIMARY KEY AUTOINCREMENT,'\
              'dev_name TEXT, run_ts REAL,'\
              'N_read INT, send_cnt INT,'\
              'mean1 REAL, std1 REAL, range1 REAL,'\
              'mean2 REAL, std2 REAL, range2 REAL,'\
              'prescale INT, calib INT)'
        cur.execute(sql)
        sql = 'SELECT max(id), send_cnt, prescale, calib FROM sync_log'
        row = cur.execute(sql)
        t1 = time.time()
        arr1 = self.Nread_time()
        mean1, std1 = arr1.mean(), arr1.std()
        range1 = arr1.max() - arr1.min()
        row = list(row)[0]
        if any(row):
            _, l_cnt, psc0, cal0 = row
            psc1, cal1 = self.update_clock_cfg2(l_cnt, psc0, cal0, t1, mean1, std1)
        else:
            psc1, cal1 = 32767, self.get_clock_cfg()
        send_cnt = self.update_CNT()
        arr2 = self.Nread_time()
        mean2, std2 = arr2.mean(), arr2.std()
        range2 = arr2.max() - arr2.min()
        sql = 'INSERT INTO sync_log (dev_name, run_ts, N_read, send_cnt,'\
              'mean1, std1, range1, mean2, std2, range2, prescale, calib) '\
              'VALUES(?,?,?,?,?,?,?,?,?,?,?,?)'
        cur.execute(sql, [self.name, t1, N_READ, send_cnt,
                          mean1, std1, range1, mean2, std2, range2, psc1, cal1])
        self.conn.commit()


usbd = USBDevice(idVendor=0xffff, idProduct=0xf103)
conn = sqlite3.connect("log.db")
rtc = RTC(usbd, 'stm32f103 myboard', conn)
rtc.sync_and_db()
rtc.exit()
