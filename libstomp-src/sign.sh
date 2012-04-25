# http://www.codealias.info/technotes/openssl_rsa_sign_and_verify_howto
echo -e -n "--- ping\n" > data.txt
openssl dgst -sha1 -out hash < data.txt 
cat hash
#cat hash.s|cut -d ' ' -f 2 > hash
#openssl dgst < data.txt > hash

# First attempt at signing?
echo "----- RSA signed"
openssl rsautl -sign -inkey ../../john-private.pem -keyform PEM -in hash  > signature
openssl rsautl -verify -inkey ../../john-public.pem -keyform PEM -pubin -in signature > verified
cat hash
cat verified
cat signature | openssl base64

# SHA1 signing


echo "----- SHA1 signed"
openssl sha1 -sign ../../john-private.pem data.txt > sig.txt
cat sig.txt|openssl base64
openssl sha1 -verify ../../john-public.pem -signature sig.txt data.txt 
