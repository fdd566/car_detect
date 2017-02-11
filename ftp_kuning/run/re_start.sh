program='re.sh'
sn=`ps | grep $program | grep -v grep |awk '{print $1}'`
kill $sn

program='test'
sn=`ps | grep $program | grep -v grep |awk '{print $1}'`
kill $sn

./re.sh &
echo 'start ok'
