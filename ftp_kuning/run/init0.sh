echo 148 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio148/direction
echo 0 > /sys/class/gpio/gpio148/value