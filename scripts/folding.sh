#!/bin/bash

OUTPUT_DIR=../../../../../datasets 

cd ../traces-new/ti/lu/C/folded

########################## ACQUISITION TIMES #########################
TIMES=$OUTPUT_DIR/ti_folding_times.csv 

#Header
echo "Timer,Factor,Time,Regular" > $TIMES

# Application timer
awk -F'=' '/Time in/{gsub(" ","", $2 ); print "Application,1,"$2","$2}' ../64/details.I >> $TIMES
REGULAR=`awk -F'=' '/Time in/{gsub(" ","", $2 ); print $2}' ../64/details.I`
for i in {2,4,8,16,32}; do
    echo -n "Application,"$i >> $TIMES;
    awk -F'=' '/Time in/{gsub(" ","", $2 ); printf ","$2","}' $i/details.I >> $TIMES;
    echo $REGULAR >> $TIMES;
done

# Real timer
awk -F' ' '/real/{ print "Real,1,"$2","$2}' ../64/details.I >> $TIMES
REGULAR=`awk -F' ' '/real/{ print $2}' ../64/details.I`
for i in {2,4,8,16,32}; do
    echo -n "Real,"$i >> $TIMES
    awk -F' ' '/real/{ printf ","$2","}' $i/details.I >> $TIMES
    echo $REGULAR >> $TIMES
done

# User timer
awk -F' ' '/real/{ print "User,1,"$4","$4}' ../64/details.I >> $TIMES
REGULAR=`awk -F' ' '/real/{ print $4}' ../64/details.I`
for i in {2,4,8,16,32}; do
    echo -n "User,"$i >> $TIMES
    awk -F' ' '/real/{ printf ","$4","}' $i/details.I >> $TIMES
    echo $REGULAR >> $TIMES
done

########################## TRACE CONTENTS  #########################
#Header
echo "eventID,opcode,subblock,COMP-4c.avg" > $OUTPUT_DIR/ti_folding_regular.csv

#Regular mode
awk -F' ' '/EVENT/{gsub("EVENT|:","",$1); event= $1","$2}; /COMP/{printf event;gsub("4c-|:","",$2); print","$2","$5}' ../64/I/0 >> $OUTPUT_DIR/ti_folding_regular.csv

#Folded modes
for i in {2,4,8,16,32}; do
    echo "eventID,opcode,subblock,COMP-4c.avg" > $OUTPUT_DIR/ti_folding_$i.csv
    awk -F' ' '/EVENT/{gsub("EVENT|:","",$1); event= $1","$2}; /COMP/{printf event;gsub("4c-|:","",$2); print","$2","$5}' $i/I/0 >> $OUTPUT_DIR/ti_folding_$i.csv
done
cd -
