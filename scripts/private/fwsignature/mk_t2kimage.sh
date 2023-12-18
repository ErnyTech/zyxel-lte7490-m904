#!/bin/sh
################################################################################
# This script generates t2k images(that includes image header) from the standard
# binary file. It can generates header with NULL, SHA256, RSA signatures, and
# key inbuilt to the header. This script internally invoke header generation
# application with corresponding parameters.
#
# Example Usage:
# To generate firmware image with NULL hash
# 	mk_t2kimage.sh null firmware.bin
#
# To generate firmware image with SHA256
#	mk_t2kimage.sh sha256 firmware.bin
#
# To generate firmware image with RSA signature and key inbuilt to the header
#	mk_t2kimage.sh rsa private.pem firmware.bin
#
# OpenSSL command to generate new private key
#	openssl genrsa -out private.pem 2048
#
# Note: RSA key size is determined from the given private key.
################################################################################

usage()
{
	echo "Usage..."
	echo "$0 <mode> <fw_file> <options>"
	echo " <mode>	    - null, sha256, rsa"
	echo "   null       - Place NULL hash in the header"
	echo "   sha256     - Place SHA256 hash in the header"
	echo "   rsa        - Place RSA signature in the header"
	echo "                private key file name is required for options"
	echo " <fw_file>    - Firmware image"
	echo " Ex: ./mk_t2kimage.sh sha256 t2kboot_slave.bin"
	echo " EX: ./mk_t2kimage.sh rsa t2kboot_slave.bin privatekey-1k.pem"
}

clean_exit()
{
	if [ -e "$public_modulus" ]
	then
		rm $public_modulus
	fi
	if [ -e "$sig_file" ]
	then
		rm $sig_file
	fi
	if [ -e "$hash_file" ]
	then
		rm $hash_file
	fi
	exit $1
}

check_image()
{
	if [ "$1" = "" ]; then
		usage
		clean_exit 1
	fi

	if [ ! -e $1 ]; then
		echo "Firmware file not found "$1
		clean_exit 1
	fi
}

#This function calculates T2K custom hash on given input file
# $1 input file name
# $2 file name to store hash
calculate_t2k_hash()
{
	img_size=$(stat -c%s $1)
	part_size=$(((60*1024)))
	tmp_hash_file=`mktemp` || exit 1

	#create  one temp filename to use as prefix
	prefix=`mktemp` && rm $prefix

	echo "  Calculating custom SHA256 hash on $1 size $img_size"
	split -b $part_size $1 $prefix

	for part_file in `ls $prefix*`
	do
#		echo "$(stat -c%s $part_file)"
		openssl	dgst -binary -sha256 < $part_file >> $tmp_hash_file
		rm $part_file
	done

	if [ $img_size -gt $part_size ]; then
		openssl dgst -binary -sha256 < $tmp_hash_file >	$2
	else
		cat $tmp_hash_file > $2
	fi
	cp $tmp_hash_file ./image_hash
	rm $tmp_hash_file
}

calculate_image_hash()
{
	echo "  Calculating image with SHA256 hash on $1"
	echo $2
	openssl dgst -binary -sha256 < $1 >	$2
	cp $2 ./image_hash
}
#PWD=`pwd`
dir=`pwd` || exit 1
echo `pwd`
#echo $(PWD)
MKSIGN="$dir/mksign"
timestamp=`date +%s` || exit 1

# $1 mode
# $2 image
# $3... options
image=$2

check_image $image

case "$1" in

	null)
		echo "Generating T2K image with NULL hash"

		# build final image with header
		echo "$MKSIGN nonlegacy $timestamp $image null"
		$MKSIGN nonlegacy $timestamp $image null
		;;

	sha256)
		echo "Generating T2K image with SHA256.."
		hash_file=`mktemp` || exit 1

		#Calculate SHA256 hash on fw file.
		# calculate_t2k_hash $image $hash_file
		calculate_image_hash $image $hash_file
       	if [ $? != 0 ]; then
			echo "Error while calculating hash on $image"
			clean_exit 1
        fi

		# build final image with header
		echo "$MKSIGN nonlegacy $timestamp $image sha256 $hash_file"
		$MKSIGN nonlegacy $timestamp $image sha256 $hash_file
		;;

	rsa)
		echo "Generating T2K image with RSA signature .."
		public_modulus=`mktemp` || clean_exit 1
		sig_file=`mktemp` || clean_exit 1
		hash_file=`mktemp` || clean_exit 1
		private_key=$3
		publickey="$3.modulus"
		publickey_hashfile="$3.publickey_hash"

		if [ ! -e $private_key ]; then
			echo "Private key file not found '$private_key'"
	        clean_exit 1
		fi

		echo "Generating public key"
		echo "  extract the public modulus(N) from the private key"
		openssl rsa -in $private_key -modulus | awk -F"=" '{if ($1 == "Modulus"){print $2}}' > $public_modulus
		xxd -r -p $public_modulus $publickey

		echo "  Calculate SHA256 hash on public key file...."
		openssl dgst -binary -sha256 < $publickey > $publickey_hashfile
		#rm $publickey
		echo "  Generated hash on public key : $publickey_hashfile"

		echo "\nGenerating signed image"
		# Calculate SHA256 hash on fw file.
		# calculate_t2k_hash $image $hash_file
		calculate_image_hash $image $hash_file
       	if [ $? != 0 ]; then
			echo "Error while calculating hash on $image"
			clean_exit 1
        fi

		# RSA Sign the hash using private key
		echo "openssl rsautl -sign -inkey $private_key -keyform PEM -pkcs -in $hash_file -out $sig_file"
		openssl rsautl -sign -inkey $private_key -keyform PEM -pkcs -in $hash_file -out $sig_file
		
		# build final image with header
		$MKSIGN nonlegacy $timestamp $image rsa $sig_file $public_modulus insertkey
		#cp $sig_file ./sig_file

		echo "  Done!!\n"
		echo "Public key Hash file : $publickey_hashfile"
		#echo "Signed image : $image.rsa\n"
		rm ./image_hash
		;;

	*)
		usage
		exit 1
		;;

esac

clean_exit 0

