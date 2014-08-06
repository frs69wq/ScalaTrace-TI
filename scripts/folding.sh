#!/bin/bash

OUTPUT_DIR=../../../../../datasets 


########################## TI ACQUISITION TIMES #########################
cd ../traces-new/ti/lu/C/folded
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
cd -

########################## TD ACQUISITION TIMES #########################

cd ../traces-new/td/lu/C/folded
TIMES=$OUTPUT_DIR/td_folding_times.csv 
#Header
echo "Timer,Factor,cores,Time,Regular" > $TIMES

# Application timer
for c in {1,2,4}; do
    awk -F'=' -v c=$c '/Time in/{gsub(" ","", $2 ); print "Application,1,"c","$2","$2}' $c-core*/1/details >> $TIMES
    REGULAR=`awk -F'=' -v c=$c '/Time in/{gsub(" ","", $2 ); print $2}' $c-core*/1/details`
    for i in {2,4,8,16,32}; do
        if [ $c -eq 4 ] && [ $i -eq 32 ] 
	then
	    echo -n "Application,"$i","$c,"NA,NA" >> $TIMES;
	else
	    echo -n "Application,"$i","$c >> $TIMES;
	    awk -F'=' '/Time in/{gsub(" ","", $2 ); printf ","$2","}' $c-core*/$i/details >> $TIMES;
	    echo $REGULAR >> $TIMES;
	fi
    done
done
cd -

########################## TI TRACE CONTENTS  #########################
cd ../traces-new/ti/lu/C/folded
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

########################## TD TRACE CONTENTS  #########################
cd ../traces-new/td/lu/C/folded/1-core

#Folded modes
for i in {1,2,4,8,16,32}; do
    echo "eventID,opcode,subblock,COMP-4b.avg" > ../$OUTPUT_DIR/td_folding_$i.csv
    awk -F' ' '/EVENT/{gsub("EVENT|:","",$1); event= $1","$2}; /COMP/{printf event;gsub("4b-|:","",$2); print","$2","$5}' $i/0 >> ../$OUTPUT_DIR/td_folding_$i.csv
done
cd -
