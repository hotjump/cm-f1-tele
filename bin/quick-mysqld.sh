#!/bin/bash

set -e

CURRENT_USER=$(whoami)
BASE_DIR=$(mktemp -d /tmp/mysql_temp.XXXXXX)
DATA_DIR="$BASE_DIR/data"
SOCKET_FILE="$BASE_DIR/mysql.sock"
LOG_FILE="$BASE_DIR/mysqld.log"
PID_FILE="$BASE_DIR/mysqld.pid"
PORT=3307

# 新增：定义 telemetry 用户
TELEMETRY_USER="telemetry"
TELEMETRY_PASS="pass@tele"

echo "📁 临时目录: $BASE_DIR"
echo "👤 用户: $CURRENT_USER"

cleanup() {
    echo -e "\n🛑 停止 mysqld..."
    if [ -f "$PID_FILE" ] && kill -0 "$(cat "$PID_FILE")" 2>/dev/null; then
        kill "$(cat "$PID_FILE")"
        sleep 2
    fi
    echo "🧹 清理: $BASE_DIR"
    rm -rf "$BASE_DIR"
    exit 0
}

trap cleanup INT TERM EXIT

mkdir -p "$DATA_DIR"

# 初始化
echo "🔧 初始化数据库..."
mysqld \
  --no-defaults \
  --initialize-insecure \
  --datadir="$DATA_DIR" \
  --user="$CURRENT_USER" \
  >/dev/null 2>&1

# 启动 mysqld
echo "🚀 启动 mysqld（日志将显示在此）..."
mysqld \
  --no-defaults \
  --datadir="$DATA_DIR" \
  --socket="$SOCKET_FILE" \
  --port=$PORT \
  --pid-file="$PID_FILE" \
  --log-error="$LOG_FILE" \
  --bind-address=127.0.0.1 \
  --skip-networking=0 \
  --disable-log-bin \
  --innodb-buffer-pool-size=64M \
  --user="$CURRENT_USER" \
  --mysqlx=OFF \
  --console &
  
MYSQLD_PID=$!

# 等待启动
echo "⏳ 等待 mysqld 就绪（最多 10 秒）..."
for i in {1..20}; do
    if [ ! -d "/proc/$MYSQLD_PID" ]; then
        echo "❌ mysqld 已意外退出！查看上面的错误或日志：$LOG_FILE"
        [ -f "$LOG_FILE" ] && cat "$LOG_FILE"
        exit 1
    fi
    if mysqladmin -S "$SOCKET_FILE" ping >/dev/null 2>&1; then
        break
    fi
    sleep 0.5
done

if ! mysqladmin -S "$SOCKET_FILE" ping >/dev/null 2>&1; then
    echo "❌ 无法连接到 mysqld，可能启动失败。"
    [ -f "$LOG_FILE" ] && cat "$LOG_FILE"
    exit 1
fi

# === 新增：创建 telemetry 用户 ===
echo "🔑 创建用户 '$TELEMETRY_USER' ..."
mysql -u root -S "$SOCKET_FILE" -e "
CREATE USER IF NOT EXISTS '$TELEMETRY_USER'@'%' IDENTIFIED BY '$TELEMETRY_PASS';
GRANT ALL PRIVILEGES ON *.* TO '$TELEMETRY_USER'@'%';
FLUSH PRIVILEGES;
"

echo "✅ 成功！"
echo "   端口: $PORT"
echo "   Root 用户：无密码（仅本地）"
echo "   Telemetry 用户: $TELEMETRY_USER / 密码: $TELEMETRY_PASS"
echo ""
echo "💡 连接示例："
echo "   mysql -h 127.0.0.1 -P $PORT -u telemetry -p"
echo ""
echo "⏳ 按 Ctrl+C 退出..."

# 等待 mysqld 进程结束
wait $MYSQLD_PID 2>/dev/null || true