import mysql.connector
import time

import win32api
import win32con
import time
import ctypes
from urllib.request import urlopen
import re as r
 
def getIP():
    d = str(urlopen('http://checkip.dyndns.com/').read())
    return r.compile(r'Address: (\d+\.\d+\.\d+\.\d+)').search(d).group(1)
 
mvka = ctypes.windll.user32.MapVirtualKeyA

def press_key(num):
    real_num = 48 + num
    win32api.keybd_event(real_num, mvka(real_num, 0), 0, 0)
    time.sleep(0.02)
    win32api.keybd_event(real_num, mvka(real_num, 0), win32con.KEYEVENTF_KEYUP, 0)

def press_key_with_shift(num):
    real_num = 48 + num
    shift_key = 0x10
    win32api.keybd_event(shift_key, mvka(shift_key, 0), 0, 0)
    win32api.keybd_event(real_num, mvka(real_num, 0), 0, 0)
    time.sleep(0.02)
    win32api.keybd_event(real_num, mvka(real_num, 0), win32con.KEYEVENTF_KEYUP, 0)
    win32api.keybd_event(shift_key, mvka(shift_key, 0), win32con.KEYEVENTF_KEYUP, 0)

def getFocusCar():
    db =  mysql.connector.connect(host = "xxx.xxx.xxx.xxx", port = 12306, user = "xxxxx", password = "xxxxx", database = "f1_2022_tele")
    print(getIP())
    my_ip = getIP()
    while True:
        time.sleep(1)
        db.reconnect()
        cursor = db.cursor()
        cursor.callproc('CarFocusDataJIT', (my_ip,))
        
        #MySQL [f1_2022]> call CarFocusDataJIT();
        #+------+-----------+-----------------+------------+----------------+--------+-----------+---------------+---------------+-----------+--------+------+--------+---------------+--------------+
        #| NO.  | 车手      | 车队            | UNIX       | scenes         | 圈数   | 最快圈    | 差距-头车     | 差距-前车     | 上一圈    | s1     | s2   | 当前   | 轮胎-磨损     | 赛道位置     |
        #+------+-----------+-----------------+------------+----------------+--------+-----------+---------------+---------------+-----------+--------+------+--------+---------------+--------------+
        #|    4 | 佩雷兹    | Red Bull Racing | 1665571846 | battle_in_0_5s |      7 | 1:07.651  | +8.508s       | +0.307s       | 1:07.914  | 16.991 |      | 36.299 | S(6)-36%      | 0.53         |
        #+------+-----------+-----------------+------------+----------------+--------+-----------+---------------+---------------+-----------+--------+------+--------+---------------+--------------+

        cur_time = int(time.time())
        focus_car_pos = 0
        date_time = 0

        for i in cursor.stored_results(): 
            results = i.fetchall()
            for car in results:
                focus_car_pos = int(car[0])
                driver_name = car[1]
                team_name = car[2]
                date_time = int(car[3])
                scenes = car[4]
                print(f'{focus_car_pos:<15}|{driver_name:<15}|{team_name:<15}|{scenes:<15}|{date_time:<15}|{cur_time:<15}')
        
        if abs(cur_time - date_time) < 3 and focus_car_pos > 0:
        #if focus_car_pos > 0:
            ## 按键
            if focus_car_pos < 10 :
                press_key(focus_car_pos%10)
            elif focus_car_pos == 10:
                press_key(0)
            elif focus_car_pos < 20 :
                press_key_with_shift(focus_car_pos%10)
            else :
                press_key_with_shift(0)
        
        cursor.close()
        db.close()

if __name__ == "__main__":
    getFocusCar()