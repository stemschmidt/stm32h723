Initial test of development setup
To verify that the docker container runs and has access to the debugger on the host:
1. >git clone git@github.com:stemschmidt/stm32h723.git
2. >cd stm32h723
3. >code .
4. Select "Reopen in Container" in popup, wait for everything to settle
5. Select "Reload Window" if popup appears
6. Connect nucleo-h723zg board to USB port of the host
7. Launch openocd with stm32h723zg_openocd.cfg (see below for wsl2 setup)
8. In VSCode select "Run and Debug" on the left (or Ctrl + Shift + D)
9. Select "Test environment" from the launch drop-down menu
10. Select "Start Debugging" (or F5)
11. Select "Continue" (or F5)
12. Verify that LED1 is blinking with 0.5 Hz

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
