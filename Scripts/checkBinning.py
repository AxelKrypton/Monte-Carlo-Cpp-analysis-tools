#
#  Copyright (c) 2014 Christopher Pinke
#  Copyright (c) 2020 Alessandro Sciarra
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#

#!/bin/python

import sys
import argparse
import os
from subprocess import *

#definitions
stringDefault=""
intDefault = -1

def getOutputFileName(args):
	name =  args.filePrefix + args.file + args.filePostfix
	if(os.path.isfile(name)):
		print "File for output \"" + name + "\" already exists, delete it..."
		os.remove(name)
	return name

def mergeResultAndBinsizeToFile(binsize, desiredName, tmpName):
	# this file should have two lines
	f = open(tmpName)
	lines = f.readlines()
	f.close()

	# append lines to "big" file, adding binsize
	f = open(desiredName, "a")
	for line in lines:
		if "#" in line:
			newline = "# binsize " + line[1:]
			f.write(newline)
		else:
			newline = str(binsize) + "\t" + line
			f.write(newline)
	f.close()

def createArgumentsForProgramCall(binsize, args):
	arguments = ['./' + args.executable]
	arguments += ['--file={0}'.format(args.file)] 
	# add add. prefix
	arguments += ['--analysisOutputFilePrefix=_{0}'.format(args.filePrefix)] 
	arguments += ['--analysisOutputFilePostfix={0}'.format(args.filePostfix)] 
	# for now, only analyze mean
	arguments += ["--analyzeVariance"]
	arguments += ["--analyzeSkewness"]
	arguments += ["--analyzeKurtosis"]
	arguments += ['--binsize={0}'.format(binsize)]
	if (args.binningMustFit):
		arguments += ["--binningMustFitDataSampleSize"]

	return arguments

def addArgumentsToParser(parser):
	parser.add_argument('-e', '--executable', default="../dataAnalysis", help="Path to dataAnalysis executable")
	parser.add_argument('-f', '--file', default=stringDefault, help="File to analyse")
	parser.add_argument('-pre', '--filePrefix', default=stringDefault, help="Prefix for output file")
	parser.add_argument('-post', '--filePostfix', default="_binning_stat", help="Postfix for output file")
	parser.add_argument('-fit', '--binningMustFit', default=False, action='store_true', help="Require that binning parameter is a multiple of the data size")
	parser.add_argument('-out', '--printToStdOut', default=False, action='store_true', help="Print output of analysis program to screen")
	parser.add_argument('-min', '--binsizeMin', type=int, default = intDefault, help = "Minimal value for binsize (binsize=1 is always evaluated)")
	parser.add_argument('-max', '--binsizeMax', type=int, default = intDefault, help = "Maximal value for binsize")
	parser.add_argument('-incr', '--binsizeIncr', type=int, default = 1, help = "Increment for binsize (binsize=1 is always evaluated)")

def parseArgumentsFromCommandLine():
	parser = argparse.ArgumentParser(description="Analyse datafile with multiple binsizes")
	addArgumentsToParser(parser)
	args = parser.parse_args()

	#check if file is given, this can be done nicer
	if args.file == stringDefault:
		print "# No datafile given. Aborting!"
		exit(1)

	return args

def handleReturnCode(subject, binsize, outputFilename, tmpOutputFilename):
	if subject.returncode == 0:
		print "#\tProgram completed successfully"
		mergeResultAndBinsizeToFile(binsize, outputFilename, tmpOutputFilename)
		os.remove(tmpOutputFilename)
	else:
		if subject.returncode == 2:
			print "#\tProgram terminated with exit code %i, binsize does not match data size. Continue!" % (subject.returncode )
		else:
			print "#\tProgram terminated with exit code %i. Aborting!" % (subject.returncode )
			return False
	return True

def runAnalysisProgram(arguments, stdOut):
	if(stdOut):
		subject =  Popen(arguments)
	else:
		subject =  Popen(arguments, stdout=PIPE)
		for line in subject.stdout:
			pass
	subject.wait()
	return subject

def incrementBinsize(binsize, args):
	binsizeFitsDataInFile = True
	if binsize == 1:
		binsize += 1
	else:
		if (args.binsizeMin != intDefault) and  (binsize <= args.binsizeMin):
			binsize = args.binsizeMin
		binsize += args.binsizeIncr

	if args.binsizeMax != intDefault:
		if binsize > args.binsizeMax:
			print "# Maximal binsize reached. Break!"
			binsizeFitsDataInFile = False

	return binsize, binsizeFitsDataInFile

def main():
	args = parseArgumentsFromCommandLine()

	outputFilename = getOutputFileName(args)
	tmpOutputFilename = "_" + outputFilename
	binsize = 1
	binsizeFitsDataInFile = True

	while(binsizeFitsDataInFile):
		print "# Analyze data with binsize "+ str(binsize)  
		arguments = createArgumentsForProgramCall(binsize, args)
		subject = runAnalysisProgram(arguments, args.printToStdOut)
		if not handleReturnCode(subject, binsize, outputFilename, tmpOutputFilename):
			break

		binsize, binsizeFitsDataInFile = incrementBinsize(binsize, args)

if __name__ == '__main__':
	sys.exit(main())
