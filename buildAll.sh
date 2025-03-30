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

[ -d build ] || mkdir build
cd build
cmake ..
cmake --build .
if ! sudo make install; then
    echo "Error: make install failed!" >&2
    exit 1
fi


# Check for statusLED cronjob
CRONJOB="*/5 * * * * /usr/local/bin/statusLED"
CRON_EXISTS=$(sudo crontab -l 2>/dev/null | grep -F "$CRONJOB" || true)

if [ -z "$CRON_EXISTS" ]; then
    echo "Adding cronjob to run statusLED every 5 minutes."
    (sudo crontab -l 2>/dev/null; echo "$CRONJOB") | sudo crontab -
else
    echo "Cronjob for statusLED already exists."
fi

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
