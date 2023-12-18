#!/bin/sh
#
#This script is used to double check the certificate status during boot stage
#

#Check if OpenSSL is exist:
check_openssl()
{
	if !(`openssl version |grep -q OpenSSL`) ; then
		echo "openssl is not available, skip!"
		exit 1
	fi
}

#Check if CA direcory is exist:
check_CA_dir()
{
	if [ -d /data/CA ]; then
		echo "CA directory is exist"
	else
		echo "CA directory is not exist, skip check cert!"
		exit 1
	fi
}
#Check if two CA file is exist:
check_CA_file()
{
	if [ -s /data/CA/newreq.crt ]; then
		if [ -s /data/CA/newreq.key ]; then
			echo "2 CA file is exist"
		else
			echo "==============Warning!!!!!!Key file missing, Remove CA direcotry!!=============="
			rm -r /data/CA
			sync	
			exit 1
		fi
	else
		echo "==============Warning!!!!!!Cert file missing, Remove CA direcotry!!=============="
		rm -r /data/CA
		sync	
		exit 1	
	fi
}

#Check if the Cert match the key:
check_cert_key()
{
	CERT=$(openssl x509 -noout -modulus -in /data/CA/newreq.crt | openssl md5)
	KEY=$(openssl rsa -noout -modulus -in /data/CA/newreq.key | openssl md5)
	if [ "$CERT" == "$KEY" ];then
		echo "CERT is match the Key!"
		chmod 444 /data/CA/newreq.crt
		chmod 400 /data/CA/newreq.key
		sync
	else
		echo "==============Warning!!!!!! CERT is not match the Key , Remove CA directory!!!!=============="
		rm -r /data/CA
		sync
	fi
}

check_openssl
check_CA_dir
check_CA_file
check_cert_key
