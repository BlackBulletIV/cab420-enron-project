from sklearn import svm
from load_set import load_set
import time

print("Loading data")
X, y = load_set()
X_test, y_test = load_set("_test")

iters = [5, 10, 50, 100, 500, 1000]

for i in iters:
  print("Iter", i)
  print("Fitting data")
  clf = svm.LinearSVC(C=10, max_iter=i, tol=1e-8)

  t0 = time.clock()
  clf.fit(X, y)
  t1 = time.clock()
  print("Time:", t1 - t0)

  print("Scoring data")
  print("Score:", clf.score(X_test, y_test))
