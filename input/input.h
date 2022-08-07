#ifndef __INPUT_H__
#define __INPUT_H__

/**
 * @brief Get the value of arrow-key press.
 * Returns 0 when input wasn't an arrow-key.
 *
 * @return The integer value of the arrow-keys.
 */
int get_arrow_keys(int *val);
void clrscrn(void);

#endif // __INPUT_H__