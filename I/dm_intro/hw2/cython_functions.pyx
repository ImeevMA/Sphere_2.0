import numpy as np
cimport numpy as np
import math
import copy


cpdef matrix_multiply(np.ndarray[np.float64_t, ndim=2] X, np.ndarray[np.float64_t, ndim=2] Y):
    """ Matrix multiplication
    Inputs:
      - X: A numpy array of shape (N, M)
      - Y: A numpy array of shape (M, K)
    Output:
      - out: A numpy array of shape (N, K)
    """
    cdef np.ndarray[np.float64_t, ndim=2] res;
    res = np.zeros((X.shape[0], Y.shape[1]), dtype=np.float64)
    for i in range(X.shape[0]):
        for j in range(Y.shape[1]):
            for k in range(X.shape[1]):
                res[i, j] += X[i, k] * Y[k, j]
    return res


cpdef matrix_rowmean(np.ndarray[np.float64_t, ndim=2] X, weights=np.empty(0)):
    """ Calculate mean of each row.
    In case of weights do weighted mean.
    For example, for matrix [[1, 2, 3]] and weights [0, 1, 2]
    weighted mean equals 2.6666 (while ordinary mean equals 2)
    Inputs:
      - X: A numpy array of shape (N, M)
      - weights: A numpy array of shape (M,)
    Output:
      - out: A numpy array of shape (N,)
    """
    cdef np.ndarray[np.float64_t, ndim=1] fastw = weights
    cdef np.ndarray[np.float64_t, ndim=1] res = np.zeros(X.shape[0], dtype=np.float64)
    if not fastw.any():
        fastw = np.ones(X.shape[1], dtype=np.float64)
    weight = 0.
    for k in range(fastw.shape[0]):
        weight = weight + fastw[k]
    for i in range(X.shape[0]):
        for j in range(X.shape[1]):
            res[i] += X[i, j] * fastw[j]
        res[i] = res[i] / weight
    return res


cpdef apply_mean(np.ndarray[np.float64_t, ndim=2] X):
    cdef np.ndarray[np.float64_t, ndim=1] mean = matrix_rowmean(X)
    for i in range(X.shape[0]):
        for j in range(X.shape[1]):
            X[i, j] = X[i, j] - mean[i]
    return X


cpdef apply_std(np.ndarray[np.float64_t, ndim=2] X, zero_mean=True):
    cdef np.ndarray[np.float64_t, ndim=1] std = np.zeros(X.shape[0])
    cdef np.ndarray[np.float64_t, ndim=1] mean = np.zeros(X.shape[0])
    for i in range(X.shape[0]):
        for j in range(X.shape[1]):
            std[i] += X[i, j] * X[i, j]
        if zero_mean:
            std[i] = math.sqrt(std[i] / X.shape[1])
        else:
            mean = matrix_rowmean(X)
            std[i] = math.sqrt(std[i] / X.shape[1] - mean[i] * mean[i])
    for i in range(X.shape[0]):
        for j in range(X.shape[1]):
            X[i, j] = X[i, j] / std[i]
    return X


cpdef apply_top_n(np.ndarray[np.float64_t, ndim=2] X, int top_n):
    cdef np.float64_t tmp
    if top_n < X.shape[1]:
        for i in range(X.shape[0]):
            tmp = sorted(X[i])[-top_n]
            for j in range(X.shape[1]):
                if X[i, j] < tmp:
                    X[i, j] = 0.
    return X


cpdef cosine_similarity(np.ndarray[np.float64_t, ndim=2] X0, top_n=10, with_mean=True, with_std=True):
    """ Calculate cosine similarity between each pair of row.
    1. In case of with_mean: subtract mean of each row from row
    2. In case of with_std: divide each row on it's std
    3. Select top_n best elements in each row or set other to zero.
    4. Compute cosine similarity between each pair of rows.
    Inputs:
      - X: A numpy array of shape (N, M)
      - top_n: int, number of best elements in each row
      - with_mean: bool, in case of subtracting each row's mean
      - with_std: bool, in case of subtracting each row's std
    Output:
      - out: A numpy array of shape (N, N)

    Example (with top_n=1, with_mean=True, with_std=True):
        X = array([[1, 2], [4, 3]])
        after mean and std transform:
        X = array([[-1.,  1.], [ 1., -1.]])
        after top n choice
        X = array([[0.,  1.], [ 1., 0]])
        cosine similarity:
        X = array([[ 1.,  0.], [ 0.,  1.]])

    """
    cdef np.ndarray[np.float64_t, ndim=2] X = copy.copy(X0)
    cdef np.ndarray[np.float64_t, ndim=2] res = np.zeros((X.shape[0], X.shape[0]), dtype=np.float64)
    cdef np.ndarray[np.float64_t, ndim=1] norms = np.zeros(X.shape[0], dtype=np.float64)
    cdef np.float64_t tmp = 0.

    if with_mean:
        X = apply_mean(X)

    if with_std:
        X = apply_std(X, with_mean)

    apply_top_n(X, top_n)

    for i in range(X.shape[0]):
        for j in range(X.shape[1]):
            norms[i] += X[i, j] * X[i, j]
        norms[i] = math.sqrt(norms[i])
    for i in range(1, X.shape[0]):
        for j in range(i):
            tmp = 0.
            for k in range(X.shape[1]):
                tmp += X[i, k] * X[j, k]
            res[i, j] = tmp / norms[i] / norms[j]
            res[j, i] = res[i, j]
        res[i, i] = 1.
    res[0, 0] = 1.
    return res
