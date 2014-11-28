#!/bin/bash
BINARY=2
T=1
for i in `seq 1000`; do
number=$RANDOM
let "number %= $BINARY" 
if [ "$number" -eq $T ]
then
  curl localhost:4006/statsServer?q=cacheMiss
else
  curl localhost:4006/statsServer?q=cacheHit 
fi
sleep 0.1; 
done

