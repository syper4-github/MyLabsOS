function Fmul {
ans=$(($1 * $2))
echo $ans
}
while :
do
echo $(date +'%M') > time_data
if read encoder; then
	time_now < time_data 
	echo $(Fmul $encoder $time_now)
fi
done
