#!/bin/bash


# --- Service Test ---
# systemctl list-units --type=service --state=running
# systemctl status MRPI250610.service 
# pstree | grep mydaemon


# --- Stop & Disable & Remove service ----
# Lelltija:                      systemctl stop MRPI250610.service
# Kvetekzo bootkor nem indul el: systemctl disable MRPI250610.service
# rm /etc/systemd/system/MRPI250610.service
# rm /usr/local/bin/MRPI250610


# --- syslog ---
# 
# journalctl: syslog megjelenítése
#
# -f: real-time 
# -u: filter 
# journalctl -u MRPI250610 -f
# journalctl -f | grep "MRPI250610"

echo "Service Install Started..."

# Bináris fájl neve (ne legyen elérési út!)
BINARY_NAME="MRPI250610"
BINARY_SOURCE="./$BINARY_NAME"
BINARY_TARGET="/usr/local/bin/$BINARY_NAME"
SERVICE_FILE="/etc/systemd/system/$BINARY_NAME.service"

# Ellenőrizzük, hogy létezik-e a bináris fájl
if [[ ! -f "$BINARY_SOURCE" ]]; then
    echo "❌ Hiba: A $BINARY_SOURCE nem található."
    exit 1
fi

# Másolás a rendszer bináris könyvtárába
echo "📁 Bináris másolása: $BINARY_SOURCE -> $BINARY_TARGET"
sudo cp "$BINARY_SOURCE" "$BINARY_TARGET"
sudo chmod +x "$BINARY_TARGET"

# --- Jogostultások---
# User=nobody
# Group=nogroup

# --- Elérhető UART ---
# dmesg | grep tty
# Ez megmutatja, hogy mely soros eszközök jelentek meg, pl: ttyUSB0

# --- UART jogosultság használata ---
# csak az a user hasnzálhatja az UART-ot aki a dialout csoport tagja
# igy ellenőrízehted: getent group dialout
# igy adahatod hozzá: sudo usermod -a -G dialout marrob
# sudo gpasswd -d marrob dialout
if [ -z "$1" ]; then
    echo "Használat: sudo $0 <felhasználónév>"
    exit 1
fi

CURRENT_USER="$1"

# Hozzáadás a 'dialout' csoporthoz
echo "Adding user '$CURRENT_USER' to 'dialout' group..."
sudo usermod -a -G dialout "$CURRENT_USER"


# --- Systemd service fájl létrehozása ---
# Ez a probléma jól mutatja, mennyire fontos a systemd megfelelő konfigurálása, amikor egy háttérfolyamatot (démont) kezelünk:
# Démonizálás C-ben: A fork() és setsid() hívásokkal a programod leválik a terminálról, és háttérben fut.
# systemd és a processztípusok: A systemd-nek tudnia kell, hogyan viselkedik az ExecStart parancs által indított folyamat.
#    Type=simple (alapértelmezett): A systemd feltételezi, hogy a folyamat a ExecStart indítása után azonnal fut és készen áll.
#    Type=forking (amit most használsz): A systemd megvárja, amíg az ExecStart parancs által elindított folyamat fork()-ol, a 
#    szülőfolyamat kilép, és a gyermekfolyamat marad futni. Ez a helyes típus a legtöbb C-ben írt, klasszikusan démonizált programhoz.

echo "⚙️ Systemd service fájl létrehozása: $SERVICE_FILE"
sudo bash -c "cat > $SERVICE_FILE" <<EOF
[Unit]
Description=$BINARY_TARGET service for proper shutdown...
After=network.target

[Service]
Type=forking
ExecStart=$BINARY_TARGET
Restart=always
User=$CURRENT_USER
Group=dialout 

[Install]
WantedBy=multi-user.target
EOF

# Systemd újratöltése és szolgáltatás engedélyezése
echo "🔄 Systemd újratöltése..."
sudo systemctl daemon-reload

echo "✅ Szolgáltatás engedélyezése boot-kor..."
sudo systemctl enable "$BINARY_NAME.service"

echo "🚀 Szolgáltatás indítása..."
sudo systemctl start "$BINARY_NAME.service"

# Ellenőrzés
echo "📋 Státusz:"
sudo systemctl status "$BINARY_NAME.service" --no-pager