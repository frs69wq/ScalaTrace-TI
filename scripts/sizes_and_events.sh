#!/bin/bash

cd ../traces-new/ti/lu;

echo "Class, processes, size" >  ../../../datasets/scalatrace_sizes.csv ;
ls -l [B,C]/*/I/0  | awk '{printf("%s,%s\n",$9, $5)}' | awk '{gsub("/I/0","");print}' |awk '{gsub("/",",");print}' >> ../../../datasets/scalatrace_sizes.csv ;

echo "Class, processes, events" >  ../../../datasets/events_compact.csv ;
echo "Class, processes, events" >  ../../../datasets/events_unrolled.csv ;
for i in `ls [B,C]/*/I/0`; do 
  inst=`echo $i | awk -F '/' '{print $1","$2}'`
  echo $inst","`grep EVENT $i | wc -l` >> ../../../datasets/events_compact.csv ;
  echo $inst","`awk -F' ' '/COM/{gsub("\\\\(","",$4); s+=$4} END {print s}' $i` >> ../../../datasets/events_unrolled.csv ;
done

cd -
