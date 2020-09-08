

A = rand(3,3);
H = A*A';
ishermitian(H);

L = getCholeskyDecomposition(H);

scale = 3;

X_known = rand(3, scale);
B = L*X_known;

X_solved = forwardElimation(L,B);

X_known - X_solved
