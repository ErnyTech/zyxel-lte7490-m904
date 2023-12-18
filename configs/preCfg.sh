#!/bin/sh
P="LTE7490-M904_Generic"
echo "P=$P"
echo "copy $TOPDIR/configs/$P.defconfig to .config!"
cp $TOPDIR/configs/$P.defconfig $TOPDIR/.config
