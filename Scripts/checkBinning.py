#!/bin/python

import sys
import argparse
import os
from subprocess import *

#definitions
stringDefault=""
stdout = False

def getOutputFileName(args):
	return args.filePrefix + args.file + args.filePostfix

def mergeResultAndBinsizeToFile(binsize, desiredName, tmpName):
	# this file should have two lines
	f = open(tmpName)
	lines = f.readlines()
	f.close()

	print lines

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

	return arguments

def main():

	parser = argparse.ArgumentParser(description="Analyse datafile with multiple binsizes")
	parser.add_argument('-e', '--executable', default="../dataAnalysis", help="Path to dataAnalysis executable")
	parser.add_argument('-f', '--file', default=stringDefault, help="File to analyse")
	parser.add_argument('-pre', '--filePrefix', default=stringDefault, help="Prefix for output file")
	parser.add_argument('-post', '--filePostfix', default="_binning_stat", help="Postfix for output file")

	args = parser.parse_args()

	#check if file is given, this can be done nicer
	if args.file == stringDefault:
		print "No datafile given. Aborting!"
		exit(1)

	outputFilename = getOutputFileName(args)
	if(os.path.isfile(outputFilename)):
		print "File for output \"" + outputFilename + "\" already exists, delete it..."
		os.remove(outputFilename)

	binsize = 1
	binsizeFitsDataInFile = True
	while(binsizeFitsDataInFile):
		tmpOutputFilename = "_" + outputFilename

		print "Call prog with binsize "+ str(binsize)  
		arguments = createArgumentsForProgramCall(binsize, args)

		# run the prog
		if(stdout):
			subject =  Popen(arguments)
		else:
			subject =  Popen(arguments, stdout=PIPE)
			for line in subject.stdout:
				pass
		subject.wait()
		if subject.returncode == 0:
			print "\tProgram completed successfully"
			mergeResultAndBinsizeToFile(binsize, outputFilename, tmpOutputFilename)
			os.remove(tmpOutputFilename)
		else:
			print "\tProgram terminated with exit code %i. Aborting!" % (subject.returncode )
			binsizeFitsDataInFile = False

		binsize += 1

if __name__ == '__main__':
	sys.exit(main())
