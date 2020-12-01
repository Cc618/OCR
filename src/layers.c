#include "layers.h"
#include <math.h>
#include "error.h"
#include "initializer.h"

// --- Layer ---
Matrix *layerForward(Layer *l, const Matrix *x, bool training) {
    return l->forward(l, x, training);
}

Matrix *layerBackward(Layer *l, const Matrix *grad) {
    return l->backward(l, grad);
}

void layerUpdate(Layer *l, Optimizer *o) {
    if (l->update != NULL)
        l->update(l, o);
}

void layerFree(Layer *l) {
    if (l->free)
        l->free(l);

    free(l);
}

void layerSave(Layer *l, SaveContext *ctx) {
    if (l->save != NULL)
        l->save(l, ctx);
}

void layerLoad(Layer *l, SaveContext *ctx) {
    if (l->load != NULL)
        l->load(l, ctx);
}

// --- Dense ---
static Matrix *denseForward(Dense *l, const Matrix *x, bool training) {
    ASSERT(x->rows == l->weight->cols && x->cols == 1,
            "denseForward : Invalid x shape");

    // Update training context
    if (training)
        l->x = matrixCopy(x);

    Matrix *y = matrixDot(l->weight, x);
    matrixAddMat(y, l->bias);

    return y;
}

static Matrix *denseBackward(Dense *l, const Matrix *grad) {
    // dL / dW = (dL/dY) . x^T
    Matrix *gradWeight = matrixOuter(grad, l->x);
    matrixAddMat(l->gradWeight, gradWeight);

    // dL / dB = (dL/dY)
    matrixAddMat(l->gradBias, grad);

    matrixFree(gradWeight);

    // Free training context
    matrixFree(l->x);
    l->x = NULL;

    return matrixDotT(l->weight, grad);
}

static void denseUpdate(Dense *l, Optimizer *o) {
    optimizerUpdate(o, l->weight, l->gradWeight);
    optimizerUpdate(o, l->bias, l->gradBias);
}

static void denseFree(Dense *l) {
    matrixFree(l->weight);
    matrixFree(l->gradWeight);
    matrixFree(l->bias);
    matrixFree(l->gradBias);
}

static void denseSave(Dense *l, SaveContext *ctx) {
    // TODO Raph : Save weight + bias matrices
}

static void denseLoad(Dense *l, SaveContext *ctx) {
    // TODO Raph : Load weight + bias matrices
}

Layer *denseNew(size_t in, size_t out) {
    Dense *l = malloc(sizeof(Dense));

    // Override virtual functions
    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool))denseForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*))denseBackward;
    l->update = (void (*)(Layer *l, Optimizer *o))denseUpdate;
    l->free = (void (*)(Layer *l))denseFree;
    l->save = (void (*)(Layer *l, SaveContext *ctx))denseSave;
    l->load = (void (*)(Layer *l, SaveContext *ctx))denseSave;

    // Init weights and gradients
    l->gradWeight = matrixZero(out, in);
    l->bias = matrixZero(out, 1);
    l->gradBias = matrixZero(out, 1);

    // Xavier initialisation for weight
    l->weight = randNormal(out, in);
    float var = 2.f / (in + out);
    float std = sqrt(var);
    matrixMul(l->weight, std);

    // Init training context
    l->x = NULL;

    return (Layer*)l;
}

// --- Activation ---
static Matrix *activationForward(Activation *l, const Matrix *x,
        bool training) {
    // Update training context
    if (training)
        l->x = matrixCopy(x);

    Matrix *y = matrixCopy(x);
    l->activation(y);

    return y;
}

static Matrix *activationBackward(Activation *l, const Matrix *grad) {
    // dLoss / dX = dY / dX * dLoss / dY
    Matrix *prevGrad = l->x;
    l->prime(prevGrad);
    matrixMulMat(prevGrad, grad);

    // "Free" training context
    // !!! : l->x is not freed since we return it but is nulled
    // This avoid allocating and copying a bunch of memory
    l->x = NULL;

    return prevGrad;
}

static Layer *activationNew(
        void (*activation)(Matrix *x),
        void (*prime)(Matrix *x)) {
    Activation *l = malloc(sizeof(Activation));

    // Set attributes
    l->activation = activation;
    l->prime = prime;

    // Override virtual functions
    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool))activationForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*))activationBackward;
    l->update = NULL;
    l->free = NULL;
    l->save = NULL;
    l->load = NULL;

    // Init training context
    l->x = NULL;

    return (Layer*)l;
}

Layer *sigmoidNew() {
    return activationNew(matrixSigmoid, matrixSigmoidPrime);
}

// --- Softmax ---
static Matrix *softmaxForward(Softmax *l, const Matrix *x,
        bool training) {
    ASSERT(x->cols == 1, "softmaxForward : x must be a vector");

    // y[i] = exp(y[i]) / sum(exp(y[j]))
    Matrix *y = matrixZero(x->rows, x->cols);
    float ratio = 0;
    for (size_t i = 0; i < y->rows; ++i) {
        y->data[i] = exp(x->data[i]);
        ratio += y->data[i];
    }

    for (size_t i = 0; i < y->rows; ++i)
        y->data[i] /= ratio;

    // Update training context
    if (training)
        l->y = matrixCopy(y);

    return y;
}

static Matrix *softmaxBackward(Softmax *l, const Matrix *grad) {
    // dLoss / dX = dY / dX * dLoss / dY
    Matrix *prevGrad = matrixLike(l->y);

    // dLoss / dXi = grad[i] * y[i] * (1 - y[i]) - sum(grad[j] * y[i] * y[j])
    for (size_t i = 0; i < grad->rows; ++i) {
        float result = grad->data[i] * l->y->data[i] * (1 - l->y->data[i]);

        for (size_t j = 0; j < grad->rows; ++j)
            if (i != j)
                result -= grad->data[j] * l->y->data[i] * l->y->data[j];

        prevGrad->data[i] = result;
    }

    // Free training context
    matrixFree(l->y);
    l->y = NULL;

    return prevGrad;
}

Layer *softmaxNew() {
    Softmax *l = malloc(sizeof(Softmax));

    // Override virtual functions
    l->forward = (Matrix *(*)(Layer *l, const Matrix*, bool))softmaxForward;
    l->backward = (Matrix *(*)(Layer *l, const Matrix*))softmaxBackward;
    l->update = NULL;
    l->free = NULL;
    l->save = NULL;
    l->load = NULL;

    // Init training context
    l->y = NULL;

    return (Layer*)l;
}
