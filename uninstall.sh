#!/bin/bash

# Uninstall script for KEIRO250803

BINARY_NAME="KEIRO250803"
BINARY_TARGET="/usr/local/bin/$BINARY_NAME"
SERVICE_FILE="/etc/systemd/system/$BINARY_NAME.service"

echo ">>> Uninstall started for $BINARY_NAME..."

# Ha fut, allitsuk le a szolgoltatast
if systemctl is-active --quiet "$BINARY_NAME.service"; then
    echo ">>> Stopping service..."
    sudo systemctl stop "$BINARY_NAME.service"
fi

# Ha enged�lyezve van, tiltsuk le
if systemctl is-enabled --quiet "$BINARY_NAME.service"; then
    echo ">>> Disabling service..."
    sudo systemctl disable "$BINARY_NAME.service"
fi

# Service fajl torlese
if [ -f "$SERVICE_FILE" ]; then
    echo ">>> Removing service file: $SERVICE_FILE"
    sudo rm -f "$SERVICE_FILE"
fi

# Biniris torlese
if [ -f "$BINARY_TARGET" ]; then
    echo ">>> Removing binary: $BINARY_TARGET"
    sudo rm -f "$BINARY_TARGET"
fi

# Systemd frissitese
echo ">>> Reloading systemd..."
sudo systemctl daemon-reload

echo ">>> Uninstall completed."