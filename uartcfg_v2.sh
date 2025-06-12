#!/bin/bash

# 1. Modify cmdline.txt – remove serial console output
echo "[1/3] Modifying cmdline.txt..."
sudo sed -i 's/console=serial0,[0-9]* *//g' /boot/cmdline.txt || {
    echo "❌ Failed to update /boot/cmdline.txt" >&2
    exit 1
}

# 2. Modify config.txt – ensure UART is enabled
echo "[2/3] Setting enable_uart=1 in config.txt..."
if grep -q "^enable_uart=" /boot/config.txt; then
    sudo sed -i 's/^enable_uart=.*/enable_uart=1/' /boot/config.txt || {
        echo "❌ Failed to update enable_uart in /boot/config.txt" >&2
        exit 1
    }
else
    echo "enable_uart=1" | sudo tee -a /boot/config.txt >/dev/null || {
        echo "❌ Failed to append enable_uart=1 to /boot/config.txt" >&2
        exit 1
    }
fi

# 3. Disable serial-getty services – prevent login shell on UART
echo "[3/3] Disabling serial-getty services..."

if ! sudo systemctl disable serial-getty@serial0.service 2>&1; then
    echo "⚠️ serial-getty@serial0.service could not be disabled or does not exist." >&2
fi

if ! sudo systemctl disable serial-getty@ttyAMA0.service 2>&1; then
    echo "⚠️ serial-getty@ttyAMA0.service could not be disabled or does not exist." >&2
fi

if ! sudo systemctl disable serial-getty@ttyS0.service 2>&1; then
    echo "⚠️ serial-getty@ttyS0.service could not be disabled or does not exist." >&2
fi

# Done – suggest reboot
echo "✅ UART configuration complete. Please reboot the Raspberry Pi:"
echo "    sudo reboot"