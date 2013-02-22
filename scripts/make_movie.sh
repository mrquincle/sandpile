#!/bin/bash

if [ $# -ne 2 ]
then
  echo "Type the name of the directory with the .jpeg files"
  read PATH
else
  FILEPATH="$2"
fi

echo "We go to path: $FILEPATH"
cd "$FILEPATH"

# Accelerated
FPS=10
OFPS=10

#WIDTH=128
#HEIGHT=128

MASK=height

FILENAME="0movie.$MASK"

FORMAT=jpeg

echo "Converting all $MASK.ppm files..."
for f in $MASK*.ppm; do 
	convert -quality 100 $f `basename $f .ppm`.jpeg; 
done

# Automatic retrieve the size of the jpeg
WIDTH=`identify -verbose ${MASK}_0.jpeg | grep Geometry | cut -f2 -d':' | cut -f1 -d'x' | tr -d ' '`
HEIGHT=`identify -verbose ${MASK}_0.jpeg | grep Geometry | cut -f2 -d':' | cut -f2 -d'x' | cut -f1 -d'+' | tr -d ' '` 

echo "Size: ${WIDTH}x${HEIGHT}"

# The default mf mask does not understand filenames with numbers like file0.jpeg, file1.jpeg, etc. 
# there should be leading zeros to make this work, hence this workaround in which we sort ourselves
ls -1 ${MASK}*.jpeg > unordered
sort -t_ -k2 -n unordered > ordered
#head ordered

# This will lead to debug info about frames being skipped, this stimms.... (we do not care about audio)
#echo /usr/bin/mencoder mf://@ordered -mf w=$WIDTH:h=$HEIGHT:fps=$FPS:type=$FORMAT -ovc lavc -lavcopts vcodec=mpeg4 -ofps $OFPS -o "$FILENAME.avi"

/usr/bin/mencoder mf://@ordered -mf w=$WIDTH:h=$HEIGHT:fps=$FPS:type=$FORMAT -ovc lavc -lavcopts vcodec=mpeg4 -ofps $OFPS -o "$FILENAME.avi"

# Create an .swf file from .avi (this can be used for YouTube)
# ffmpeg -i $FILENAME.avi -ar 22050 -ab 56 -b 200000 -r 12 -f swf -s 600x400 -acodec mp3 -ac 1 $FILENAME.swf

# Create an .swf file from .avi (this can be used for YouTube), this doesn't not care about audio
#/usr/bin/ffmpeg -i $FILENAME.avi -f swf -s 600x400 $FILENAME.swf

