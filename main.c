//main.c
#include "bigint.h"


int 
main()
{
  // 사용할 bigint 만들기
  amhbi_t *one              = amhbi_init_int(1);
  amhbi_t *negativetwo      = amhbi_init_str("-2");
  amhbi_t *fourquadrillion  = amhbi_init_str("4000000000000000");
  amhbi_t *morethan64bits   = amhbi_init_str("18446744073709551617");
  amhbi_t *nine             = amhbi_init_str("9");

  // 연산 5가지
  amhbi_t *result1 = amhbi_add_seq(3, one, negativetwo, morethan64bits);
  amhbi_t *result2 = amhbi_mult(negativetwo, morethan64bits);
  amhbi_t *result3 = amhbi_quo(morethan64bits, fourquadrillion);
  amhbi_t *result4 = amhbi_rem(morethan64bits, fourquadrillion);
  amhbi_t *result5 = amhbi_pow(morethan64bits, nine);
  amhbi_free(5, one, negativetwo, fourquadrillion, morethan64bits, nine);

  // 전환과 결과보이기
  printf("result1 = %s\n", amhbi_to_str(result1));
  printf("result2 = %s\n", amhbi_to_str(result2));
  printf("result3 = %s\n", amhbi_to_str(result3));
  printf("result4 = %s\n", amhbi_to_str(result4));
  printf("result5 = %s\n", amhbi_to_str(result5));

  amhbi_free(5, result1, result2, result3, result4, result5);
}
