#!/bin/sh -e

echo "updating\n"
git submodule update --init --recursive

echo "building and installing WiringPi\n"

cd WiringPiLibrary/WiringPi
./build

echo "======================================================\n"
gpio readall
echo "======================================================\n"

cd ../../programs

mkdir -p build
cd build
cmake ..
cmake --build .
if ! sudo make install; then
    echo "Error: make install failed!" >&2
    exit 1
fi


# Define cron jobs
CRONJOBS=(
  "*/5 * * * * /usr/local/bin/statusLED"
  "0 3 * * * apt update && apt upgrade -y && apt autoclean && apt autoremove -y"
  "0 3 */14 * * find /var/log -type f -mtime +30 -exec rm -f {} \; && rm -rf /tmp/* /var/tmp/* && journalctl --vacuum-time=30d && apt autoremove --purge -y && apt clean && rm -rf ~/.cache/*"
)

# Get current crontab
CURRENT_CRONTAB=$(sudo crontab -l 2>/dev/null || true)

# Loop through cron jobs
for CRONJOB in "${CRONJOBS[@]}"; do
  if ! echo "$CURRENT_CRONTAB" | grep -Fq "$CRONJOB"; then
    echo "Adding cronjob: $CRONJOB"
    (echo "$CURRENT_CRONTAB"; echo "$CRONJOB") | sudo crontab -
    CURRENT_CRONTAB=$(sudo crontab -l 2>/dev/null || true)  # Update current crontab
  else
    echo "Cronjob already exists: $CRONJOB"
  fi
done

# Check if shutdownButton is already set to run on startup
SERVICE_FILE="/etc/systemd/system/shutdownButton.service"

if [ ! -f "$SERVICE_FILE" ]; then
    echo "Creating systemd service for shutdownButton."
    sudo tee "$SERVICE_FILE" > /dev/null <<EOF
[Unit]
Description=Shutdown Button Service
After=multi-user.target

[Service]
ExecStart=/usr/local/bin/shutdownButton
Restart=always
User=root

[Install]
WantedBy=multi-user.target
EOF

    sudo systemctl daemon-reload
    sudo systemctl enable shutdownButton
    echo "ShutdownButton will now run on startup."

    # Ask user if they want to reboot
    read -p "Do you want to reboot now? (y/N) " REBOOT
    if [ "$REBOOT" = "y" ] || [ "$REBOOT" = "Y" ]; then
        sudo reboot
    fi
else
    echo "ShutdownButton service is already installed."
fi
