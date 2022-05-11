if ! [ -n "$ARDUINO15_PATH" ]; then
    if [ -d ~/Library/Arduino15 ]; then
        ARDUINO15_PATH=~/Library/Arduino15
    elif [ -d ~/.arduino15 ]; then
        ARDUINO15_PATH=~/.arduino15
    else
        echo "Please set the ARDUINO15_PATH environment variable to the path of Arduino15 on your computer."
        exit 1
    fi
fi

[ -n "$1" ] && v=$1 || v=V2
v="$(tr '[a-z]' '[A-Z]' <<< $v)"
if [ $v = "V1" ]; then
    v=""
elif [ "$v" != "V2" ]; then
    echo "Usage: mbitbuild.sh [V1|V2]"
    exit 1
fi

# compile c sources
arduino-cli compile -b sandeepmistry:nRF5:BBCmicrobit$v --build-property build.extra_flags="-DNRF52 -DMICROBIT$v"

# replace gcc with g++ for c++ sources (with gcc it won't link with the c++ libraries)
mv $ARDUINO15_PATH/packages/sandeepmistry/tools/gcc-arm-none-eabi/5_2-2015q4/bin/arm-none-eabi-gcc ../gcc
cp $ARDUINO15_PATH/packages/sandeepmistry/tools/gcc-arm-none-eabi/5_2-2015q4/bin/arm-none-eabi-g++ $ARDUINO15_PATH/packages/sandeepmistry/tools/gcc-arm-none-eabi/5_2-2015q4/bin/arm-none-eabi-gcc
[ -d build/mbit$v ] || mkdir -p build/mbit$v
arduino-cli compile -b sandeepmistry:nRF5:BBCmicrobit$v  --build-property build.extra_flags="-DNRF52 -DMICROBIT$v" --output-dir build/mbit$v
a=$?
mv ../gcc $ARDUINO15_PATH/packages/sandeepmistry/tools/gcc-arm-none-eabi/5_2-2015q4/bin/arm-none-eabi-gcc
exit $a