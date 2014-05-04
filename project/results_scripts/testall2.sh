#!/bin/bash
#
#echo "Starting Omnetpp"
#echo "omnetpp.default:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/defaultconfig
#echo "omnetpp.all2way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all2wayconfig
#echo "omnetpp.all4way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all4wayconfig
#echo "omnetpp.fullyassociative:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/allfaconfig
#echo "omnetpp.l12way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l12wayconfig
#echo "omnetpp.l1dsmall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1dsmallconfig
#echo "omnetpp.l1ismall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1ismallconfig
#echo "omnetpp.l1small:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1smallconfig
#echo "omnetpp.l24way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l24wayconfig
#echo "omnetpp.l2big:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l2bigconfig
#
#echo "Starting Omnetpp with memchunksize=32"
#echo "omnetpp32.default:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/defaultconfigmcs2
#echo "omnetpp32.all2way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all2wayconfigmcs2
#echo "omnetpp32.all4way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all4wayconfigmcs2
#echo "omnetpp32.fullyassociative:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/allfaconfigmcs2
#echo "omnetpp32.l12way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l12wayconfigmcs2
#echo "omnetpp32.l1dsmall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1dsmallconfigmcs2
#echo "omnetpp32.l1ismall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1ismallconfigmcs2
#echo "omnetpp32.l1small:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1smallconfigmcs2
#echo "omnetpp32.l24way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l24wayconfigmcs2
#echo "omnetpp32.l2big:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l2bigconfigmcs2
#
#echo "Starting Omnetpp with memchunksize=64"
#echo "omnetpp64.default:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/defaultconfigmcs4
#echo "omnetpp64.all2way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all2wayconfigmcs4
#echo "omnetpp64.all4way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/all4wayconfigmcs4
#echo "omnetpp64.fullyassociative:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/allfaconfigmcs4
#echo "omnetpp64.l12way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l12wayconfigmcs4
#echo "omnetpp64.l1dsmall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1dsmallconfigmcs4
#echo "omnetpp64.l1ismall:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1ismallconfigmcs4
#echo "omnetpp64.l1small:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l1smallconfigmcs4
#echo "omnetpp64.l24way:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l24wayconfigmcs4
#echo "omnetpp64.l2big:"
#zcat ../../traces-long/omnetpp.gz | ./cachesim configs/l2bigconfigmcs4
#
#echo "Starting Astar"
#echo "astar.default:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/defaultconfig
#echo "astar.all2way:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/all2wayconfig
#echo "astar.all4way:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/all4wayconfig
#echo "astar.fullyassociative:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/allfaconfig
#echo "astar.l12way:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l12wayconfig
#echo "astar.l1dsmall:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l1dsmallconfig
#echo "astar.l1ismall:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l1ismallconfig
#echo "astar.l1small:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l1smallconfig
#echo "astar.l24way:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l24wayconfig
#echo "astar.l2big:"
#zcat ../../traces-long/astar.gz | ./cachesim configs/l2bigconfig
#
echo "Starting Bzip2"
#echo "bzip2.default:"
#zcat ../../traces-long/bzip2 | ./cachesim configs/defaultconfig
echo "bzip2.all2way:"
zcat ../../traces-long/bzip2 | ./cachesim configs/all2wayconfig
echo "bzip2.all4way:"
zcat ../../traces-long/bzip2 | ./cachesim configs/all4wayconfig
echo "bzip2.fullyassociative:"
zcat ../../traces-long/bzip2 | ./cachesim configs/allfaconfig
echo "bzip2.l12way:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l12wayconfig
echo "bzip2.l1dsmall:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l1dsmallconfig
echo "bzip2.l1ismall:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l1ismallconfig
echo "bzip2.l1small:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l1smallconfig
echo "bzip2.l24way:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l24wayconfig
echo "bzip2.l2big:"
zcat ../../traces-long/bzip2 | ./cachesim configs/l2bigconfig


echo "Starting Gobmk"
echo "gobmk.default:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/defaultconfig
echo "gobmk.all2way:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/all2wayconfig
echo "gobmk.all4way:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/all4wayconfig
echo "gobmk.fullyassociative:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/allfaconfig
echo "gobmk.l12way:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l12wayconfig
echo "gobmk.l1dsmall:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l1dsmallconfig
echo "gobmk.l1ismall:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l1ismallconfig
echo "gobmk.l1small:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l1smallconfig
echo "gobmk.l24way:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l24wayconfig
echo "gobmk.l2big:"
zcat ../../traces-long/gobmk.gz | ./cachesim configs/l2bigconfig

#echo "Starting Sjeng"
#echo "sjeng.default:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/defaultconfig
#echo "sjeng.all2way:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/all2wayconfig
#echo "sjeng.all4way:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/all4wayconfig
#echo "sjeng.fullyassociative:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/allfaconfig
#echo "sjeng.l12way:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l12wayconfig
#echo "sjeng.l1dsmall:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l1dsmallconfig
#echo "sjeng.l1ismall:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l1ismallconfig
#echo "sjeng.l1small:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l1smallconfig
#echo "sjeng.l24way:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l24wayconfig
#echo "sjeng.l2big:"
#zcat ../../traces-long/sjeng.gz | ./cachesim configs/l2bigconfig
