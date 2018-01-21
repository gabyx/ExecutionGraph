#!/bin/bash
#  Find and Replace Files inplace.
#  fileRegexReplace -h for Help!

function finish {
    info "FileRegexReplace done =============================================:"
}
trap finish EXIT

NJobs=1
ForceFlag='false'
Verbose='true'
declare -a regex=()


WARNINGFMT="\033[31m\033[1m"
INFOFMT="\033[34m\033[1m"
INFONORMALFMT="\033[1m"
CLEARFMT="\033[0m"

function warning(){
    echo -e "$WARNINGFMT$1$CLEARFMT"
}
function info(){
    echo -e "$INFOFMT$1$CLEARFMT"
}
function infoVerbose(){
    echo -ne "$INFOFMT"
    echo -En "$1"
	echo -e "$CLEARFMT"
}
function infoN(){
    echo -e "$INFONORMALFMT$1$CLEARFMT"
}
function infoNVerbose(){
	echo -ne "$INFONORMALFMT"
    echo -En "$1"
	echo -e "$CLEARFMT"
}

function join_by { local d=$1; shift; echo -n "$1"; shift; printf "%s" "${@/#/$d}"; }

function replaceInFileAll(){
    file=$1
    echo -e "${INFOFMT}Process file $file ..."
    
    allRegexes=$(join_by "; " "${regex[@]}")
    
    if [[ $Verbose == 'true' ]]; then
        infoN "Will execute:"
        infoNVerbose "perl -i.bak -0777pe '$allRegexes' '$file' && rm '$file.bak'"
    fi
        
    if [[ $ForceFlag == 'false' ]]; then
        info "===== Dry run!!, add flag -f to force the run!"
        return
    else
        replaceInFile "$file" "$allRegexes"
    fi
}

function printHelp(){

    read -r -d '' message << EOM
    fileRegexReplace [-v|--verboseOff] [-f|--force] [-e|--exclude]... [-r|--regex]... [-R|--fileRegex]... searchPath
    
    Find and Replace Files inplace all Regexes to Perl are "-regextype posix-extended".
    
        -n|--nJobs         how many jobs to use to do the replacement (default = 1, if more console printing will be scrambled!)
        -f|--force         force the replacement (if not specified, its a dry run!)
        -e|--excludeDir    exclude pattern like "*/external/*" (multiple -e allowed) 
                           (input to find, no perl regex!)
        -r|--regex         replacment regex (e.g: s@([^\w])_([a-zA-Z0-9]+)(?=[^\w])?@\1m_\2\3@g) 
                           (multiple -r allowed)
        -R|--fileRegex     matches only files with the given regex e.g. ".*\.(hpp|cpp)" $
                           (multiple -r allowed)
        -v|--verboseOff    do not print additional information!
    
        searchPath         filepath for recursive search and replace'
EOM
    infoN  "${message}"
}

function replaceInFile(){

    perl -i.bak -0777pe "$2" "$1" && rm "$1.bak"
    # 0777: reads the file into ram (changes file spereator http://stackoverflow.com/questions/16742526/how-to-search-and-replace-across-multiple-lines-with-perl)
    # with this we can match/replace also multiple lines wit /s modifier as "s@.*@Gaga@s"
    # p: reads line by line
    # e: command execution
    return $?
}

info "FileRegexReplace ==================================================:"

# Processing some options =====================================================================
# echo "BEFORE GETOPT:" "1:"$1 "2:"$2 "3:"$3 "4:"$4;
# Execute getopt
## Note that we use `"$@"' to let each command-line parameter expand to a 
# separate word. The quotes around `$@' are essential!
# We need ARGS as the `eval set --' would nuke the return value of getopt.

ARGS=$(getopt -o n:r:e:R:fhv -l "nJobs:,regex:,excludeDir:,fileRegex:,force,help,verboseOff" -n "fileRegexReplace.sh" -- "$@");
#echo $ARGS
# if last command failed exit
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
# Note the quotes around `$ARGS': they are essential!
eval set -- "$ARGS"
#echo "AFTER  GETOPT: $@";
while true ; do
    case "$1" in
        -n|--nJobs) NJobs="$2"; shift 2;;
        -h|--help) printHelp; exit 0; shift ;;
        -f|--force) info "Force flag!"; ForceFlag='true'; shift ;;
        -r|--regex) regex+=("$2") ; shift 2;;
        -v|--verboseOff) Verbose='false'; shift ;; 
        -e|--excludeDir) info "Exclude: '$2'";
                        if [ -z "$excludeDirs" ]; then
                           excludeDirs+=( -not -path "$2" );
                        else
                           excludeDirs+=( -and -not -path "$2" ); 
                        fi
                        shift 2;;
        --) shift ; break ;;
        -R|--fileRegex) info "Filename Regex: '$2'"; fileNameRegex=( -regextype posix-extended -regex "$2" ); shift 2;;
        *) warning "Internal error!: $1" ; exit 1 ;;
    esac
done

#echo "Remaining arguments:"
searchFolder=$1
#=================================================================================================


if [ -z "$searchFolder" ] || [ "$searchFolder" == "" ]; then
   warning "No SearchFolder specified"
   exit
fi

if [ -z "$excludeDirs" ]; then
   warning "No excludeDirs specified"
else
   info "Exclude Dir Option: '$excludeDirs'"
fi

if [ ${#regex[@]} -eq 0 ]; then
   warning "No replacment strings specified!"
   exit
else
   for r in "${regex[@]}"; do
   infoVerbose "Replacement String: '$r'"
   done
fi

if [ -d "${searchFolder}" ] || [ -f "${searchFolder}" ] ; then
    info "SearchDir: '${searchFolder}'" 
else
    warning "Search directory '${searchFolder}' does not exist!"
    exit
fi

# Print all files
if [[ $Verbose == 'true' ]]; then
    files=$(find "${searchFolder}" -type f "${fileNameRegex[@]}" "${excludeDirs[@]}")
    s=$(echo "$files" | wc -l)
    info "Replacing $s files"
fi


# Replace all files
(
find "${searchFolder}" -type f "${fileNameRegex[@]}" "${excludeDirs[@]}" -print0 | while read -d $'\0' -r file ; 
do 
    ((i=i%NJobs)); ((i++==0)) && wait
    replaceInFileAll "$file" &
done
)