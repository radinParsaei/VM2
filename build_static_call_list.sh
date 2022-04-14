info=""
fileCount=0
funcCount=0
source="\nbool bundledLibCall(const Value& file, const Value& func, Value* args, int argc, Value*& res) {"
for i in $(echo $1 | tr ";" "\n")
do
    source="${source}\n\tif (file == Value($fileCount)) {"
    info="${info}${i}:\n  - _: $fileCount\n"
    for j in $(cat libraries/$i/static_call_list.txt)
    do
        source="${source}\n\t\tif (func == Value($funcCount)) {"
        info="${info}  - ${j}: $funcCount\n"
        funcCount=$((funcCount+1))
        source="${source}\n\t\t\tres = bundled_lib_${i}_${j}(args, argc);"
        source="${source}\n\t\t\treturn true;"
        source="${source}\n\t\t}"
        source="extern \"C\" Value* bundled_lib_${i}_${j}(Value* args, int argc);\n${source}"
    done
    source="${source}\n\t}"
    fileCount=$((fileCount+1))
    funcCount=0
    info="${info}\n"
done
source="${source}\n\treturn false;\n}"
echo "#include \"VM.h\""
echo
echo -e "$source"
echo -e "$info" > static_call_list.yml