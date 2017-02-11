echo 149 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio149/direction
echo 0 > /sys/class/gpio/gpio149/value