#!/bin/bash

#link lib name
LN=treeb

#lib name
L=lib"${LN}"

#target
T1=interface
T2=public
T3=protected
T4=private
T5=knowhow

#destination
D0=./lib/
D1=${D0}test_${T1}/${T1}/
D2=${D1}test_${T2}/${T2}/
D3=${D2}test_${T3}/${T3}/
D4=${D3}test_${T4}/${T4}/
D5=${D4}test_${T5}/${T5}/

mkdir -p ${D5} 

#create lib files
touch ./lib/"${L}".h

echo "#ifndef ${L}" >> ./lib/"${L}".h
echo "#define ${L}" >> ./lib/"${L}".h
echo "" >> ./lib/"${L}".h
echo "#endif" >> ./lib/"${L}".h

#file name
FN1="${T1}".c
FN2="${T2}".c
FN3="${T3}".c
FN4=${T4}.c
FN5=${T5}.c

#full file name
F1="${D1}${FN1}"
F2="${D2}${FN2}"
F3="${D3}${FN3}"
F4="${D4}${FN4}"
F5="${D5}${FN5}"

#create files
touch "${F1}"
touch "${F2}"
touch "${F3}"
touch "${F4}"
touch "${F5}"

#init
echo "#ifndef ${T1}_c" >> "${F1}" 
echo "#define ${T1}_c" >> "${F1}" 
echo "#include <stdlib.h>" >> "${F1}" 
echo "#endif" >> "${F1}" 


echo "#ifndef ${T2}_c" >> "${F2}"
echo "#define ${T2}_c" >> "${F2}"
echo "#include <${FN1}>" >> "${F2}"
echo "#include <stdlib.h>" >> "${F2}"
echo "#endif" >> "${F2}"


echo "#ifndef ${T3}_c" >> "${F3}"
echo "#define ${T3}_c" >> "${F3}"
echo "#include <${FN1}>" >> "${F3}"
echo "#include <${FN2}>" >> "${F3}"
echo "#include <stdlib.h>" >> "${F3}"
echo "#endif" >> "${F3}"


echo "#ifndef ${T4}_c" >> "${F4}"
echo "#define ${T4}_c" >> "${F4}"
echo "#include <${FN1}>" >> "${F4}"
echo "#include <${FN2}>" >> "${F4}"
echo "#include <${FN3}>" >> "${F4}"
echo "#include <stdlib.h>" >> "${F4}"
echo "#endif" >> "${F4}"


echo "#ifndef ${T5}_c" >> "${F5}"
echo "#define ${T5}_c" >> "${F5}"
echo "#include <${FN1}>" >> "${F5}"
echo "#include <${FN2}>" >> "${F5}"
echo "#include <${FN3}>" >> "${F5}"
echo "#include <${FN4}>" >> "${F5}"
echo "#include <stdlib.h>" >> "${F5}"
echo "#endif" >> "${F5}"

#create hard link
ln "${F1}" "${D5}" 
ln "${F2}" "${D5}"
ln "${F3}" "${D5}"
ln "${F4}" "${D5}"

ln "${F5}" "${D0}"

#build file name
BF=build.sh

B1="${D0}${BF}"
B2="${D1}${BF}"
B3="${D2}${BF}"
B4="${D3}${BF}"
B5="${D4}${BF}"
B6="${D5}${BF}"

#script header
I="${D5}"
H=#!/bin/bash

#create build script
echo "${H}" >> "${BF}"
echo "rm ./ok" >> "${BF}"
echo "gcc main.c -o ok -L. -l${LN} -Wl,-rpath,. -I." >> "${BF}"
echo "#include <${L}.h>" >> main.c
echo "#include <stdlib.h>" >> main.c
echo "#include <stdio.h>" >> main.c
echo "" >> main.c
echo "int main(int argc, char** argv)" >> main.c
echo "{" >> main.c
echo "" >> main.c
echo "printf(\"ok\n\");" >> main.c
echo "return 0;" >> main.c
echo "}" >> main.c

I="./test_${T1}/${T1}/test_${T2}/${T2}/test_${T3}/${T3}/test_${T4}/${T4}/test_${T5}/${T5}"
echo "${H}" >> "${B1}"
echo "F=${FN5}" >> "${B1}"
echo "I=${I}" >> "${B1}"
echo "rm ./${L}.so" >> "${B1}"
echo "gcc -shared -o ${L}.so -fPIC \${F} -I\${I}" >> "${B1}"
echo "cp ${L}.so .." >> "${B1}"
echo "cp ${L}.h .." >> "${B1}"

I="./test_${T2}/${T2}/test_${T3}/${T3}/test_${T4}/${T4}/test_${T5}/${T5}"
echo "${H}" >> "${B2}"
echo "F=${FN1}" >> "${B2}"
echo "I=${I}" >> "${B2}"
echo "gcc -c \${F} -I\${I}" >> "${B2}"

I="./test_${T3}/${T3}/test_${T4}/${T4}/test_${T5}/${T5}"
echo "${H}" >> "${B3}"
echo "F=${FN2}" >> "${B3}"
echo "I=${I}" >> "${B3}"
echo "gcc -c \${F} -I\${I}" >> "${B3}"

I="./test_${T4}/${T4}/test_${T5}/${T5}"
echo "${H}" >> "${B4}"
echo "F=${FN3}" >> "${B4}"
echo "I=${I}" >> "${B4}"
echo "gcc -c \${F} -I\${I}" >> "${B4}"

I="./test_${T5}/${T5}"
echo "${H}" >> "${B5}"
echo "F=${FN4}" >> "${B5}"
echo "I=${I}" >> "${B5}"
echo "gcc -c \${F} -I\${I}" >> "${B5}"

I="."
echo "${H}" >> "${B6}"
echo "F=${FN5}" >> "${B6}"
echo "I=${I}" >> "${B6}"
echo "gcc -c \${F} -I\${I}" >> "${B6}"

#set execute flag
chmod +x "${B1}" "${B2}" "${B3}" "${B4}" "${B5}" "${B6}" "${BF}"

echo "*.swp" >> .gitignore
echo "*.o" >> .gitignore
echo "*.so" >> .gitignore
echo "tags" >> .gitignore
