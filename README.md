Initial test of development setup
To verify that the docker container runs and has access to the debugger on the host:
1. Launch Ubuntu via wsl2
2. Move to the directory in which you want to place the repository
3. >git clone git@github.com:stemschmidt/stm32h723.git
4. >cd stm32h723
5. >code .
6. Wait for VSCode to start and select "Reopen in Container" in popup, wait for everything to settle
7. Select "Reload Window" if popup appears
8. Connect nucleo-h723zg board to USB port of the host
9. Launch openocd with stm32h723zg_openocd.cfg (see below: "Launch openocd")
10. In VSCode select "Run and Debug" on the left (or Ctrl + Shift + D)
11. Select "Test environment" from the launch drop-down menu
12. Select "Start Debugging" (or F5)
13. Select "Continue" (or F5)
14. Verify that LED1 is blinking with 0.5 Hz

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
