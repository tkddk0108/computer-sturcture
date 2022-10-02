/* main.c
 *
 * Test out a few amh_bigint functions.
 *
 * Author: Adam M. Holmes
 */

#include "amh_bigint.h"


int 
main()
{
  // Create a few bigints
  amhbi_t *one              = amhbi_init_int(1);
  amhbi_t *negativetwo      = amhbi_init_str("-2");
  amhbi_t *fourquadrillion  = amhbi_init_str("4000000000000000");
  amhbi_t *morethan64bits   = amhbi_init_str("18446744073709551617");
  amhbi_t *nine             = amhbi_init_str("9");

  // Do some simple math
  amhbi_t *result1 = amhbi_add_seq(3, one, negativetwo, morethan64bits);
  amhbi_t *result2 = amhbi_mult(negativetwo, morethan64bits);
  amhbi_t *result3 = amhbi_quo(morethan64bits, fourquadrillion);
  amhbi_t *result4 = amhbi_rem(morethan64bits, fourquadrillion);
  amhbi_t *result5 = amhbi_pow(morethan64bits, nine);
  amhbi_free(5, one, negativetwo, fourquadrillion, morethan64bits, nine);

  // Convert and print results
  printf("result1 = %s\n", amhbi_to_str(result1));
  printf("result2 = %s\n", amhbi_to_str(result2));
  printf("result3 = %s\n", amhbi_to_str(result3));
  printf("result4 = %s\n", amhbi_to_str(result4));
  printf("result5 = %s\n", amhbi_to_str(result5));

  // Free our bigint results
  amhbi_free(5, result1, result2, result3, result4, result5);
}
