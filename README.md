This is a final project for CAB420 at QUT, at least most of it (I didn't include a third classifier which I didn't work on). The project has code for programs which do the following using the Enron email corpus (as a single CSV file):

* Split up the dataset by author. (C)
* Limit the authors to the original set of executives. (Ruby)
* Select training and testing subsets of emails from each author. (C)
* Preprocess the subsets into vectors of TF-IDF scores for the top words in the corpus. (Ruby)
* Build and test naive Bayes and SVM classifiers to predict who wrote the email by its content (or, TF-IDF scores). (Python)

The datasets are not included due to their sheer size.
