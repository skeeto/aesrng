#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <gcrypt.h>

int main (int argc, char **argv)
{
  size_t bytes = 1;
  if (argc > 1)
    bytes = atoi (argv[1]);
  
  /* Initialize the cipher */
  gcry_cipher_hd_t aes;
  gcry_cipher_open(&aes, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CBC, 0);
  
  /* Initialize the cipher key and iv */
  char key[32], iv[16];
  gcry_randomize (&key[0], 32, GCRY_STRONG_RANDOM);
  gcry_create_nonce (&iv[0], 16);
  gcry_cipher_setkey (aes, key, 32);
  gcry_cipher_setiv (aes, iv, 16);
  
  unsigned char in[16], out[16];
  gcry_create_nonce (&out[0], 16);

  /* Initialize the counter */
  unsigned char counter_iv[16];
  gcry_create_nonce (&counter_iv[0], 16);
  gcry_mpi_t counter = gcry_mpi_new (16);
  gcry_mpi_scan (&counter, GCRYMPI_FMT_USG, &counter_iv[0], 16, NULL);
  
  while (bytes)
    {
      gcry_mpi_add_ui (counter, counter, 1);
      gcry_mpi_print (GCRYMPI_FMT_USG, &in[0], 16, NULL, counter);
      int i;
      for (i = 0; i < 16; i++)
	in[i] ^= out[i];
      gcry_cipher_encrypt (aes, &out[0], 16, &in[0], 16);
      
      /* Print out the bytes. */
      size_t nout = 16;
      if (nout > bytes)
	nout = bytes;     
      fwrite (&out[0], nout, 1, stdout);
      
      bytes -= nout;
    }
  
  return EXIT_SUCCESS;
}