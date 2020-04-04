#!/bin/bash
#=========================================================================================
# ExecutionGraph
# Copyright (C) 2014 by Gabriel Nützi <gnuetzi (at) gmail (døt) com>
#
# @date Sat Apr 04 2020
# @author Gabriel Nützi, gnuetzi (at) gmail (døt) com
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#=========================================================================================

# Compile script to check no-compile tests
# against declared compile output regexes

function printError() {
    echo "$@" >&2
}

function die() {
    printError "$@"
    exit 1
}

cmake="$1"
target="$2"
configuration="$3"
workingDir="$4"
testIndex="$5"
compileOutputRegexes="$6"

[ -f "$compileOutputRegexes" ] || die "[failure]: compile error reference '$compileOutputRegexes' not found"

# Read all compile errors regex which need to match the compile output
allRegexes=$(sed -rn -E "/^#if\s+EG_NO_COMPILE_TEST_INDEX\s+==\s+${testIndex}/,/^#endif/p" "$compileOutputRegexes" |
    sed -rn 's@^\s*//\s+CompileErrorRegex:\s+"(.*)"@\1@p')

cd "$workingDir" || die "working directory '$workingDir' wrong"
out=$("${cmake}" --build . --target "$target" --config "$configuration" 2>&1 1>/dev/null )

# Check all regexes
errors=$(mktemp)
echo "$allRegexes" | while IFS= read -r regex; do
    if ! echo "$out" | grep -qE "$regex"; then
        echo "-> [failure]: Compile output regex '$regex' not matched!" >"$errors"
    fi
done

if [ -s "$errors" ]; then
    printError "Output is:"
    printError "$out"
    cat "$errors" >&2
    exit 1
fi

exit 0
