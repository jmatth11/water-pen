#ifndef JM_AVR_MATH_H
#define JM_AVR_MATH_H

/**
 * Map a given target value in a range to an output range.
 *
 * @param[in] x The target value.
 * @param [in] in_min The original minimum range value.
 * @param [in] in_max The original maximum range value.
 * @param [in] out_min The output minimum range value.
 * @param [in] out_max The output maximum range value.
 * @return The converted target value.
 */
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif
