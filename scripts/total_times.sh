#!/bin/bash

HEADER=`echo "Class, Processes, Run, Time, TD_TI, COMM_COMP"`
cd ../traces/ti/lu;

echo $HEADER >  ../../../datasets/ti_total_comm_time.csv ;
for i in `ls [B,C]/{8,16}/*/0`; do 
  inst=`echo $i | awk -F '/' '{print $1","$2","$3}'`
  echo $inst","`awk -F' ' '/COMM/{gsub("\\\\(","",$4);s+=($4*$5)} END {print s",Time-Independent,Communication"}' $i`>> ../../../datasets/ti_total_comm_time.csv
done

echo $HEADER >  ../../../datasets/ti_total_comp_time.csv ;
for i in `ls [B,C]/{8,16}/*/0`; do 
  inst=`echo $i | awk -F '/' '{print $1","$2","$3}'`
  echo $inst","`awk -F' ' '/COMP/{gsub("\\\\(","",$4);s+=($4*$5)} END {print s",Time-Independent,Computation"}' $i`>> ../../../datasets/ti_total_comp_time.csv
done

cd -

cd ../traces/td/lu;

echo $HEADER >  ../../../datasets/td_total_comm_time.csv ;
for i in `ls [B,C]/{8,16}/*/0`; do 
  inst=`echo $i | awk -F '/' '{print $1","$2","$3}'`
  echo $inst","`awk -F' ' '/COMM/{gsub("\\\\(","",$4);s+=($4*$5)} END {print s",Timed,Communication"}' $i`>> ../../../datasets/td_total_comm_time.csv
done

echo $HEADER >  ../../../datasets/td_total_comp_time.csv ;
for i in `ls [B,C]/{8,16}/*/0`; do 
  inst=`echo $i | awk -F '/' '{print $1","$2","$3}'`
  echo $inst","`awk -F' ' '/COMP/{gsub("\\\\(","",$4);s+=($4*$5)} END {print s",Timed,Computation"}' $i`>> ../../../datasets/td_total_comp_time.csv
done

cd -
