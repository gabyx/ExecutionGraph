#!/bin/bash
owndir="$(cd "$(dirname "$0")"; pwd -P)"
$owndir/format-hooks/pre-commit --formatAll