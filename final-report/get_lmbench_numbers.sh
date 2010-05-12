#!/bin/bash
DIR_PATH=/srv/cs523/misc/final-report/
LM_BENCH_PATH="$DIR_PATH""lmbench3/results/armv7l-linux-gnu"
FILES_REGEXP="palm-webos-device.[7-9]"
RESULTS_FILENAME=lmbench-results.tar.gz

pushd $DIR_PATH
if [ ! -d lmbench3 ]; then
  tar -xf $RESULTS_FILENAME 
fi

metrics=( 'Simple syscall' 'Simple read' 'Simple write' 'Simple fstat' 'Simple open/close' 'Process fork\+exit' 'Process fork\+execve' 'Process fork\+/bin/sh' )

for i in `/usr/bin/seq 0 ${#metrics[@]}`
do
  echo ${metrics[${i}]}
  /bin/egrep "${metrics[${i}]}" $LM_BENCH_PATH/$FILES_REGEXP | /usr/bin/awk -F: '{print $3}' | /usr/bin/awk 'BEGIN{s=0;}{s=s+$1;}END{print s/NR;}'; 
done
