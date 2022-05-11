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

# compile c sources
arduino-cli compile -b arduino:sam:arduino_due_x

# replace gcc with g++ for c++ sources (with gcc it won't link with the c++ libraries)
mv $ARDUINO15_PATH/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-gcc ../gcc
cp $ARDUINO15_PATH/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-g++ $ARDUINO15_PATH/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-gcc
[ -d build/due ] || mkdir -p build/due
arduino-cli compile -b arduino:sam:arduino_due_x --output-dir build/due
a=$?
mv ../gcc $ARDUINO15_PATH/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-gcc
exit $a