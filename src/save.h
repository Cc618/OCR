#ifndef SAVE_H
#define SAVE_H

#include <stdlib.h>
#include "matrix.h"

void matrixSave(char name[], const Matrix *m);

Matrix *matrixLoad(char name[]);

#endif
