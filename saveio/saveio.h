#ifndef __SAVEIO_H__
#define __SAVEIO_H__

int read_save(int *fieldsize, int *bombper, int ***masks);
int write_save(int **masks);

#endif // __SAVEIO_H__