# faddress='-fsanitize=address -fsanitize-recover=all'
# fsan='-lasan'
# main='main'
#lshared=''
set -e
faddress=''
fsan=''
main='main-dlopen'
lshared='-lshared'
rm -f libshared.so
g++  -fPIC -std=c++17 $faddress shared_library.cpp -shared -o libshared.so
rm -f shared.o
g++ -c $faddress -std=c++17 shared_library.cpp  -o shared.o
rm -f ${main}.o
g++ $faddress -std=c++17  -c  ${main}.cpp -o ${main}.o
rm -f ${main}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
g++ -o ${main}  -std=c++17 $fsan -L. ${main}.o shared.o -ldl
LD_DEBUG=symbols,bindings ./${main}  
