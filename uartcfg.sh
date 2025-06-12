#!/bin/bash

# 1. Modify cmdline.txt - remove serial console output
echo "[1/4] Modifying cmdline.txt..."
sudo sed -i 's/console=serial0,[0-9]* *//g' /boot/cmdline.txt

# 2. Modify config.txt - ensure UART is enabled
echo "[2/4] Setting enable_uart=1 in config.txt..."
if grep -q "^enable_uart=" /boot/config.txt; then
    # Replace existing line
    sudo sed -i 's/^enable_uart=.*/enable_uart=1/' /boot/config.txt
else
    # Add it if not present
    echo "enable_uart=1" | sudo tee -a /boot/config.txt >/dev/null
fi

# 3. Disable serial-getty services - prevent login shell on UART
echo "[3/4] Disabling and stopping serial-getty services..."
# Disable (prevent auto-start on boot)
sudo systemctl disable serial-getty@serial0.service 2>/dev/null
sudo systemctl disable serial-getty@ttyAMA0.service 2>/dev/null
sudo systemctl disable serial-getty@ttyS0.service 2>/dev/null

# Stop (immediately stop if running)
sudo systemctl stop serial-getty@serial0.service 2>/dev/null
sudo systemctl stop serial-getty@ttyAMA0.service 2>/dev/null
sudo systemctl stop serial-getty@ttyS0.service 2>/dev/null

# 4. Optional: Mask serial-getty services for stronger prevention
#    This makes sure nothing else can try to start them.
echo "[4/4] Masking serial-getty services (stronger prevention)..."
sudo systemctl mask serial-getty@serial0.service 2>/dev/null
sudo systemctl mask serial-getty@ttyAMA0.service 2>/dev/null
sudo systemctl mask serial-getty@ttyS0.service 2>/dev/null


# Done - suggest reboot
echo ""
echo "---------------------------------------------------------"
echo "✔️ UART configuration complete. Please reboot your Raspberry Pi."
echo "   A reboot is essential for all changes to take effect."
echo "   Command to reboot:  sudo reboot"
echo "---------------------------------------------------------"