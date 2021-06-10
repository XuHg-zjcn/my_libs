#!/bin/python
import math
import time
from datetime import datetime, timedelta
import struct
import sqlite3

import numpy as np

from usbd import USBDevice

N_READ = 25
N_READ_MS = 50
TIMESTAMP_BIAS = 946684800
FREQ_STAND = 32768
WARING_PPM = 200
READ_DT_BIAS = 0.005

pack_ts = 0x00
getN_ts = 0x01
getN_stop = 0x02
get_calib = 0x03
set_calib = 0x04
set_clock = 0x05
prep_CNT = 0x06
set_prep = 0x07
rtc_exit = 0x08



class RTC:
    """
    使用注意事项:
    为了提高时间精度，使用前建议关闭占用资源大的软件。
    插入USB第一次运行，可能会误差较大，建议先用Ctrl+C结束程序，再次运行本程序。
    不要把开发板插到USB Hub上，建议直接接电脑USB口，通信延时可能会增加误差。
    不要用劣质数据线，数据包可能会重发，造成延时。
    使用中断传输模式
    """
    def __init__(self, usbd, name='rtc0', conn=None):
        self.usbd = usbd
        self.name = name
        self.conn = conn
        self.prl = FREQ_STAND-1

    def byte6_to_ts(self, x):
        div, cnt = struct.unpack('<HI', x)
        return cnt + (self.prl-div)/(self.prl+1) + TIMESTAMP_BIAS

    def read_time(self):
        # MCU向PC发送RTC时间戳
        rtc1 = self.usbd.read(6)
        pc1 = time.time()
        rtc1 = self.byte6_to_ts(rtc1)
        # MCU收到PC数据包时记录的RTC间戳
        pc2 = time.time()
        usbd.write('\x00')
        rtc2 = self.usbd.read(6)
        rtc2 = self.byte6_to_ts(rtc2)
        return rtc1-pc1, rtc2-pc2

    def Nread_time(self, n=N_READ, ms=N_READ_MS):
        arr = []
        ts = datetime.now()
        usbd.write(struct.pack('<BHH', getN_ts, n, ms))
        for i in range(n):
            arr.append(self.read_time())
        usbd.write(struct.pack('<B', getN_stop))
        arr = np.array(arr)
        dt = timedelta(seconds=arr.mean())
        mean = arr.mean() + READ_DT_BIAS
        err = arr.std()/math.sqrt(n)
        mrang = arr.max() - arr.min()
        laty = (arr[1]-arr[0])*1000
        m_laty = laty.mean()
        s_laty = laty.std()
        print("电脑时间:", ts.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3])
        print("RTC 时间:", (ts+dt).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3])
        print("  偏差  : {:.6f}±{:.6f}s".format(mean, err))
        print("通信延时: {:.4f}±{:.4f}ms".format(m_laty, s_laty))
        return mean, err, mrang, m_laty

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

    def set_calib_cfg(self, cal):
        self.usbd.write(struct.pack('<BI', set_calib, cal))

    def set_clock_cfg(self, psc, cal):
        self.usbd.write(struct.pack('<BIB', set_clock, psc, cal))

    def get_calib_cfg(self):
        self.usbd.write(struct.pack('<B', get_calib))
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

    def update_clock_cfg2(self, l_cnt, psc0, cal0, t1, dt_mean, dt_err):
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
        f1_err = dt_err * f0 / pc_sec
        f1_ppm = f1_err / f1 * 1e6
        print("==============================")
        print("上次校准:", datetime.fromtimestamp(t0).strftime("%Y-%m-%d %H:%M:%S"))
        print("RTC走了{}, 电脑走了{}".format(str(timedelta(seconds=rtc_sec))[:-3], str(timedelta(seconds=pc_sec))[:-3]))
        print("走时偏差:{:.2f}ppm, 平均每天{}{:.3f}s".format(ppm_sec, fast_slow_chinese(sec_day), abs(sec_day)))
        print("RTC的原设置: PSC={}, CR={}, F={:.3f}Hz".format(psc0, cal0, f0))
        print("测量到RTC频率={:.3f}±{:.4f}Hz, 误差{:.2f}ppm".format(f1, f1_err, f1_ppm))
        print("==============================")
        if f1_ppm > 1:
            ch = input("测量误差太大了, 是否要校准频率[y/n/e]")
            if ch == 'n':
                return psc0, cal0
            if ch == 'e':
                f1 = float(input('请输入自定义频率(Hz):'))
        if pc_sec < 3600:
            ch = input("距离上次校准时间太短了, 只有{:.1f}秒, 是否要校准频率[y/n/e]".format(pc_sec))
            if ch == 'n':
                return psc0, cal0
            if ch == 'e':
                f1 = float(input('请输入自定义频率(Hz):'))
        stand_ppm = (f1-FREQ_STAND)/FREQ_STAND*1e6
        if abs(stand_ppm) > WARING_PPM:
            print("警告: 与标准值({}Hz)偏差太大, 是否继续".format(FREQ_STAND))
        psc1, cal1, fe = self._freq2param(f1)
        #if psc0 == psc1:
        #    self.set_calib_cfg(cal1)
        #else:
        self.set_clock_cfg(psc1, cal1)
        self.prl = psc1
        print("已校准频率 PSC={}, CR={}, f_err={:.4f}Hz".format(psc1, cal1, fe))
        print("==============================")
        return psc1, cal1

    def sync_and_db(self):
        cur = self.conn.cursor()
        sql = 'CREATE TABLE IF NOT exists sync_log('\
              'id INTEGER PRIMARY KEY AUTOINCREMENT,'\
              'dev_name TEXT, run_ts REAL,'\
              'N_read INT, send_cnt INT,'\
              'mean1 REAL, std1 REAL, range1 REAL, latency1 REAL,'\
              'mean2 REAL, std2 REAL, range2 REAL, latency2 REAL,'\
              'prescale INT, calib INT)'
        cur.execute(sql)
        sql = 'SELECT max(id), send_cnt, prescale, calib FROM sync_log'
        row = cur.execute(sql)
        row = list(row)[0]
        if any(row):
            _, l_cnt, psc0, cal0 = row
            self.prl = psc0
        else:
            psc1, cal1 = FREQ_STAND-1, self.get_calib_cfg()
        t1 = time.time()
        mean1, err1, range1, laty1 = self.Nread_time()
        if any(row):
            psc1, cal1 = self.update_clock_cfg2(l_cnt, psc0, cal0, t1, mean1, err1)
        send_cnt = self.update_CNT()
        mean2, err2, range2, laty2 = self.Nread_time()
        sql = 'INSERT INTO sync_log (dev_name, run_ts, N_read, send_cnt,'\
              'mean1, std1, range1, latency1, mean2, std2, range2, latency2, prescale, calib) '\
              'VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)'
        cur.execute(sql, [self.name, t1, N_READ, send_cnt,
                          mean1, err1, range1, laty1, mean2, err2, range2, laty2, psc1, cal1])
        self.conn.commit()


usbd = USBDevice(idVendor=0xffff, idProduct=0xf103)
conn = sqlite3.connect("log.db")
rtc = RTC(usbd, 'stm32f103 myboard', conn)
rtc.sync_and_db()
rtc.exit()
