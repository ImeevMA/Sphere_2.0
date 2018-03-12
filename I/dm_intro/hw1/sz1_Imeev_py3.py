import numpy as np

# Chess board
def t1(n = 7):
    board = np.zeros((n, n), dtype=np.int16)
    board[::2,::2] = 1
    board[1::2,1::2] = 1
    return board

    # (np.arange(1, n * n + 1) % 2).reshape((n, n))
    # return np.tile(quadro, [4, 4])

# Max 3 to null
def t2(n=100):
    vect = np.random.randint(-100, 100, n)
    vect[np.fabs(vect).argsort()[-3:]] = 0
    return vect

# Snake 2
def t3():
    return np.arange(1, 26).reshape((5,5)).T

# Euclidean distance
# m - matrix, for example m = np.random.randn(5,5)
# v - vector, for example v = np.random.randn(5)
def t4(m, v):
    return np.linalg.norm(m - v, axis=1)

# Cosine distance
# m - matrix, for example m = np.random.randn(5,5)
# v - vector, for example v = np.random.randn(5)
def t5(m, v):
    return ((m.T / np.linalg.norm(m, axis = 1)).T * (v / np.linalg.norm(v))).sum(axis = 1)
