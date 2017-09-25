typedef enum { false, true } bool;
typedef unsigned int u_int;

/*
 * Function: randint
 * --------------------------------------------------------
 * Generates a random unsigned integer n, such that
 * a <= n < b, using C random generator.
 *
 * @args  a :  the lower bound (inclusive)
 * @args  b :  the upper bound (exclusive)
 *
 * @return  A number between a (inclusive) and b(exclusive)
 */
u_int randint(u_int, u_int);

/*
 * Function: event
 * --------------------------------------------------------
 * Runs a probabilistic event and returns true on sucess,
 * false on fail.
 *
 * @args  probability :  the prob. of the event (from 0 to 1)
 *
 * @return  a boolean representing sucess of fail
 */
bool event(double);
