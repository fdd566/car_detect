if [ ! $1 ]; then
	echo "$1 is null"
	exit
fi
rm run.tar.gz
curl $1 -u controller:Stevia1016 -o run.tar.gz
echo "curl : '$?'"
if [ X"$?" != X"0" ]; then
	echo 'curl error'
	exit
fi

program='re.sh'
sn=`ps | grep $program | grep -v grep |awk '{print $1}'`
kill $sn

program='test'
sn=`ps | grep $program | grep -v grep |awk '{print $1}'`
kill $sn

rm run -rf
tar zxvf run.tar.gz
cd run
cp ../sn.txt .
./re.sh &
echo 'update over'
