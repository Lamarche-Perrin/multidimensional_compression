/*
 * This file is part of Multidimensional Compression.
 *
 * Multidimensional Compression implements a combinatorial algorithm for the lossy
 * compression of multidimensional datasets by computing optimal rectangular
 * partitions. A trade-off between the size of the resulting partition and
 * the information loss it induces is optimised by dynamic programming. In
 * addition, feasibility constraints can be express on the subsets that might
 * appear in such partitions. For details regarding the formal grounds of this
 * work, please refer to:
 * 
 * Robin Lamarche-Perrin. An Information-theoretic Framework for the Lossy
 * Compression of Link Streams. Research Report, arXiv:1807.06874, July 2018.
 * <https://arxiv.org/abs/1807.06874>
 * 
 * Copyright © 2018 Robin Lamarche-Perrin
 * (<Robin.Lamarche-Perrin@lip6.fr>)
 * 
 * Multidimensional Compression is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 * 
 * Multidimensional Compression is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this
 * program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>
#include <list>
#include <map>

class Element;
class Set;
class Subset;
class Partition;
class MultiElement;
class MultiSet;
class MultiSubset;
class MultiPartition;


class Element
{
public:
	Set *set;
	std::string name;
	int id;

	Element (Set *set, std::string name);

	std::string toString (bool rec = false);
};


class Set
{
public:
	MultiSet *multiSet;
	int dim;
	int elementNb = 0;
	int subsetNb = 0;
		
	std::string name;
	std::vector<Element*> elements;
	std::map<std::string,Element*> elementsByName;

	Subset *topSubset = NULL;
	std::vector<Subset*> subsets;
	std::map<std::string,Subset*> subsetsByName;
	
	Set (MultiSet *multiset, std::string name);

	void setElements (std::string filename);
	
	Element *getElement (int id);
	Element *getElement (std::string name);
	Subset *getSubset (int id);
	Subset *getSubset (std::string name);

	std::string toString (bool rec = false);
};


class Subset
{
public:
	Set *set;
	std::string name;
	int id;

	bool top = false;
	bool bot = false;
	Element *element = NULL;

	std::list<Partition*> partitions;

	Subset (Set *set, std::string name, bool vTop = false);
	Subset (Set *set, std::string name, Element *element, bool vTop = false);
	
	void getElements (std::list<Element*> &elements);

	std::string toString (bool rec = false);
};



class Partition
{
public:
	Subset *subset;
	std::string name;
	std::list<Subset*> subsets;

	Partition (Subset *subset, std::list<Subset*> subsets);
	//Partition (Subset *subset, std::list<std::string> subsetNames);
	
	std::string toString (bool rec = false);
};



class MultiElement
{
public:
	int dim = 0;
	double value;
	std::vector<Element*> elements;

	MultiSet *multiSet = NULL;
	int id = 0;

	MultiElement (double value);

	int addElement (Element *element);

	std::string toString (bool rec = false);
};


class MultiSet
{
public:
	int dim = 0;
	std::string name;
	int multiElementNb = 1;
	int multiSubsetNb = 1;

	std::vector<Set*> sets;
	std::map<std::string,Set*> setsByName;

	std::vector<MultiElement*> multiElements;

	MultiSubset *topMultiSubset = NULL;
	std::vector<MultiSubset*> multiSubsets;

	MultiSet (std::string name);

	void buildMultiElements ();
	void buildMultiSubsets ();

	MultiPartition *getMultiPartition (double lambda);

	Set *getSet (std::string name);

	void setMultiElement (std::string *names, double value);
	void setMultiElements (std::string fileName);
	
	MultiElement *getMultiElement (std::string *names);
	MultiElement *getMultiElement (std::list<Element*>::iterator *elementIterators);

	MultiSubset *getMultiSubset (std::vector<Subset*> subsets);
	
	std::string toString (bool rec = false);
};


class MultiSubset
{
public:
	int id;
	int dim = 0;
	std::vector<Subset*> subsets;
	MultiSet *multiSet;

	bool top = false;
	bool bot = false;

	int multiElementNb = 0;
	double sumValue = std::numeric_limits<double>::quiet_NaN();
	double sumInfo = std::numeric_limits<double>::quiet_NaN();

	double loss = std::numeric_limits<double>::quiet_NaN();
	double cost = std::numeric_limits<double>::quiet_NaN();
	MultiPartition *multiPartition = NULL;

	std::list<MultiPartition*> multiPartitions;

	void computeLoss ();
	void computeCost (double lambda);
	
	MultiSubset (MultiSet *multiSet);

	int addSubset (Subset *subset);
	void getMultiElements (std::list<MultiElement*> &multiElements);

	std::string toString (bool rec = false);
};


class MultiPartition
{
public:
	int dim;
	std::list<MultiSubset*> multiSubsets;

	int size = 0;
	double loss = 0;
	double cost = 0;

	MultiPartition (int dim);

	void addMultiSubset (MultiSubset *multiSubset);
	
	std::string toString (bool rec = false);
};
