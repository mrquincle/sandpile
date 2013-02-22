#!/bin/bash

p=`pwd`
echo "We work in: $p"

subdirs=(Lin_nondiss Lin_diss Rossum)

count=0
while [ "x${subdirs[count]}" != "x" ]; do
  file_cmds="at_cmds_${subdirs[count]}"
  echo "Subdir: $file_cmds"
  echo "export DISPLAY=:0" > $file_cmds
  echo "cd $p/${subdirs[count]}" >> $file_cmds
  echo "../../../scripts/run_nx.sh 4" >> $file_cmds
  postpone=$(( $count * 3 ))
  count=$(( $count + 1 ))
  if [ $postpone -eq 0 ]; then
    echo "Run experiment ${subdirs[count]} in one minute" 
    at -f $file_cmds now + 1 minute
  else
    echo "Postpone experiment ${subdirs[count]} till over $postpone hours" 
    at -f $file_cmds now + $count hours
  fi
done

echo "Queue of activities:" 
atq
echo "Remove activities by atrm and the job number"

exit 0
