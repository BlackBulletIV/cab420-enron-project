from sklearn import naive_bayes
from load_set import load_set
import time

print("Loading data")
X, y = load_set()
X_test, y_test = load_set("_test")

alpha_vals = [1e-10, 0.0001, 0.01, 0.05, 0.1, 0.15, 0.25, 0.5, 0.75, 1]

print("\nMultinomial distribution")

for a in alpha_vals:
  print("Alpha", a)
  print("Fitting data")
  clf = naive_bayes.MultinomialNB(alpha=a)

  t0 = time.clock()
  clf.fit(X, y)
  t1 = time.clock()
  print("Time:", t1 - t0)

  print("Scoring data")
  print("Score:", clf.score(X_test, y_test))


print("\nBernoulli distribution")

for a in alpha_vals:
  print("Alpha", a)
  print("Fitting data")
  clf = naive_bayes.BernoulliNB(alpha=a)

  t0 = time.clock()
  clf.fit(X, y)
  t1 = time.clock()
  print("Time:", t1 - t0)

  print("Scoring data")
  print("Score:", clf.score(X_test, y_test))
