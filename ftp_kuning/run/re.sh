i=0
while [ 1 ]
do
    procID=`pgrep test`
    if [ "" == "$procID" ];
    then
        ./test
		 i=`expr $i + 1`
    fi
	if [ "$i" == "100" ];
	then
		reboot
	fi
	echo "re '$i'"
    sleep 1
done &
