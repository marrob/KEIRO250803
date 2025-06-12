#!/bin/bash

# 1. Modify cmdline.txt - remove serial console output
echo "[1/3] Modifying cmdline.txt..."
sudo sed -i 's/console=serial0,[0-9]* *//g' /boot/cmdline.txt

# 2. Modify config.txt - ensure UART is enabled
echo "[2/3] Setting enable_uart=1 in config.txt..."
if grep -q "^enable_uart=" /boot/config.txt; then
    # Replace existing line
    sudo sed -i 's/^enable_uart=.*/enable_uart=1/' /boot/config.txt
else
    # Add it if not present
    echo "enable_uart=1" | sudo tee -a /boot/config.txt >/dev/null
fi

# 3. Disable serial-getty services - prevent login shell on UART
echo "[3/3] Disabling serial-getty services..."
sudo systemctl disable serial-getty@serial0.service 2>/dev/null
sudo systemctl disable serial-getty@ttyAMA0.service 2>/dev/null
sudo systemctl disable serial-getty@ttyS0.service 2>/dev/null

# Done - suggest reboot
echo "? UART configuration complete. Please reboot the Raspberry Pi:"
echo "    sudo reboot"