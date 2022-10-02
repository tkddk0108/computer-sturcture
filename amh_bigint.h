/* amh_bigint.h:
 *
 * A portable arbitrary-precision integer arithmetic library written in C as a
 * personal exercise. Integers are stored as arrays of chars, each representing
 * a digit. Various arithmetic functions are implemented for use on these 
 * integers.
 *
 * Author: Adam M. Holmes
 */

#ifndef AMHBI_H
#define AMHBI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>


/*
 * Bigint struct
 */

typedef struct 
{
  char *digits;
  uint8_t sign;
  uint64_t length;
} amhbi_t;


/*
 * Initialization functions; use these to convert to bigints
 */

/* Returns a zero valued bigint */
amhbi_t * amhbi_init_zero ();

/* Returns an exact copy of num */
amhbi_t * amhbi_init_cpy (amhbi_t *num);

/* Returns the bigint representation of the given string */
amhbi_t * amhbi_init_str (char *str);

/* Returns the bigint representation of the given signed int */
amhbi_t * amhbi_init_int (int64_t val);

/* Returns the bigint representation of the given unsigned int */
amhbi_t * amhbi_init_uint (uint64_t val);


/*
 * Arithmetic functions; these always return new bigints
 */

/* Sum num1 and num2 */
amhbi_t * amhbi_add (amhbi_t *num1, amhbi_t *num2);

/* Sum a sequence of bigints */
amhbi_t * amhbi_add_seq (int argc, ...);

/* Subtract num2 from num1 */
amhbi_t * amhbi_subt (amhbi_t *num1, amhbi_t *num2);

/* Subtract a sequence of bigints from one another */
amhbi_t * amhbi_subt_seq (int argc, ...);

/* Multiply num1 by num2 */
amhbi_t * amhbi_mult (amhbi_t *num1, amhbi_t *num2);

/* Multiply a sequence of bigints together */
amhbi_t * amhbi_mult_seq (int argc, ...);

/* Multiply num1 by the given power of 10 */
amhbi_t * amhbi_mult_pow10 (amhbi_t *num, uint64_t p);

/* Raise num to the p power */
amhbi_t * amhbi_pow (amhbi_t *num, amhbi_t *p);

/* Divide num1 by num2; return the quotient */
amhbi_t * amhbi_quo (amhbi_t *num1, amhbi_t *num2);

/* Divide num1 by num2; return the remainder */
amhbi_t * amhbi_rem (amhbi_t *num1, amhbi_t *num2);

/* Quickly divide num by two; return the quotient */
amhbi_t * amhbi_half (amhbi_t *num);

/* Returns the greatest common divisor of num1 and num2 */
amhbi_t * amhbi_gcd (amhbi_t *num1, amhbi_t *num2);


/*
 * Utility functions
 */

/* Destroys each of the given bigints */
void amhbi_free (int argc, ...);

/* Returns the size in digits of num */
uint64_t amhbi_size (amhbi_t *num);

/* Given a sequence of bigints, returns the size of the largest */
uint64_t amhbi_size_max (int argc, ...);

/* Given a sequence of bigints, returns the size of the smallest */
uint64_t amhbi_size_min (int argc, ...);

/* Returns the sign of num; 0 is positive, 1 is negative */
uint8_t amhbi_sign (amhbi_t *num);

/* Checks if num is positive */
uint8_t amhbi_ispos (amhbi_t *num);

/* Checks if num is negative */
uint8_t amhbi_isneg (amhbi_t *num);

/* Checks if num is even */
uint8_t amhbi_iseven (amhbi_t *num);

/* Checks if num is odd */
uint8_t amhbi_isodd (amhbi_t *num);

/* Checks if num is zero */
uint8_t amhbi_iszero (amhbi_t *num);

/* Checks if num is either 1 or -1 */
uint8_t amhbi_isunit (amhbi_t *num);

/* Returns a copy of the absolute value of num */
amhbi_t * amhbi_abs (amhbi_t *num);

/* Returns a copy of the additive inverse of num */
amhbi_t * amhbi_negate (amhbi_t *num);

/* Decrements num by one */
amhbi_t * amhbi_decr (amhbi_t *num);

/* Increments num by one */
amhbi_t * amhbi_incr (amhbi_t *num);

/* Given a sequence of bigints, returns a copy of the largest */
amhbi_t * amhbi_max (int argc, ...);

/* Given a sequence of bigints, returns a copy of the smallest */
amhbi_t * amhbi_min (int argc, ...);

/* Compare num1 to num2 */
int8_t amhbi_cmp (amhbi_t *num1, amhbi_t *num2);


/*
 * Conversion functions; use these to convert from bigints
 */

/* Returns the string representation of the given bigint */
char * amhbi_to_str (amhbi_t *num);

/* Returns the signed int representation of the given bigint; truncates! */
int64_t amhbi_to_int (amhbi_t *num);

/* Returns the unsigned int representation of the given bigint; truncates! */
uint64_t amhbi_to_uint (amhbi_t *num);


/*
 * Helper functions
 */

/* Returns an empty bigint of the given length */
static amhbi_t * amhbi_init_empty (uint64_t length);

/* Trims leading zeros from num */
static amhbi_t * amhbi_trim (amhbi_t *num);

/* Splits num at the given index */
static amhbi_t ** amhbi_split (amhbi_t *num, uint64_t i);

/* Split used by fft multiplication */
static int64_t * amhbi_split_ntt (amhbi_t *num, uint64_t n);

/* Concatenate a sequence of bigints */
static amhbi_t * amhbi_concat (int argc, ...);

/* Multiply two numbers together using long multiplication */
static amhbi_t * amhbi_mult_long (amhbi_t *num1, amhbi_t *num2);

/* Multiply two numbers together using the Karatsuba algorithm */
static amhbi_t * amhbi_mult_karatsuba (amhbi_t *num1, amhbi_t *num2);

/* Multiply two numbers together using FFTs */
static amhbi_t * amhbi_mult_fft (amhbi_t *num1, amhbi_t *num2);

/* Number-theoretic transform */
static int64_t * amhbi_fft (int64_t *a, long double w, uint64_t n, uint64_t m);

/* Inverse number-theoretic transform */
static int64_t * amhbi_ifft (int64_t *a, long double w, uint64_t n, uint64_t m);

/* Modular exponentiation */
static int64_t amhbi_ifft_pow (uint64_t n, int64_t p, uint64_t m);

/* Divide num1 by num2; return quotient and remainder */
static amhbi_t ** amhbi_div (amhbi_t *num1, amhbi_t *num2);

/* Divide by iterated subtraction; return the quotient */
static amhbi_t * amhbi_quo_slow (amhbi_t *num1, amhbi_t *num2);

/* Divide by iterated subtraction; return the remainder */
static amhbi_t * amhbi_rem_slow (amhbi_t *num1, amhbi_t *num2);


#endif
