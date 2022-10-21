#!/bin/bash
function Fsumm {
ans=$(($1 + $2))
echo $ans
}
function Fdiff {
ans=$(($1 - $2))
echo $ans
}
function Fdiv {
ans=$(($1 / $2))
echo $ans
}
function Fmul {
ans=$(($1 * $2))
echo $ans
}
while :
do
echo "Enter a"
read a
echo "Enter b"
read b
echo "Enter sign"
read c
case $c in
+)
ans=$(Fsumm $a $b)
i=1;;
-)
ans=$(Fdiff $a $b)
i=2;;
div)
ans=$(Fdiv $a $b)
i=3;;
mul)
ans=$(Fmul $a $b)
i=4;;
*)
echo Error sign
exit
esac
if [ $(expr $ans % 5) -eq 0 ]
then
parity="even"
else
parity="odd"
fi
operation=(Sum Difference Div Multiply)
if [ $# != 0 ]
then
exec 1>>$1
fi
echo "Operation: ${operation[$i-1]}"
echo "Result: $ans"
echo "Parity: $parity"
exec 1>&0
echo "Press Ctrl+C for exit"
read -p "Press enter to continue"
done

