# ResearchPaper---Fairness-in-Unsupervised-Learning
This repository houses a Jupyter Code for problems related to a research papers sighting fairness in unsupervised learning. 
# Fairness in Unsupervised Machine Learning
Advances in machine learning research have resulted in the development of increasingly accurate models. While improving the accuracy is the primary objective of these algorithms,
their use to allocate social goods and opportunities such as access to healthcare and job and
educational opportunities warrants a closer look at the societal impacts of their outcomes.
Recent studies have exposed a discriminatory outlook in the outcomes of these algorithms,
leading to treatment disparity towards individuals belonging to marginalized groups based on
gender and race in real-world applications like automated resume processing, loan application
screening, and criminal risk prediction. Designing fair and accurate machine learning models
is thus an essential and immediate requirement for these algorithms to make a meaningful
real-world impact. To emphasize the importance of fairness in unsupervised learning, we consider the following hypothetical scenario: An employee-friendly company is looking to open branches at
multiple locations across the city and distribute its workforce in these branches to improve
work efficiency and minimize overall travel time to work. The company has employees with
diverse backgrounds based on, for instance, race and gender and does not prefer any group
of employees over other groups based on these attributes. 

The company’s diversity policy dictates hiring a minimum fraction of employees from each group in every branch. Thus, the natural question is: where should the branches be set up to maximize work efficiency, minimize travel time, and maintain diversity. In other words, the problem is to devise an unsupervised learning algorithm for identifying branch locations with the fairness (diversity) constraints applied to each branch. This problem can be naturally formulated as a clustering problem with additional fairness constraints on allocating the data points to the cluster centers.
