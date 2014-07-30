#!/bin/bash

cd ../traces/ti/lu;

echo "Class, processes, size" >  ../../../datasets/scalatrace_sizes.csv ;
ls -l [B,C]/*/I/0  | awk '{printf("%s,%s\n",$9, $5)}' | awk '{gsub("/I/0","");print}' |awk '{gsub("/",",");print}' >> ../../../datasets/scalatrace_sizes.csv ;

echo "Class, processes, events" >  ../../../datasets/events_compact.csv ;
echo "Class, processes, events" >  ../../../datasets/events_unrolled.csv ;
for i in `ls [B,C]/*/I/0`; do 
  echo `echo $i | cut -d '/' -f 1,2 | awk '{gsub("/",",");print}'`","`grep EVENT $i | wc -l` >> ../../../datasets/events_compact.csv ;
  echo `echo $i | cut -d '/' -f 1,2 | awk '{gsub("/",",");print}'`","`grep COM $i | cut -d '(' -f 2 | cut -d ' ' -f 1 | awk '{s+=$1} END {print s}'` >> ../../../datasets/events_unrolled.csv ;
done

cd -
