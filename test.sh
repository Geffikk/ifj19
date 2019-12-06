#!/bin/bash

if [$# -eq 0]; then
	travis_build=0
else
	travis_build=$1
fi

make clean && make

red='\033[0;31m'
green='\033[0;32m'
nc='\033[0m'
black='\e[30m'

testsbad=0
testpercent=0
testgood=0

echo -e "${nc}======================== [ Tests starts ] ============================"
echo""

for file in ./tests/*; do
        let testpercent++
	echo -e "${nc}-------------------[ Test $file ]------------------------"
	echo -e "${nc}                                |"
	echo -e "${nc}                                V"
	./ifj19 < ${file} > temp_out
	if [ $? -eq 0 ]; then
		let testgood++
		echo -e "                          \e[30;48;5;82m TEST PASSED \e[0m"
		echo ""                      
	else
		echo -e "                          \e[30;48;5;196m TEST FAILED \e[0m"
		echo ""		
		let testsbad++
	fi
done

echo ""
echo -e "${nc}------------------------- TEST RESULT ------------------------------"
echo ""
final=$(($testgood*100))
final2=$(($final/$testpercent))
echo -e "${nc} Test success - "$final2"%"
echo ""
if [ $testsbad -gt 0 ]; then
	echo -e "${red}                        Tests failed: "$testsbad""
	echo ""
	echo -e "${nc}--------------------------------------------------------------------"
	exit 1
else
	echo -e "${green}                    Tests passed (You're pro)"
	echo ""
	echo -e "${nc}--------------------------------------------------------------------"
	exit 0
fi
