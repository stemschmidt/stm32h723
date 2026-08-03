Apply patch in zephyr:
patch -p1 < ../scripts/solution/accel-websocket.patch

Build modified sample in zephyr/samples/net/socket/http_server:

west build -p -b nucleo_h723zg -- -DEXTRA_DTC_OVERLAY_FILE=/workspaces/stm32h723/scripts/overlay/adxl345.overlay -DEXTRA_CONF_FILE="/workspaces/stm32h723/scripts/overlay/adxl345.conf;overlay-dhcpv4.conf;/workspaces/stm32h723/scripts/debug/gdb_support.conf;/workspaces/stm32h723/scripts/debug/nucleo_h723zg.conf"