#!/bin/bash
echo "Downloading table from server"
python loadtable.py
RPATH=../../locale/caesar.
echo "Move json to locale folder"
mv 2.csv ${RPATH}en
mv 3.csv ${RPATH}de
mv 4.csv ${RPATH}ru
mv 5.csv ${RPATH}sv
mv 6.csv ${RPATH}sp
mv 7.csv ${RPATH}ro
mv 8.csv ${RPATH}fr
mv 9.csv ${RPATH}hu
mv 10.csv ${RPATH}cs
mv 11.csv ${RPATH}it
mv 12.csv ${RPATH}nl
mv 13.csv ${RPATH}pl
mv 14.csv ${RPATH}ua
mv 15.csv ${RPATH}fn
mv 16.csv ${RPATH}pr
mv 17.csv ${RPATH}sb
mv 18.csv ${RPATH}bg
mv 19.csv ${RPATH}tr
mv 20.csv ${RPATH}th
mv 21.csv ${RPATH}kr

rm out.csv