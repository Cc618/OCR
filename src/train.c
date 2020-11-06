#include "train.h"
#include <stdio.h>
#include <math.h>

// Returns a random permutation of [0, n)
static size_t *randRange(size_t n) {
    size_t *data = malloc(sizeof(size_t) * n);

    for (size_t i = 0; i < n; ++i)
        data[i] = i;

    // Permute with the fisher yates algorithm
    for (size_t i = 0; i < n; ++i) {
        // i <= j < n
        size_t j = i + rand() % (n - i);

        // Swap items at i and j
        size_t tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }

    return data;
}

void train(Network *net, Dataset *dataset, size_t epochs, size_t batchSize,
        Preprocessor preprocessor) {
    size_t nBatch = dataset->count / batchSize;

    for (size_t e = 1; e <= epochs; ++e) {
        // Generate all indices of each sample
        size_t *indices = randRange((dataset->count / batchSize) * batchSize);

        // For each batch
        for (size_t batch = 0; batch < nBatch; ++batch) {
            float avgLoss = 0;

            // For each sample
            for (size_t sample = 0; sample < batchSize; ++sample) {
                size_t i = batch * batchSize + sample;

                // TODO : Dataset + preprocessor
                // Matrix *x = xs[sample];
                // Matrix *y = ys[sample];

                // // Accumulate gradients
                // avgLoss += networkBackward(net, x, y);
            }

            avgLoss /= batchSize;

            // Display result
            printf("Epoch %4lu", e);
            printf(", Loss : %.1e\n", avgLoss);

            // Optimize
            networkUpdate(net);
        }

        free(indices);
    }

}
