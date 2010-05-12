#!/bin/bash
# 4 column, size of file,  number created, creations per second, removals per second

DIR_PATH=/srv/cs523/misc/final-report/
LM_BENCH_PATH="$DIR_PATH""lmbench3/results/armv7l-linux-gnu"
BASE_FILES_REGEXP="palm-webos-device.[0-6]"
CONTAINER_FILES_REGEXP="palm-webos-device.[7-9]"
IN_CONTAINER_FILES_REGEXP="palm-webos-device.[102][102]"
RESULTS_FILENAME=lmbench-results.tar.gz

pushd $DIR_PATH
if [ ! -d lmbench3 ]; then
  tar -xf $RESULTS_FILENAME 
fi

metrics=( '0k' '1k' '4k' '10k' )

for i in `/usr/bin/seq 0 ${#metrics[@]}`
do
  echo "${metrics[${i}]}""create"
  BASE_CREATE=`/bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$BASE_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$3;}END{print s/NR;}'`; 
  /bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$CONTAINER_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$3;}END{print s/(NR*'"$BASE_CREATE"');}'; 
  /bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$IN_CONTAINER_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$3;}END{print s/(NR*'"$BASE_CREATE"');}'; 

  echo "${metrics[${i}]}""delete"
  BASE_DELETE=`/bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$BASE_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$4;}END{print s/NR;}'`; 
  /bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$CONTAINER_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$4;}END{print s/(NR*'"$BASE_DELETE"');}'; 
  /bin/egrep "^""${metrics[${i}]}" $LM_BENCH_PATH/$IN_CONTAINER_FILES_REGEXP | /usr/bin/awk 'BEGIN{s=0;}{s=s+$4;}END{print s/(NR*'"$BASE_DELETE"');}'; 
done

