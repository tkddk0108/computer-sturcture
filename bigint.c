// bigint.c
// 함수파일
 
#include "bigint.h"


amhbi_t *
amhbi_init_zero ()
{
  amhbi_t *num = amhbi_init_empty(1);
  num->digits[0] = '0';
  return num;
}


amhbi_t *
amhbi_init_cpy (amhbi_t *num)
{
  amhbi_t *cpy = amhbi_init_empty(amhbi_size(num));
  cpy->sign = amhbi_sign(num);
  strncpy(cpy->digits, num->digits, amhbi_size(num));
  return cpy;
}


amhbi_t *
amhbi_init_str (char *str)
{
  // Create bignum
  uint64_t length = strlen(str);
  amhbi_t *num = amhbi_init_empty(length);

  // Check if input is negative, adjust offset and sign
  int8_t offset = 0;
  uint8_t sign = 0;
  if (str[0] == '-') {
    offset = 1;
    sign = 1;
  }
  
  // Copy digits; set initial parameters
  strncpy(num->digits, &str[offset], length);
  num->length = strlen(num->digits);
  num->sign = sign;
  return amhbi_trim(num);
}


amhbi_t *
amhbi_init_int (int64_t val)
{
  char num[128];
  sprintf(num, "%lli", (long long int)val);
  return amhbi_init_str(num);
}


amhbi_t *
amhbi_init_uint (uint64_t val)
{
  char num[128];
  sprintf(num, "%llu", (long long unsigned)val);
  return amhbi_init_str(num);
}


static amhbi_t *
amhbi_init_empty (uint64_t length)
{
  // Create struct and digit array
  amhbi_t *num = calloc(1, sizeof(amhbi_t));
  assert(num);
  num->digits = calloc(length + 1, sizeof(char));
  assert(num->digits);
  
  // Set initial parameters
  num->length = length;
  num->sign = 0;
  return num;
}


char * 
amhbi_to_str (amhbi_t *num)
{
  uint64_t length = amhbi_size(num);
  uint8_t offset = 0;
  char *str = calloc(length + 2, sizeof(char));
  if (num->sign) {
    str[0] = '-'; 
    offset = 1;
  }
  strncpy(&str[offset], num->digits, length);
  return str;
}


int64_t 
amhbi_to_int (amhbi_t *num)
{
  int64_t val = atoll(num->digits);
  return (num->sign) ? (0 - val) : val;
}


uint64_t 
amhbi_to_uint (amhbi_t *num)
{
  return atoll(num->digits);
}


static amhbi_t *
amhbi_trim (amhbi_t *num)
{
  // Count the number of leading zeros, fix length
  uint64_t zeros = 0;
  while ((num->digits[zeros] == '0' || num->digits[zeros] == 0) && 
    zeros < amhbi_size(num)) zeros++;
  num->length -= zeros;
  
  // Stop here if there was no leading zeros
  if (!zeros) return num;
  
  // Check if result will end up being zero
  if (!amhbi_size(num)) {
    free(num->digits);
    num->digits = calloc(2, sizeof(char));
    assert(num->digits);
    num->digits[0] = '0';
    num->length = 1;
    num->sign = 0;
    return num;
  }
  
  // Condense digit array and return the trimmed result
  char *tmp = calloc(amhbi_size(num) + 1, sizeof(char));
  assert(tmp);
  strncpy(tmp, &num->digits[zeros], amhbi_size(num));
  free(num->digits);
  num->digits = tmp;
  return num;
}


static amhbi_t **
amhbi_split (amhbi_t *num, uint64_t i)
{
  // Force adjust the validity of the given index
  if (i > amhbi_size(num)) i = amhbi_size(num);
  
  // Create array of split results
  amhbi_t **res = calloc(2, sizeof(amhbi_t *));
  assert(res);
  
  // Do not split if index is an extreme
  if (!i) {
    res[0] = amhbi_init_zero();
    res[1] = amhbi_init_cpy(num);
    return res;
  } else if (i == amhbi_size(num)) {
    res[0] = amhbi_init_cpy(num);
    res[1] = amhbi_init_zero();
    return res;
  }
  
  // Create each split result
  res[0] = amhbi_init_empty(i + 1);
  res[1] = amhbi_init_empty(amhbi_size(num) - i + 1);
  
  // Copy both halves of number into their respective split
  strncpy(res[0]->digits, num->digits, i);
  strncpy(res[1]->digits, &num->digits[i], amhbi_size(num) - i);
  
  // Adjust lengths and return
  res[0]->length = strlen(res[0]->digits);
  res[1]->length = strlen(res[1]->digits);
  return res;
}


static int64_t *
amhbi_split_ntt (amhbi_t *num, uint64_t n)
{
  // Create the result vector
  int64_t *res = calloc(n, sizeof(int64_t));
  assert(res);
  
  // Split the number into n parts, pad with zeros
  amhbi_t *rem = amhbi_init_cpy(num);
  uint64_t i; for (i = 0; i < n; i++) {
    amhbi_t **split = amhbi_split(rem, amhbi_size(rem) - 1);
    res[i] = amhbi_to_int(split[1]);
    amhbi_free(2, rem, split[1]);
    rem = split[0];
    free(split);
  }
  
  return res;
}


static amhbi_t *
amhbi_concat (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *res = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *tmp = amhbi_init_cpy(va_arg(args, amhbi_t *));
    char str[amhbi_size(res) + amhbi_size(tmp) + 1];
    strncpy(str, res->digits, amhbi_size(res) + 1);
    strncat(str, tmp->digits, amhbi_size(tmp) + 1);
    amhbi_free(2, tmp, res);
    res = amhbi_init_str(str);
  }
  va_end(args);
  return res;
}


void
amhbi_free (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  int i; for (i = 0; i < argc; i++) {
    amhbi_t *num = va_arg(args, amhbi_t *);
    if (num && num->digits) free(num->digits);
    if (num) free(num);
  }
  va_end(args);
}


uint64_t
amhbi_size (amhbi_t *num)
{
  return num->length;
}


uint64_t 
amhbi_size_max (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *max = va_arg(args, amhbi_t *);
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *tmp = va_arg(args, amhbi_t *);
    if (amhbi_size(tmp) > amhbi_size(max)) {
      max = tmp;
    }
  }
  va_end(args);
  return amhbi_size(max);
}


uint64_t 
amhbi_size_min (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *min = va_arg(args, amhbi_t *);
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *tmp = va_arg(args, amhbi_t *);
    if (amhbi_size(tmp) < amhbi_size(min)) {
      min = tmp;
    }
  }
  va_end(args);
  return amhbi_size(min);
}


uint8_t
amhbi_sign (amhbi_t *num)
{
  return num->sign;
}


uint8_t
amhbi_ispos (amhbi_t *num)
{
  assert(!amhbi_iszero(num));
  return (amhbi_sign(num)) ? 0 : 1;
}


uint8_t
amhbi_isneg (amhbi_t *num)
{
  assert(!amhbi_iszero(num));
  return (amhbi_sign(num)) ? 1 : 0;
}


uint8_t
amhbi_iseven (amhbi_t *num)
{
  return (num->digits[amhbi_size(num) - 1] % 2 == 0) ? 1 : 0;
} 


uint8_t
amhbi_isodd (amhbi_t *num)
{
  return (num->digits[amhbi_size(num) - 1] % 2 == 0) ? 0 : 1;
}


uint8_t 
amhbi_iszero (amhbi_t *num)
{
  return (amhbi_size(num) == 1 && num->digits[0] == '0') ? 1 : 0;
}


uint8_t 
amhbi_isunit (amhbi_t *num)
{
  return (amhbi_size(num) == 1 && num->digits[0] == '1') ? 1 : 0;
}


amhbi_t *
amhbi_abs (amhbi_t *num)
{
  amhbi_t *res = amhbi_init_cpy(num);
  res->sign = 0;
  return res;
}


amhbi_t *
amhbi_negate (amhbi_t *num)
{
  if (amhbi_iszero(num)) return num;
  amhbi_t *cpy = amhbi_init_cpy(num);
  cpy->sign = (amhbi_sign(num)) ? 0 : 1;
  return cpy;
}


amhbi_t *
amhbi_decr (amhbi_t *num)
{
  amhbi_t *decr = amhbi_init_str("1");
  amhbi_t *res = amhbi_subt(num, decr);
  free(num->digits);
  num->digits = res->digits;
  num->sign = res->sign;
  num->length = res->length;
  free(res); amhbi_free(1, decr);
  return num;
}


amhbi_t *
amhbi_incr (amhbi_t *num)
{
  amhbi_t *incr = amhbi_init_str("1");
  amhbi_t *res = amhbi_add(num, incr);
  free(num->digits);
  num->digits = res->digits;
  num->sign = res->sign;
  num->length = res->length;
  free(res); amhbi_free(1, incr);
  return num;
}


amhbi_t *
amhbi_max (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *max = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *tmp = amhbi_init_cpy(va_arg(args, amhbi_t *));
    if (amhbi_cmp(tmp, max) > 0) {
      amhbi_free(1, max);
      max = tmp;      
    } else {
      amhbi_free(1, tmp);
    }
  }
  va_end(args);
  return max;
}


amhbi_t *
amhbi_min (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *min = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *tmp = amhbi_init_cpy(va_arg(args, amhbi_t *));
    if (amhbi_cmp(tmp, min) < 0) {
      amhbi_free(1, min);
      min = tmp;
    } else {
      amhbi_free(1, tmp);
    }
  }
  va_end(args);
  return min;
}


int8_t
amhbi_cmp (amhbi_t *num1, amhbi_t *num2)
{
  // Check signs
  if (!amhbi_sign(num1) && amhbi_sign(num2)) return 1;
  if (amhbi_sign(num1) && !amhbi_sign(num2)) return -1;
  
  // Check lengths
  if (amhbi_size(num1) < amhbi_size(num2)) {
    return (amhbi_sign(num1)) ? 1 : -1;
  }
  if (amhbi_size(num1) > amhbi_size(num2)) {
    return (amhbi_sign(num1)) ? -1 : 1;
  }
  
  // Check digits
  uint64_t index = 0;
  while (index < amhbi_size(num1)) {
    if (num1->digits[index] < num2->digits[index]) {
      return (amhbi_sign(num1)) ? 1 : -1;
    } else if (num1->digits[index] > num2->digits[index]) {
      return (amhbi_sign(num1)) ? -1 : 1;
    }
    index++;
  }
  return 0;
}


amhbi_t *
amhbi_add (amhbi_t *num1, amhbi_t *num2)
{
  // Adjust operation based on the signs of the inputs
  uint8_t sign = 0;
  if (!amhbi_sign(num1) && amhbi_sign(num2)) {
    amhbi_t *rev = amhbi_negate(num2);
    amhbi_t *res = amhbi_subt(num1, rev);
    amhbi_free(1, rev);
    return res;
  } else if (amhbi_sign(num1) && !amhbi_sign(num2)) {
    amhbi_t *rev = amhbi_negate(num1);
    amhbi_t *res = amhbi_subt(num2, rev);
    amhbi_free(1, rev);
    return res;
  } else if (amhbi_sign(num1) && amhbi_sign(num1)) {
    sign = 1;
    num1 = amhbi_negate(num1);
    num2 = amhbi_negate(num2);
  }
  
  // Initialize the result, set its sign
  amhbi_t *res = amhbi_init_empty(amhbi_size_max(2, num1, num2) + 1);
  res->sign = sign;
  
  // Record right-most indices, set initial carry to zero
  uint64_t ind_0 = amhbi_size(res);
  uint64_t ind_1 = amhbi_size(num1);
  uint64_t ind_2 = amhbi_size(num2);
  uint8_t carry = 0;
  
  // Sum every digit from right to left, carry if necessary
  while (ind_0 > 0) {
    // Calculate sum
    int32_t sum = 0;
    if (ind_1 > 0 && ind_2 > 0) {
      sum = (num1->digits[ind_1 - 1] - '0') + (num2->digits[ind_2 - 1] - '0');
    } else if (ind_1 > 0 && ind_2 == 0) {
      sum = (num1->digits[ind_1 - 1] - '0');
    } else if (ind_1 == 0 && ind_2 > 0) {
      sum = (num2->digits[ind_2 - 1] - '0');
    }
    
    // Check for carry
    if (carry) {sum += 1; carry = 0;}
    
    // Check for overflow
    if (sum > 9) {sum -= 10; carry = 1;}
    
    // Set result
    res->digits[ind_0 - 1] = (sum + '0');
    
    // Move indices leftward
    if (ind_0 > 0) ind_0--;
    if (ind_1 > 0) ind_1--;
    if (ind_2 > 0) ind_2--;
  }
  
  // Free negated input if necessary
  if (sign) amhbi_free(2, num1, num2);
  
  return amhbi_trim(res);
}


amhbi_t *
amhbi_add_seq (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *sum = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *num = va_arg(args, amhbi_t *);
    amhbi_t *tmp = amhbi_add(num, sum);
    amhbi_free(1, sum);
    sum = tmp;
  }
  va_end(args);
  return sum;
}


amhbi_t *
amhbi_subt (amhbi_t *num1, amhbi_t *num2)
{
  // Adjust operation based on the signs of the inputs
  uint8_t sign = 0;
  if (amhbi_sign(num2)) {
    amhbi_t *rev = amhbi_negate(num2);
    amhbi_t *res = amhbi_add(num1, rev);
    amhbi_free(1, rev);
    return res;
  } else if (amhbi_sign(num1) && !amhbi_sign(num2)) {
    amhbi_t *rev = amhbi_negate(num1);
    amhbi_t *res = amhbi_add(rev, num2);
    res->sign = 1;
    amhbi_free(1, rev);
    return res;
  } else if (amhbi_cmp(num1, num2) < 0) {
    sign = 1;
    amhbi_t *tmp = num1;
    num1 = num2;
    num2 = tmp;
  }
  
  // Initialize the result, set its sign
  amhbi_t *res = amhbi_init_empty(amhbi_size_max(2, num1, num2) + 1);
  res->sign = sign;
  
  // Record right-most indices, set initial borrow to zero
  uint64_t ind_0 = res->length;
  uint64_t ind_1 = num1->length;
  uint64_t ind_2 = num2->length;
  int8_t borrow = 0;
  
  // Subtract every digit right to left, borrow if necessary
  while (ind_0 > 0) {
    // Calculate difference
    int32_t diff = 0;
    if (ind_1 > 0 && ind_2 > 0) {
      if ((num1->digits[ind_1 - 1] - borrow) < num2->digits[ind_2 - 1]) {
        diff = ((num1->digits[ind_1 - 1] - '0') - borrow + 10) - 
                (num2->digits[ind_2 - 1] - '0');
        borrow = 1;
      } else {
        diff = (num1->digits[ind_1 - 1] - '0' - borrow) - 
               (num2->digits[ind_2 - 1] - '0');
        borrow = 0;
      }
    } else if (ind_1 > 0 && ind_2 == 0) {
      diff = (num1->digits[ind_1 - 1] - '0') - borrow;
      if (diff < 0) {
        borrow = 1;
        diff += 10;
      } else {
        borrow = 0;
      }
    }
    
    // Set result
    res->digits[ind_0 - 1] = (diff + '0');
    
    // Move indices leftward
    if (ind_0 > 0) ind_0--;
    if (ind_1 > 0) ind_1--;
    if (ind_2 > 0) ind_2--;
  }
  
  return amhbi_trim(res);
}


amhbi_t *
amhbi_subt_seq (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *diff = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *num = va_arg(args, amhbi_t *);
    amhbi_t *tmp = amhbi_subt(diff, num);
    amhbi_free(1, diff);
    diff = tmp;
  }
  va_end(args);
  return diff;
}


amhbi_t *
amhbi_mult (amhbi_t *num1, amhbi_t *num2)
{
  // For numbers smaller than 750 digits, use long multiplication
  if (amhbi_size(num1) < 750 && amhbi_size(num2) < 750) {
    return amhbi_mult_long(num1, num2);
  }
  // For numbers smaller than 10000 digits, use the Karatsuba algorithm
  //if (amhbi_size(num1) < 5000 && amhbi_size(num2) < 5000) {
    return amhbi_mult_karatsuba(num1, num2);
  //}
  // For anything larger, use fast Fourier transforms
  //return amhbi_mult_fft(num1, num2);
}


amhbi_t *
amhbi_mult_seq (int argc, ...)
{
  assert(argc > 0);
  va_list args;
  va_start(args, argc);
  amhbi_t *prod = amhbi_init_cpy(va_arg(args, amhbi_t *));
  int i; for (i = 1; i < argc; i++) {
    amhbi_t *num = va_arg(args, amhbi_t *);
    amhbi_t *tmp = amhbi_mult(num, prod);
    amhbi_free(1, prod);
    prod = tmp;
  }
  va_end(args);
  return prod;
}


amhbi_t *
amhbi_mult_pow10 (amhbi_t *num, uint64_t p)
{
  assert(p >= 0);
  if (!p) return amhbi_init_cpy(num);
  amhbi_t *cpy = amhbi_init_empty(amhbi_size(num) + p);
  cpy->sign = amhbi_sign(num);
  strncpy(cpy->digits, num->digits, amhbi_size(num));
  while (p) {
    cpy->digits[amhbi_size(num) + p - 1] = '0';
    p--;
  }
  return amhbi_trim(cpy);
}


static amhbi_t *
amhbi_mult_long (amhbi_t *num1, amhbi_t *num2)
{
  // Initialize result sum, set its sign
  amhbi_t *sum = amhbi_init_zero();
  
  // Determine outer loop index, set inital power of ten to zero
  uint64_t ind_2 = amhbi_size(num2);
  uint64_t power = 0;
  
  // The outer loop is each digit of the multiplier
  while (ind_2 > 0) {
    amhbi_t *step = amhbi_init_empty(amhbi_size(num1) + 2);
    uint64_t ind_s = amhbi_size(num1) + 2;
    uint64_t ind_1 = amhbi_size(num1);
    uint64_t mult = num2->digits[ind_2 - 1] - '0';
    uint64_t carry = 0;
    
    // The inner loop is each digit of the multiplicand
    while (ind_s > 0) {
      // Calculate the product, add carry from previous iterations
      uint64_t prod = 0;
      if (ind_1 > 0) prod = ((num1->digits[ind_1 - 1] - '0') * mult);
      prod += carry;
      carry = 0;
      
      // Check for new carry
      if (prod > 9) {carry = prod / 10; prod = prod % 10;} 
      
      // Set result and decrement inner indices
      step->digits[ind_s - 1] = prod + '0';
      if (ind_1) ind_1--;
      if (ind_s) ind_s--;
    }
    
    // Multiply this step by 10^power, add the result to the total sum
    amhbi_t *tmp_step = amhbi_mult_pow10(step, power);
    amhbi_t *tmp_sum = amhbi_add(sum, tmp_step);
    amhbi_free(3, sum, step, tmp_step); sum = tmp_sum;
    
    if (ind_2) ind_2--;
    power++;
  }
  
  // Set sign and return
  sum->sign = (amhbi_sign(num1) == amhbi_sign(num2)) ? 0 : 1;
  return amhbi_trim(sum);
}


static amhbi_t *
amhbi_mult_karatsuba (amhbi_t *num1, amhbi_t *num2)
{
  // Base case
  if (amhbi_size(num1) == 1 || amhbi_size(num2) == 1) {
    return amhbi_mult_long(num1, num2);
  }

  // Figure out split size
  uint64_t m = (amhbi_size_max(2, num1, num2) + 1) / 2;
  uint64_t s1 = (amhbi_size(num1) > m) ? amhbi_size(num1) - m : 0;
  uint64_t s2 = (amhbi_size(num2) > m) ? amhbi_size(num2) - m : 0;
  
  // Split each number
  amhbi_t **split1 = amhbi_split(num1, s1);
  amhbi_t **split2 = amhbi_split(num2, s2);
  amhbi_t *h1 = split1[0];
  amhbi_t *h2 = split2[0];
  amhbi_t *l1 = split1[1];
  amhbi_t *l2 = split2[1];
  free(split1); free(split2);
  
  // Recurse on split numbers
  amhbi_t *z2 = amhbi_mult(h1, h2);
  amhbi_t *z0 = amhbi_mult(l1, l2);
  amhbi_t *z1_0_0 = amhbi_add(h1, l1);
  amhbi_t *z1_0_1 = amhbi_add(h2, l2);
  amhbi_t *z1_0 = amhbi_mult(z1_0_0, z1_0_1);
  amhbi_t *z1 = amhbi_subt_seq(3, z1_0, z2, z0);
  
  // Solve polynomial
  amhbi_t *res2 = amhbi_mult_pow10(z2, m * 2);
  amhbi_t *res1 = amhbi_mult_pow10(z1, m);
  amhbi_t *res0 = amhbi_init_cpy(z0);
  amhbi_t *res = amhbi_add_seq(3, res2, res1, res0);
  
  // Free intermediate results
  amhbi_free(4, h1, h2, l1, l2);
  amhbi_free(9, z2, z0, z1_0_0, z1_0_1, z1_0, z1, res2, res1, res0);

  // Set sign and return
  res->sign = (amhbi_sign(num1) == amhbi_sign(num2)) ? 0 : 1; 
  return amhbi_trim(res);
}


/*
 * TODO: Determine correct root of unity and modulus at runtime
 */

static amhbi_t *
amhbi_mult_fft (amhbi_t *num1, amhbi_t *num2)
{
  // Split numbers into zero padded vectors of size (n = 2^x) > s
  uint64_t s = amhbi_size_max(2, num1, num2);
  uint64_t n = pow(2, ceil(log((s + 1))/log(2)));
  int64_t *one = amhbi_split_ntt(num1, n);
  int64_t *two = amhbi_split_ntt(num2, n);

  // Choose modulus and omega
  long double w = 85;
  int64_t m = 337;
  
  // Apply a NTT to each vector
  int64_t *one_ntt = amhbi_fft(one, w, n, m);
  int64_t *two_ntt = amhbi_fft(two, w, n, m);
  
  // Pointwise multiplication (mod m)
  int64_t *ires = calloc(n, sizeof(int64_t));
  assert(ires);
  uint64_t i; for (i = 0; i < n; i++) {
    ires[i] = (one_ntt[i] * two_ntt[i]) % m;
  }
  
  // Apply an inverse NTT to get the convolution
  int64_t *res = amhbi_ifft(ires, w, n, m);
  
  // Restore the polynomial to integer form (x = base 10)
  amhbi_t *sum = amhbi_init_int(res[0]);
  for (i = 1; i < n; i++) {
    amhbi_t *tmp1 = amhbi_init_int(res[i]);
    amhbi_t *tmp2 = amhbi_mult_pow10(tmp1, i);
    amhbi_t *tmp_sum = amhbi_add(tmp2, sum);
    amhbi_free(3, sum, tmp1, tmp2);
    sum = tmp_sum;
  }

  // Adjust sign and return
  sum->sign = (amhbi_sign(num1) == amhbi_sign(num2)) ? 0 : 1;
  return sum;
}


static int64_t *
amhbi_fft (int64_t *a, long double w, uint64_t n, uint64_t m)
{
  // Base case
  if (n == 1) return a;
  
  // Split vector a into even and odd halves
  int64_t *a_even = calloc((n / 2), sizeof(int64_t));
  int64_t *a_odd = calloc((n / 2), sizeof(int64_t));
  uint64_t i = 0; uint64_t ev; for (ev = 0; ev < n; ev += 2) {
    a_even[i++] = a[ev];
  }
  i = 0; uint64_t od; for (od = 1; od < n; od += 2) {
    a_odd[i++] = a[od];
  }  
  
  // Recurse on halves
  int64_t *y_even = amhbi_fft(a_even, (w * w), (n / 2), m);
  int64_t *y_odd = amhbi_fft(a_odd, (w * w), (n / 2), m);
  
  // Initialize result vector
  int64_t *y = calloc(n, sizeof(int64_t));
  
  // Combine; x = w^i; apply modulus to result if requested
  long double x = 1.0L;
  for (i = 0; i < (n / 2); i++) {
    y[i] = (y_even[i]) + (x * (y_odd[i]));
    y[i+n/2] = (y_even[i]) - (x * (y_odd[i]));
    // Mod the results
    y[i] = (y[i] < 0) ? m - (abs(y[i]) % m) : y[i] % m;
    y[i+n/2] = (y[i+n/2] < 0) ? m - (abs(y[i+n/2]) % m) : y[i+n/2] % m;
    x *= w;
  }
  
  return y;
}


static int64_t *
amhbi_ifft (int64_t *a, long double w, uint64_t n, uint64_t m)
{
  int64_t *res1 = amhbi_fft(a, w, n, m);
  int64_t *res2 = calloc(n, sizeof(int64_t));
  assert(res2);
  int64_t r = amhbi_ifft_pow(n, (m - 2) % m, m) % m;
  res2[0] = ((res1[0] % m) * (r % m)) % m;
  uint8_t i; for (i = 1; i < n; i++) {
    res2[i] = ((res1[n - i] % m) * (r % m)) % m;
  }
  return res2;
}


static int64_t 
amhbi_ifft_pow (uint64_t n, int64_t p, uint64_t m)
{
  int64_t res = 1;
  uint64_t i; for (i = 0; i < p; i++) {
    res = ((res % m) * (n % m)) % m;
  }
  return res;
}


amhbi_t *
amhbi_pow (amhbi_t *num, amhbi_t *p)
{
  amhbi_t *res = amhbi_init_str("1");
  amhbi_t *a = amhbi_init_cpy(num);
  amhbi_t *b = amhbi_init_cpy(p);

  // Perform iterated exponentiation by squaring
  while (!amhbi_iszero(b)) {
    if (amhbi_isodd(b)) {
      // res *= a
      amhbi_t *tmp = amhbi_mult(res, a); 
      amhbi_free(1, res); 
      res = tmp;
      // b -= 1
      amhbi_decr(b);
    }
    // b /= 2
    amhbi_t *tmp = amhbi_half(b); 
    amhbi_free(1, b); 
    b = tmp;
    // a *= a
    tmp = amhbi_mult(a, a); 
    amhbi_free(1, a); 
    a = tmp;
  }
  
  amhbi_free(2, a, b);
  return res;
}


static amhbi_t **
amhbi_div (amhbi_t *num1, amhbi_t *num2)
{
  // Create array of results (quotient, remainder)
  amhbi_t **res = calloc(2, sizeof(amhbi_t *));
  assert(res);
  
  amhbi_t *tmpnum1 = amhbi_abs(num1);
  amhbi_t *tmpnum2 = amhbi_abs(num2);
  amhbi_t *quo = amhbi_init_zero();
  amhbi_t *rem = amhbi_init_zero();
  amhbi_t **split = amhbi_split(tmpnum1, 1);
  amhbi_t *down = amhbi_init_cpy(split[0]);
  amhbi_t *left = amhbi_init_cpy(split[1]);
  uint64_t p = amhbi_size(tmpnum1);
  if (p) p--;
  
  // Loop until there is nothing left to divide into
  do {
    // Keep pulling down digits until down is larger than num2
    while (amhbi_cmp(down, tmpnum2) < 0) {
      amhbi_free(2, split[0], split[1]); free(split);
      split = amhbi_split(left, 1);
      amhbi_t *tmp = amhbi_concat(2, down, split[0]);
      amhbi_free(2, down, left);
      down = tmp;
      left = amhbi_init_cpy(split[1]);
      if (p) p--;
      if (amhbi_iszero(left)) break;
    }
  
    // Calculate quotient, set remainder as next down
    amhbi_t *tmp = amhbi_quo_slow(down, tmpnum2);
    amhbi_t *tmp_pow = amhbi_mult_pow10(tmp, p);
    amhbi_t *tmp_quo = amhbi_add(quo, tmp_pow);
    amhbi_free(4, tmp, tmp_pow, quo, rem);
    quo = tmp_quo;
    rem = amhbi_rem_slow(down, tmpnum2);
    amhbi_free(1, down); 
    down = amhbi_init_cpy(rem);
  } while (!amhbi_iszero(left));
  amhbi_free(6, split[0], split[1], down, left, tmpnum1, tmpnum2);
  free(split);
  
  // Set quotient (+ sign) and remainder
  res[0] = quo; res[1] = rem;
  res[0]->sign = (amhbi_sign(num1) == amhbi_sign(num2)) ? 0 : 1;
  return res;
}


amhbi_t *
amhbi_quo (amhbi_t *num1, amhbi_t *num2)
{
  assert(!amhbi_iszero(num2));
  if (!amhbi_cmp(num1, num2)) return amhbi_init_int(1);
  amhbi_t **res = amhbi_div(num1, num2);
  amhbi_t *quo = res[0]; amhbi_t *rem = res[1];
  amhbi_free(1, rem); free(res);
  return quo;
}


static amhbi_t *
amhbi_quo_slow (amhbi_t *num1, amhbi_t *num2)
{
  assert(!amhbi_iszero(num2));
  if (amhbi_cmp(num1, num2) < 0) return amhbi_init_zero();
  
  // Perform iterative division by subtraction
  amhbi_t *quo = amhbi_init_zero();
  amhbi_t *rem = amhbi_init_cpy(num1);
  while (amhbi_cmp(rem, num2) >= 0) {
    amhbi_t *tmp = amhbi_subt(rem, num2);
    amhbi_free(1, rem);
    rem = tmp;
    amhbi_incr(quo);
  }
  
  amhbi_free(1, rem);
  return quo;
}


amhbi_t *
amhbi_rem (amhbi_t *num1, amhbi_t *num2)
{
  assert(!amhbi_iszero(num2));
  amhbi_t **res = amhbi_div(num1, num2);
  amhbi_t *quo = res[0]; amhbi_t *rem = res[1];
  amhbi_free(1, quo); free(res);
  
  // Adjust result based on signs
  if (amhbi_isneg(num1) && amhbi_isneg(num2)) {
    amhbi_t *tmp = amhbi_negate(rem);
    amhbi_free(1, rem);
    return tmp;
  } else if (amhbi_isneg(num1) && !amhbi_isneg(num2)) {
    amhbi_t *tmp = amhbi_subt(num2, rem);
    amhbi_free(1, rem);
    return tmp;
  } else if (!amhbi_isneg(num1) && amhbi_isneg(num2)) {
    amhbi_t *tmp = amhbi_add(num2, rem);
    amhbi_free(1, rem);
    return tmp;
  } else {
    if (amhbi_cmp(num1, num2) < 0) return amhbi_init_cpy(num1);
    return rem;
  }
}


static amhbi_t *
amhbi_rem_slow (amhbi_t *num1, amhbi_t *num2)
{
  assert(!amhbi_iszero(num2));
  if (amhbi_cmp(num1, num2) < 0) return amhbi_init_cpy(num1);
  
  // Perform iterative division by subtraction
  amhbi_t *quo = amhbi_init_zero();
  amhbi_t *rem = amhbi_init_cpy(num1);
  while (amhbi_cmp(rem, num2) >= 0) {
    amhbi_t *tmp = amhbi_subt(rem, num2);
    amhbi_free(1, rem);
    rem = tmp;
    amhbi_incr(quo);
  }
  
  amhbi_free(1, quo);
  return rem;
}


amhbi_t *
amhbi_half (amhbi_t *num)
{
  // Copy num, add a zero to the left; initialize result
  amhbi_t *cpy = amhbi_init_empty(amhbi_size(num) + 1);
  cpy->digits[0] = '0';
  strncpy(&cpy->digits[1], num->digits, amhbi_size(num));
  amhbi_t *res = amhbi_init_empty(amhbi_size(num));
  
  // Loop through the number, save results
  uint64_t index_c = 0;
  uint64_t index_r = 0;
  while (index_c <= amhbi_size(num)) {
    uint8_t even = (cpy->digits[index_c] % 2 == 0) ? 1 : 0;
    switch (cpy->digits[++index_c]) {
      case '0':
      case '1': res->digits[index_r] = (even) ? '0' : '5'; break;
      case '2':
      case '3': res->digits[index_r] = (even) ? '1' : '6'; break;
      case '4':
      case '5': res->digits[index_r] = (even) ? '2' : '7'; break;
      case '6':
      case '7': res->digits[index_r] = (even) ? '3' : '8'; break;
      case '8':
      case '9': res->digits[index_r] = (even) ? '4' : '9'; break;
    }
    index_r++;
  }
  amhbi_free(1, cpy);
  return amhbi_trim(res);
}


amhbi_t *
amhbi_gcd (amhbi_t *num1, amhbi_t *num2)
{
  amhbi_t *a = amhbi_init_cpy(num1);
  amhbi_t *b = amhbi_init_cpy(num2);
  while (amhbi_cmp(a, b) != 0) {
    if (amhbi_cmp(a, b) > 0) {
      amhbi_t *tmp = amhbi_subt(a, b);
      amhbi_free(1, a);
      a = tmp;
    } else {
      amhbi_t *tmp = amhbi_subt(b, a);
      amhbi_free(1, b);
      b = tmp;
    }
  }
  amhbi_free(1, b);
  return a;
}
