/*  gcc ./openssl_sign.c -lssl */

#include <stdio.h>
#include <string.h>
#include <error.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>

int pass_cb( char *buf, int size, int rwflag, void *u )
{
  int len;
  char tmp[1024];
  printf( "Enter pass phrase for '%s': ", (char*)u );
  scanf( "%s", tmp );
  len = strlen( tmp );

  if ( len <= 0 ) return 0;
  if ( len > size ) len = size;

  memset( buf, '\0', size );
  memcpy( buf, tmp, len );
  return len;
}

RSA* getRsaFp( const char* rsaprivKeyPath )
{
  FILE* fp;
  fp = fopen( rsaprivKeyPath, "r" );
  if ( fp == 0 ) {
    fprintf( stderr, "Couldn't open RSA priv key: '%s'. %s\n",
             rsaprivKeyPath, strerror(errno) );
    exit(1);
  }

  RSA *rsa = 0;
  rsa = RSA_new();
  rsa = PEM_read_RSAPrivateKey(fp, 0, pass_cb, (char*)rsaprivKeyPath);
  fclose( fp );
  return rsa;
}

int main( int argc, char* argv[] )
{
  if ( argc != 2 ) {
    fprintf( stderr, "Usage: %s <text to sign>\n", argv[0] );
    exit( 1 );
  }
  const char *clearText = "--- ping\n";
  //const char *clearText = argv[1];

  char rsaprivKeyPath[1024];
  //sprintf( rsaprivKeyPath, "%s/.ssh/id_rsa",  getenv ("HOME") );
  sprintf( rsaprivKeyPath, "/home/pdebois/%s.pem",  "john-private" );
  //sprintf( rsaprivKeyPath, "/home/pdebois/%s.pem",  "server-private" );

  SSL_load_error_strings();

  OpenSSL_add_all_algorithms();
  OpenSSL_add_all_ciphers();
  OpenSSL_add_all_digests();

  EVP_PKEY *evpKey = 0;
  evpKey = EVP_PKEY_new();

  RSA *rsa = 0;
  rsa = getRsaFp( rsaprivKeyPath );

  EVP_PKEY_set1_RSA( evpKey, rsa );

  EVP_MD_CTX* ctx = 0;
  ctx = EVP_MD_CTX_create();
  EVP_SignInit_ex( ctx, EVP_sha1(), 0 );
  EVP_SignUpdate( ctx, clearText, strlen( clearText ) );

  const int MAX_LEN = 1024;
  unsigned char sig[MAX_LEN];
  unsigned int sigLen;
  memset(sig, 0, MAX_LEN);

  EVP_SignFinal( ctx, sig, &sigLen, evpKey );

  printf( "%s", sig );

  EVP_MD_CTX_destroy( ctx );
  RSA_free( rsa );
  EVP_PKEY_free( evpKey );
  ERR_free_strings();
  return 0;
}

