#!/bin/python

import sys
import os
import argparse
import re
import glob
from collections import Iterable

assert sys.version_info >= (3, 0)

def flatten(items):
    """Yield items from any nested iterable; see Reference."""
    for x in items:
        if isinstance(x, Iterable) and not isinstance(x, (str, bytes)):
            for sub_x in flatten(x):
                yield sub_x
        else:
            yield x

parser = argparse.ArgumentParser(
    description=
    'Adapt includes in flatbuffers generators, for our folder structure')
parser.add_argument(
    '-i',
    '--input',
    action="append",
    required=True,
    help='Input files (glob expression).')
parser.add_argument(
    '-R',
    '--includeRegex',
    required=True,
    help='How to find the include statement.')
parser.add_argument(
    '-I',
    '--includePath',
    action="append",
    required=True,
    help='Include paths to find files!')
parser.add_argument(
    '-r',
    '--regex',
    action="append",
    required=True,
    help='Regexes which marks the string to take for the include!')
parser.add_argument(
    '-s',
    '--substitution',
    action="append",
    required=True,
    help='Substitutions for the regex.')

args = parser.parse_args()

currentDir = os.getcwd()
files = flatten([ glob.glob(g) for g in args.input ])
includePaths = args.includePath
includeRegex = re.compile(args.includeRegex)
regexes = [re.compile(r) for r in args.regex]
substitutions = args.substitution

# Find the file in the include paths
def getIncludePath(filePath):
    filePath = filePath.rstrip(' \\').strip()
    for incPath in includePaths:
        f = os.path.join(
            incPath, filePath) if os.path.isabs(incPath) else os.path.join(
                currentDir, incPath, filePath)
        print("in: " + f)
        if os.path.exists(f):
            return os.path.abspath(f), incPath
    return None, None

for file in files:

        print("Replacing '{0}'".format(file))
        with open(file, 'r') as f:
            content = f.read()

        class Substitutor:
            def __init__(self):
                self.regs = regexes
                self.subs = substitutions

            def __call__(self, includePathMatch):
                groups = includePathMatch.groups()
                assert len(groups) == 3
                newFilePath = filePath = groups[1]
                
                def substitute(m):
                    return m.expand(currentSub)

                for r, s in zip(self.regs, self.subs):
                    currentSub = s
                    newFilePath, n = r.subn(substitute, filePath)
                    if n >= 1:
                        print("repl: '{0}' --> '{1}'".format(filePath, newFilePath))
                        break
                
                return groups[0] + newFilePath + groups[2]

        content = includeRegex.sub(Substitutor(), content)

        with open(file, "w+") as f:
            f.write(content)