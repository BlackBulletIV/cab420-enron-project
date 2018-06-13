from sklearn import svm
from load_set import load_set

print("Loading data")
X, y = load_set()
X_test, y_test = load_set("_test")

C_vals = [1, 10, 50, 100, 500, 1000]

for c in C_vals:
  print("Fitting data")
  clf = svm.LinearSVC(C=c, max_iter=100)
  clf.fit(X[:,:500], y)
  print("C ", c, ": ", clf.score(X_test[:,:500], y_test))


iter_vals = [100, 500, 1000, 5000]

for i in iter_vals:
  print("Fitting data")
  clf = svm.LinearSVC(C=100, max_iter=i)
  clf.fit(X[:,:500], y)
  print("Iter ", i, ": ", clf.score(X_test[:,:500], y_test))

feat_vals = [100, 500, 1000, 2500, 5000]

for feat in feat_vals:
  print("Fitting data")
  clf = svm.LinearSVC(C=100, max_iter=100)
  clf.fit(X[:,:feat], y)
  print("Feat ", feat, ": ", clf.score(X_test[:,:feat], y_test))



