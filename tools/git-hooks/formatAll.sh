#!/bin/bash

echo "Format All:"

root="$(git rev-parse --show-toplevel)"
[ -d "$root" ] || exit 1

owndir="$(cd "$(dirname "$0")"; pwd -P)"

OS_NAME=$(uname)

if [ "$OS_NAME" = "Darwin" ]; then
  FIND_ARGS="-perm +0111"
else
  FIND_ARGS="-executable"
fi

find "$owndir"/autoformat -type f $FIND_ARGS | {
  abort=0

  while read formatter ; do
    magic="$formatter".magic
    patterns="$formatter".patterns
    [ -f "$patterns" -o -f "$magic" ] || continue

    echo "$patterns, $magic" 

    find "$root" -type f -not \( -ipath "$root/.git*" -or -ipath "$root/.vscode*" -or \
                                 -ipath "$root/build*" -or -ipath "$root/install*" -or \
                                 -ipath "$root/tools*" \) | {
      labort=0

      while IFS= read -r orig ; do
        orig="${root}/${orig}"
        echo "testing file: $orig"

        # file matches one of the patterns
        match_pattern=''
        [ -f "$patterns" ] && echo "$orig" | grep -Eqif "$patterns" && match_pattern='1'

        # file’s libmagic output matches
        match_magic=''
        [ $match_pattern ] || {
          [ -f "$magic" ] && file "$orig" | grep -Eqif "$magic" && match_magic='1'
        }

        # if none, ignore
        [ "$match_pattern" -o "$match_magic" ] ||  continue

        echo "formatting file: $orig"
        "$formatter" "$orig" || labort=1
        git add "$orig"
      done

      exit $labort
    } || abort=1
  done

  exit $abort
}

echo "Format All: [DONE]"