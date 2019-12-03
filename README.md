## Overview

CLI client for telegram clustering contest. 

*Made using [Metric](https://github.com/panda-official/metric), Boost, Lapack and C++17.*

The general idea that lies in the core of my way of solving task is:

- Take pre-trained *Word2Vec* vocabularies for english and russian
- Cluster it using [*Metric*](https://github.com/panda-official/metric) framework to a big number of classes 
- For each text calculate embeddings: 

  - create zeros vector for text embeddings with size equals to number of clusters in the *Word2Vec* vocab
  - for each word in text take it cluster's index
  - increment position at found index in the vector for text embeddings
  - in the end we have single vector for each text
- When we have embeddings vectors for text we can cluster it, compare with categories (converted to embeddings as text too), etc

I chose technologies with the following criteria (order is important):
- **speed**
- **accuracy**
- **recall**

***Futher improvements:***
- Tune hyperparams using supervised learning
- Use multithreading and batch sampling
- Parse html to omit tags etc.

## Tasks

#### **languages**

I detect languages by counting relative number of words that can be found in the most frequency 
words vocabulary for each language. 

Here I take random `num_language_samples` samples and looking among top 100 words for each language. 
And if relative number of found words greater than `language_score_min_level` it means that we detected language.

*See `assets/vocabs/top_english_words.voc` and `assets/vocabs/top_russian_words.voc`*

***Futher improvements:***
- Tune params `num_language_samples` and `language_score_min_level`

#### **news**

News is "*What? Where? When?*" text. So this task I can resolve with Name Entities recognition and dates exctracting. 
In this section I extract dates, calculate average and if average date is fresh, that means text is news. 


***Futher improvements:***
- Use NER for found answer for "*What?*" question.
- Calculate entropy for dates using [*Metric*](https://github.com/panda-official/metric) framework.
- Tune param `freshness_days`


#### **categories**

Here I calculate embeddings (described on the top) for the each text, 
calculate embeddings for categories (each category is small text with tags). 
Then using [*Metric*](https://github.com/panda-official/metric) framework I calculate *cosine distance* between text's embeddings and categories embeddings. 
Min distance means that text belongs to that category. 
But if distance more than value from `category_detect_levels` for found category, next category will be checked. 
If all distances more than values from `category_detect_levels` it means that category is "other".

*See `assets/vocabs/english_categories.voc` and `assets/vocabs/russian_categories.voc`*

*See `assets/vocabs/RusVectoresNews-2019-vectores-50000-words-1024-clusters.bin` and 
`assets/vocabs/GoogleNews-vectors-50000-words-1024-clusters.bin`*

***Futher improvements:***
- Tune param `category_detect_levels`


#### **threads**

Here same as above we calculate embeddings for the text and then run clustering over calculated embeddings. 
Title is extracting from html tag. And relevance calculated as closest distance from text embeddings to the cluster's centroid.

***Futher improvements:***
- Tune params `eps` and `minpts`, means distance in the cluster and min points in the cluster. 
- Try to use `Affinity Propagation`
- Try to use `Cosine` distance for clustering


#### **top** 

Here I take result from clustering of texts and sort it by importantcy. 
I suppose that thread is important if it fresh and thread has a lot of publications, i. e. 
thread has a lot of articles incide. Combination of that two params is sort criteria. 

## Tools

Client use predefined and pretrained vocabularies. 

First od all it is: 
- pretrained Word2Vec on english Google news (can be found here: https://code.google.com/archive/p/word2vec/, License: Apache License 2.0)
- pretrained Word2Vec on russian news (can be found here: https://rusvectores.org/ru/models/, License: CC-BY)
- crowdmarked vocabulary for russian morphology (can be found here: http://opencorpora.org/?page=downloads, License: Unknown)

Then vocabs can be cut and processed to use with current client with some tools: 

- ***cut_word2vec*** - convert origonal Word2Vec vocab to short and ready for use in the current client. Takes two argunets: path to the original Word2Vec vocab and the number of words that will be leave in the result vocab.

- ***cluster_word2vec*** - cluster cutted Word2Vec and save. Takes two argunets: path to the cutted Word2Vec vocab and the number of clusters.

- ***convert_tags_corpora*** - convert morphology tags from vocab format to Universal POS. Takes two argunets: path to the morphology vocab and the number the number of words that will be leave in the result vocab. 



## Compile using CMake

*You need [Metric](https://github.com/panda-official/metric), Boost, Lapack and C++17 support to compile.*

_Windows_

```bash
mkdir build
cd build
cmake .. -T llvm -A x64  -DBoost_NAMESPACE="libboost" -DBoost_COMPILER="-vc141"  -DSTATIC_LINKING=false
```
Then open solution in the Microsoft Visual Studio

_Linux_

Just run cmake
```bash
mkdir build
cd build
cmake ..  -DSTATIC_LINKING=true
make
```