application
 west build -p -b nucleo_h723zg

zephyr/samples/net/dhcpv4_client
 west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE="overlay-dhcpv4.conf;/workspaces/stm32h723/scripts/debug/gdb_support.conf"

zephyr/samples/net/socket/http_server:
 west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE="overlay-dhcpv4.conf;/workspaces/stm32h723/scripts/debug/gdb_support.conf"

 Launch openocd (from https://github.com/xpack-dev-tools/openocd-xpack/releases/tag/v0.12.0-7)
 PS C:\Users\konta> openocd.exe  -f \\wsl$\Ubuntu\home\stefan\Projects\stm32h723\scripts\debug\stm32h723zg_openocd.cfg
