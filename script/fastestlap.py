#!/opt/rh/rh-python38/root/usr/bin/python

import pymysql

from matplotlib import pyplot as plt

# 打开数据库连接
db = pymysql.connect(host='127.1',
                     user='telemetry',
                     port=12306,
                     password='123456',
                     database='f1_2020')
 
# 使用 cursor() 方法创建一个游标对象 cursor
cursor = db.cursor()

sql = """select position as '位置', teamName as '车队', driverName as '车手', 
gridPosition as '排位', cast(position as signed) - cast(gridPosition as signed) as '位置变化', numPitStops as '进站次数',  tyreStintsVisual as '轮胎策略',
bestLapTimeStr as '最速圈', bestLapTime, numLaps as '圈数', totalRaceTimeStr as '时间', penaltiesTime as '罚时', numPenalties as '罚时次数',  points as '得分'
from FinalClassification INNER JOIN FinalClassification_frame ON FinalClassification.sessionUID=FinalClassification_frame.sessionUID AND FinalClassification.frameIdentifier=FinalClassification_frame.frameIdentifier order by bestLapTime"""

fig, ax = plt.subplots(figsize=(24,12))
plt.ylim(21, 0)

ax.xaxis.set_major_locator(plt.MultipleLocator(1))
ax.yaxis.set_major_locator(plt.MultipleLocator(1))

def lap_time(value, tick_number):
    mins = int(int(value)/60)
    second = int(value) - 60*int(mins)
    return "%d:%02d" % (mins, second)

ax.xaxis.set_major_formatter(plt.FuncFormatter(lap_time))

# 使用 execute()  方法执行 SQL 查询 
affected = cursor.execute(sql)

rank = []
best_lap_str = []
best_lap = []
name = []

for i in range(1, affected+1):
    data = cursor.fetchone()
    rank.append(i)
    name.append(data[2])
    best_lap_str.append(data[7])
    best_lap.append(data[8])

def rank_and_name(value, tick_number):
    if tick_number > 1 and tick_number < len(name)+2:
        return "%s  %d" % (name[tick_number-2], tick_number-1)
    else:
        return "%d" % (tick_number-1)

ax.yaxis.set_major_formatter(plt.FuncFormatter(rank_and_name))

ax.set_title("Fastest Lap In Race", loc="left")
p1 = ax.barh(rank, best_lap)
plt.bar_label(p1, fmt ="%g", labels=best_lap_str, label_type='edge')

plt.xlim(xmin=best_lap[0])
plt.grid(True)
ax.legend(frameon=False)
plt.rcParams['savefig.dpi'] = 300
plt.savefig('./fastestlap.png', format='png')

# 关闭数据库连接
db.close()