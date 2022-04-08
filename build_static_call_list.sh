source="\nbool bundledLibCall(const Value& fileName, const Value& funcName, Value* args, int argc, Value*& res) {\n\tif (!fileName.startsWith(\"*\")) return false;\n\tTEXT fileNameStr = fileName.toString();\n#ifdef USE_ARDUINO_STRING\n\tfileNameStr = fileNameStr.substring(1);\n#else\n\tfileNameStr.erase(0, 1);\n#endif\n"
for i in $(echo $1 | tr ";" "\n")
do
    source="${source}\n\tif (fileNameStr == \"$i\") {"
    for j in $(cat libraries/$i/static_call_list.txt)
    do
        source="${source}\n\t\tif (funcName == \"$j\") {"
        source="${source}\n\t\t\tres = bundled_lib_${i}_${j}(args, argc);"
        source="${source}\n\t\t\treturn true;"
        source="${source}\n\t\t}"
        source="extern \"C\" Value* bundled_lib_${i}_${j}(Value* args, int argc);\n${source}"
    done
    source="${source}\n\t}"
done
source="${source}\n\treturn false;\n}"
echo "#include \"VM.h\""
echo
echo -e "$source"