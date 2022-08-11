#ifndef __SAVEIO_H__
#define __SAVEIO_H__

int read_save(int *fieldsize, int *bombper, unsigned int *seed, int ***out_masks);
int write_save(int fieldsize, int bombper, int savefield, int seed, int **masks);

#endif // __SAVEIO_H__