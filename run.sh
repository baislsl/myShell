echo $@
echo $0
shift
echo $0
echo $PATH
test -f makefile
echo $?
test gg = hh
echo $?
test gg = hh -a -f makefile
echo $?
ls -l | wc -l
ls -al | grep c
cd ..
pwd
dir
cd
pwd
find -name *.c | wc > tmp
ls | cat >> tmp
cat tmp
export a1=1
export a2=2
export a3=3g
unset a2
sleep 10
set | grep ^a
fg
