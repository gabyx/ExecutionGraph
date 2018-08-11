#!/bin/python

import sys
import os
import argparse
import re

assert sys.version_info >= (3, 0)

parser = argparse.ArgumentParser(
    description=
    'Adapt includes in flatbuffers generators, for our folder structure')
parser.add_argument(
    '-i',
    '--inputFile',
    type=str,
    required=True,
    help='Input file generated from flatc with -M option!')
parser.add_argument(
    '-I',
    '--includePaths',
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
parser.add_argument(
    '-g', '--generator', required=True, help='Used generator (cpp,ts,...)')
args = parser.parse_args()

currentDir = os.getcwd()
includePaths = [currentDir] + args.includePaths
inputFile = args.inputFile
generator = args.generator
regexes = [re.compile(r) for r in args.regex]
substitutions = args.substitution

# Parse input file
with open(inputFile, "r") as f:
    lines = [line.rstrip() for line in f.readlines()]

fbsIncludes = {}

# Correct the file path for the used generator -> "cpp/..." to  "{generator}/..."
def correctForGenerator(generator, file):
    file = file.replace("cpp/", generator + "/")
    base, ext = os.path.splitext(file)
    if generator == "cpp":
        return base + ".h"
    elif generator == "ts":
        return base + ".ts"
    else:
        raise ValueError("generator {0} not implemented!".format(generator))

# Find the file in the include paths
def findFile(fileLine):
    filePath = fileLine.rstrip(' \\').strip()
    if not filePath:
        return None
    if os.path.isabs(filePath):
        if os.path.exists(filePath):
            return filePath
    else:
        for incPath in includePaths:
            f = os.path.join(
                incPath, filePath) if os.path.isabs(incPath) else os.path.join(
                    currentDir, incPath, filePath)
            if os.path.exists(f):
                return os.path.abspath(f)
    raise ValueError(
        "File: '{0}' could not be found in the current working dir and includes paths!".
        format(filePath))


print("Parsing and finding .fbs include dependencies ...")
for line in lines:
    idx = line.find(":")

    if idx > 0:
        # new out file
        outFile = line[0:idx]
        outFile = findFile(outFile)
        currentOutFile = fbsIncludes[outFile] = []

        depFile = line[idx + 1:]
        if depFile:
            path = findFile(depFile)
            if path:
                currentOutFile.append(path)
    else:
        # dependency
        if line:
            path = findFile(line)
            if path:
                currentOutFile.append(path)

print("Making replacement regexes")

def convertToGeneratedIncludes(generator, inc):

    inc = correctForGenerator(generator, inc)

    base, ext = os.path.splitext(inc)
    base, fileName = os.path.split(base)

    if generator == "cpp":
        return os.path.join(base, generator, fileName + "_generated.h")
    elif generator == "ts":
        return os.path.join(base, generator, fileName + "_generated.ts")
    else:
        raise ValueError("generator {0} not implemented!".format(generator))


def replacement(generator, inc, regexes, substitutions):

    if generator == "ts":
        inc = os.path.splitext(inc)[0]

    oldPath = "(./)?" + os.path.split(inc)[1]
    for r, s in zip(regexes, substitutions):
        newPath, count = r.subn(s, inc)
        if count:
            return (inc, oldPath, newPath)

    raise ValueError("Could not replace include '{0}' with '{1}', '{2}'".format(
        inc, substitutions,regexes))


replacementIncludes = {}
for outFile, includes in fbsIncludes.items():

    outFile = correctForGenerator(generator, outFile)
    # Pop last line which corresponds to the generator
    includes.pop()

    # print("outFile: '{0}'".format(outFile))
    # print("includes: '{0}'".format("\n".join(includes)))
    # convert to generator include path
    generatedIncludes = [
        convertToGeneratedIncludes(generator, inc) for inc in includes
    ]

    replacementIncludes[outFile] = [
        replacement(generator, inc, regexes, substitutions)
        for inc in generatedIncludes
    ]

print("Apply regex/replace over {0} flatbuffers generated files:".format(
    len(fbsIncludes)))

for outFile, replacements in replacementIncludes.items():
    if replacements:

        print("Replacing '{0}'".format(outFile))
        with open(outFile, 'r') as f:
            content = f.read()

        for genetaredFile, oldInc, newInc in replacements:
            #print("'{0}' ->'{1}'".format(oldInc, newInc))
            repl = re.compile('"{0}"'.format(oldInc))
            content = repl.sub('"{0}"'.format(newInc), content, count=1)

        #("Write new generated filed '{0}'".format(outFile))
        with open(outFile, "w+") as f:
            f.write(content)