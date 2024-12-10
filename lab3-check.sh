check_dir=0
test=false
for i in "$@"
do
    if [ $test = true ]
    then
      echo "Debug => $i"
    fi
    if [ "$i" = "-d" ]; then
        check_dir=1           #if the -d (str) is in the name
        continue
    fi
    if [ $check_dir = 1 ]
    then
        if [ -d "$i" ]
        then
            echo "$i is a directory"
        else
            echo "$i is not a directory"
        fi
        check_dir=0
    else
        if [ -e "$i" ]; then
            echo "$i exists"
        else
            echo "$i does not exist"
        fi
    fi
done