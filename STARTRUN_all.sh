#!/bin/bash

if [ -f KILLRUN.local ]
then
    sh KILLRUN.local
else
    sh KILLRUN.sh
fi

export RCPORT=44000
export HOSTIP=192.168.222.1
export HOSTNAME=192.168.222.1

dt=`date +"%y_%b_%d_%Hh%Mm"`
echo $dt

NEWRUNNUM=$((RUNNUM+1))
echo 'Last Run number: ' $RUNNUM 'New Run number:' $NEWRUNNUM


#################  Run control ###################
./bin/euRun -n RunControl -a tcp://${RCPORT} &
sleep 5
#################  Log collector #################
./bin/euCliLogger -r tcp://${HOSTIP}:${RCPORT} -a tcp://44002 &



#################  Data collector #################
xterm -r -sb -sl 100000 -T "EventnumberSyncData collector" -e 'bin/euCliCollector -n EventnumberSyncDataCollector -t dc1 -r tcp://${HOSTIP}:${RCPORT}; -a tcp://45002; read' &
xterm -r -sb -sl 100000 -T "EventnumberSyncData collector" -e 'bin/euCliCollector -n EventnumberSyncDataCollector -t dc2 -r tcp://${HOSTIP}:${RCPORT} -a tcp://45001; read' &

#################  Producer #################
#gnome-terminal --geometry=80x600-280-900 -t "CMS HGCal Producer" -e "bash -c \"source ../setup_eudaq_cmshgcal.sh; ./HGCalProducer -r tcp://$HOSTIP:$RCPORT\" " &
#xterm -r -sb -sl 100000 -geometry 160x30 -T "Ex0-Producer" -e 'bin/euCliProducer -n Ex0Producer -t exo -r tcp://$HOSTIP:$RCPORT |tee -a logs/ex0.log ; read'&
xterm -r -sb -sl 100000 -geometry 160x30 -T "CMS-HGCAL0" -e 'bin/euCliProducer -n HGCalProducer -t cms-hgcal0 -r tcp://$HOSTIP:$RCPORT '&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal1 -r tcp://$HOSTIP:$RCPORT&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal2 -r tcp://$HOSTIP:$RCPORT&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal3 -r tcp://$HOSTIP:$RCPORT&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal4 -r tcp://$HOSTIP:$RCPORT&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal5 -r tcp://$HOSTIP:$RCPORT&
# nohup ./bin/euCliProducer -n HGCalProducer -t cms-hgcal6 -r tcp://$HOSTIP:$RCPORT&


################## OnlineMonitor ##################
printf '\033[22;33m\t  HGC Online Monitor  \033[0m \n'
flog="./logs/Run${NEWRUNNUM}_HgcOnlineMon_$dt.log"
config_file="./user/cmshgcal/conf/onlinemon.conf"

nohup ./bin/CMSHGCalMonitor  --monitor_name CMSHGCalMonitor --config_file $config_file --reduce 1 --update 1000 --reset -r tcp://$HOSTIP:$RCPORT  --root > $flog 2>&1 &

printf "The logs from the Online Monitor are in $flog file. \n"
