## Overview

CLI client for telegram clustering contest. 

The general idea that lies in the core of my way of solving task is:

- Take pre-trained Word2Vec vocabularies for english and russian
- Cluster it using Metric framework to a big number of classes 
- For each text calculate embeddings: 

  - create zeros vector for text embeddings with size equals to number of clusters in the Word2Vec vocab
  - for each word in text take it cluster's index
  - increment position at found index in the vector for text embeddings
  - in the end we have single vector for each text
- When we have embeddings vectors for text we can cluster it, compare with categories (converted to embeddings as text too), etc

*Futher improvements:*
- Tune hyperparams using supervised learning
- Use multithreading and batch sampling

#### Compile using CMake

*You need Metric, Boost, Lapack and C++17 support to compile.*

_Windows_

```bash
mkdir build
cd build
cmake .. -T llvm -A x64  -DBoost_NAMESPACE="libboost" -DBoost_COMPILER="-vc141" 
```
Then open solution in the Microsoft Visual Studio

_Linux_

Just run cmake
```bash
mkdir build
cd build
cmake ..
make
```