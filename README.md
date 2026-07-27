Setup workspace:
>west init -l manifest
>west update

Build application and samples:

in application
>west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE=/workspaces/stm32h723/scripts/debug/nucleo_h823zg.conf

in zephyr/samples/ directory
>west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE=/workspaces/stm32h723/scripts/debug/nucleo_h823zg.conf

in zephyr/samples/net/dhcpv4_client
>west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE="overlay-dhcpv4.conf;/workspaces/stm32h723/scripts/debug/gdb_support.conf;/workspaces/stm32h723/scripts/debug/nucleo_h823zg.conf"

in zephyr/samples/net/socket/http_server:
>west build -p -b nucleo_h723zg -- -DEXTRA_CONF_FILE="overlay-dhcpv4.conf;/workspaces/stm32h723/scripts/debug/gdb_support.conf;/workspaces/stm32h723/scripts/debug/nucleo_h823zg.conf"

 Launch openocd (from https://github.com/xpack-dev-tools/openocd-xpack/releases/tag/v0.12.0-7)
 PS C:\Users\konta> openocd.exe  -f \\wsl$\Ubuntu\home\stefan\Projects\stm32h723\scripts\debug\stm32h723zg_openocd.cfg
