editpath() {
  if [ $# -eq 0 ]; then
    echo "Error: no parameter"
    exit 1
  fi

selected_opt=
current_position="$opt"
  while getopts ":pad" current_position; do
    case "$current_position" in
      p)
        selected_opt='p'
        ;;
      a)
        selected_opt='a'
        ;;
      d)
        selected_opt='d'
        ;;
      \?)
        echo "Not considered option: $OPTARG" >&2
        return 1
        ;;
    esac
  done

  shift $((OPTIND - 1))
  OPTIND=1

  
  case $selected_opt in
    p)
      while [ $# -gt 0 ]; 
      do
        current_path=$1
        PATH="$current_path:$PATH"
        shift
      done
        ;;
    a)
      while [ $# -gt 0 ]; 
      do
        current_path=$1
        PATH="$PATH:$current_path"
        shift
      done
      ;;
    d)
      while [ $# -gt 0 ]; 
      do
        current_path=$1
        PATH=$(echo "$PATH" | tr ':' '\n' | grep -F -x -v "$current_path" | awk '{ printf "%s%s",pos,$0; pos=":" }')
        #awk 1.%s->empty 2.%s->path add ':' pos $0(combine)
        #grep -F -x -v Filtering the different parts
        shift
      done
      ;;
    \?)
      echo "Some error happened when edit the path" >&2
      return 1;
      ;;
  esac
}







