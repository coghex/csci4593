for (( i=0 ; i<$COLUMNS ; i++ ))
do
  echo -n "-"
done
zcat traces/omnetpp.Z | ./cachesim configs/defaultconfig
