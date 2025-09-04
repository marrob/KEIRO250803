#!/bin/bash


# --- Service Test ---
# systemctl list-units --type=service --state=running
# systemctl status KEIRO250803.service 
# pstree | grep mydaemon

# --- Stop & Disable & Remove service ----
# systemctl stop KEIRO250803.service
# systemctl disable KEIRO250803.service
# rm /etc/systemd/system/KEIRO250803.service
# rm /usr/local/bin/KEIRO250803
# systemctl restart KEIRO250803.service 

# --- RPI 4B az albbiak kikapcsoljk a consolt ---
# systemctl stop serial-getty@ttyS0.service
# systemctl disable serial-getty@ttyS0.service
# sudo systemctl mask serial-getty@ttyS0.service

# --- syslog ---
#
# journalctl: syslog megjelenítése
#
# -f: real-time
# -u: filter
# journalctl -u KEIRO250803 -f
# journalctl -f | grep KEIRO250803


echo "Service Install Started..."

# Check: is the script running as root?
if [[ "$EUID" -ne 0 ]]; then
   echo "❌ Error: This script must be run as root (e.g. sudo $0)."
   exit 1
fi

# Binary file name (no path!)
BINARY_NAME="KEIRO250803"
BINARY_SOURCE="./$BINARY_NAME"
BINARY_TARGET="/usr/local/bin/$BINARY_NAME"
SERVICE_FILE="/etc/systemd/system/$BINARY_NAME.service"

# Verify that the binary exists
if [[ ! -f "$BINARY_SOURCE" ]]; then
    echo "❌ Error: $BINARY_SOURCE not found."
    exit 1
fi

# Copy binary to system directory
echo "📁 Copying binary: $BINARY_SOURCE -> $BINARY_TARGET"
cp "$BINARY_SOURCE" "$BINARY_TARGET"
chmod +x "$BINARY_TARGET"

# --- Create systemd service file ---
echo "⚙️ Creating systemd service file: $SERVICE_FILE"
cat > "$SERVICE_FILE" <<EOF
[Unit]
Description=$BINARY_TARGET service for proper shutdown...
After=network-online.target syslog.target
Wants=network-online.target

[Service]
Type=forking
ExecStart=$BINARY_TARGET
Restart=always
# Will run as root (no User/Group specified)

[Install]
WantedBy=multi-user.target
EOF

# Reload systemd and enable the service
echo "🔄 Reloading systemd..."
systemctl daemon-reload

echo "✅ Enabling service to start on boot..."
systemctl enable "$BINARY_NAME.service"

echo "🚀 Starting service..."
systemctl start "$BINARY_NAME.service"

# Status check
echo "📋 Status:"
systemctl status "$BINARY_NAME.service" --no-pager
