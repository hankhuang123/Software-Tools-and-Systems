src="$1"
tgt="$2"
if [ "$#" -lt 2 ]   # "$#" the number of argument in lab04 file name
then
    echo "Error: Users give fewer than arguments condition" >&2
    exit 1
fi

if [ ! -d "$tgt" ]; then
    mkdir "tgt"
    for i in "$src"/*; 
    do
    year=$(basename "$i")
    #echo "$year" debug
    for j in "$i"/*; do
        course=$(basename "$j")
        #echo "$course"  debug
        if [  -d "$tgt"/"$course" ]; 
        then
            :
        else 
            mkdir "$tgt"/"$course"
        fi
            if [  -d "$tgt"/"$course"/"$year" ]; 
            then
                :
            else
                mkdir "$tgt"/"$course"/"$year"
            fi
        cp -R "$j"/*  "$tgt"/"$course"/"$year" 
    done
    done
else
    echo "Error: tgt file have been created." >&2
    exit 2
fi