#!/bin/bash

# A script to be used in conjunction with ImageCollector.
# After training images and bounding box information has been collected, run this script
# to split the data up into training data and test data

divisor=3
folderPath="$(pwd)/$1" # relative path to your image set
testFile="$folderPath/test.txt"
trainFile="$folderPath/train.txt"
echo "Test File Location: $testFile"
echo "Training File Location: $trainFile"

# delete old files if they exist
if [ -f $testFile ]; then
  echo 'Deleting existing train and test file . . .'
  rm $testFile
  rm $trainFile
fi

# get a list of all the files in the directory, and route them to
# one of two places
echo 'Iterating through files . . .'
count=1
for line in $(ls $folderPath | grep .jpg); do
  if [ $(( $count % $divisor )) -eq 0 ]; then
    echo "$folderPath/$line" >> $testFile
  else
    echo "$folderPath/$line" >> $trainFile
  fi
  count=$( expr $count + 1)
done
echo Done.
echo 'creating folder for saving weights . . . '
mkdir "$folderpath/weights"
echo "Training file size: $(du -h $trainFile | cut -f 1)"
echo "Test file size: $(du -h $testFile | cut -f 1)"
echo "Party on, Wayne."
echo