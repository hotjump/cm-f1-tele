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

fig, ax = plt.subplots(figsize=(24,12))


ax.xaxis.set_major_locator(plt.MultipleLocator(1))
ax.yaxis.set_major_locator(plt.MultipleLocator(1))

def lap1_rank_and_name(value, tick_number):
    mins = int(int(value)/60)
    second = int(value) - 60*int(mins)
    return "%d:%02d" % (mins, second)

ax.yaxis.set_major_formatter(plt.FuncFormatter(lap1_rank_and_name))

ax.tick_params(axis="x", bottom=True, top=True, labelbottom=True, labeltop=True, pad = 2)

# ax_2 = ax.twinx()
# ax_2.set_ylim(21, 0)
# ax_2.yaxis.set_major_locator(plt.MultipleLocator(1))
# ax_2.yaxis.set_major_formatter(plt.FuncFormatter(fianl_rank_and_name))

# ax.tick_params(axis="y", left=True, right=True, labelleft=True, labelright=True)

ax.set_title("Rank", loc="left")
ax.set_xlabel("Lap Number")

for i in range(1, 4+1):
    sql = """SELECT teamName as Team, participants.name as driver, lapNum, carPosition, sector1TimeInStr as S1, sector2TimeInStr as S2, sector3TimeInStr as S3, totalTimeInStr as totalTime, totalTime, driverStatusChar as driverStatus 
    FROM participants_frame INNER JOIN participants ON participants.sessionUID=participants_frame.sessionUID AND participants.frameIdentifier=participants_frame.frameIdentifier 
    INNER JOIN LapDetail_frame ON LapDetail_frame.sessionUID = participants_frame.sessionUID INNER JOIN LapDetail ON LapDetail.sessionUID=LapDetail_frame.sessionUID AND LapDetail.carIndex= participants.driver_index 
    AND LapDetail.carIndex = (SELECT carIndex FROM LapDetail_frame INNER JOIN LapDetail ON LapDetail.sessionUID=LapDetail_frame.sessionUID AND LapDetail.frameIdentifier=LapDetail_frame.frameIdentifier AND carPosition = %d)"""

    # 使用 execute()  方法执行 SQL 查询 
    affected = cursor.execute(sql % i)
    laps = []
    rank = []
    laptimes = []
    name = ""

    for j in range(1, affected):
        data = cursor.fetchone()
        name = data[1]
        lap = data[2]
        cur_rank = data[3]
        laps.append(lap)
        rank.append(cur_rank)
        laptimes.append(data[8])

    ax.plot(laps, laptimes, label=("%d: %s" % (i, name)), linewidth=2)

plt.grid(True)
ax.legend(frameon=False)

plt.rcParams['savefig.dpi'] = 300
plt.savefig('./laptime.png', format='png')

# 关闭数据库连接
db.close()