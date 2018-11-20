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

	std::string toString ();
};


class Set
{
public:
	MultiSet *multiSet;
	int dim;
		
	int size = 0;
	std::string name;
	std::list<Element*> elements;
	std::map<std::string,Element*> elementsByName;

	Subset *topSubset = NULL;
	std::list<Subset*> subsets;
	std::map<std::string,Subset*> subsetsByName;
	
	Set (MultiSet *multiset, std::string name);

	Element *getElement (std::string name);
	Subset *getSubset (std::string name);

	std::string toString ();
};


class Subset
{
public:
	Set *set;
	std::string name;
	std::list<Partition*> partitions;

	Subset (Set *set, std::string name);
	virtual ~Subset () {}
	
	void getElements (std::list<Element*> &elements);

	std::string toString ();
};


class BotSubset : public Subset
{
public:
	Element *element;
	BotSubset (Set *set, std::string name, Element *element);
	BotSubset (Set *set, std::string name, std::string elementName);
};


class MidSubset : public Subset
{
public:
	MidSubset (Set *set, std::string name);
};


class TopSubset : public Subset
{
public:
	TopSubset (Set *set, std::string name);
};


class Partition
{
public:
	Subset *subset;
	std::string name;
	std::list<Subset*> subsets;

	Partition (Subset *subset, std::list<Subset*> subsets);
	//Partition (Subset *subset, std::list<std::string> subsetNames);
	
	std::string toString ();
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

	std::string toString ();
};


class MultiSet
{
public:
	std::string name;
	int dim = 0;
	int size = 1;

	std::vector<Set*> sets;
	std::map<std::string,Set*> setsByName;

	std::vector<MultiElement*> multiElements;

	Subset *topMultiSubset = NULL;
	std::list<Subset*> multiSubsets;

	MultiSet (std::string name);

	Set *getSet (std::string name);
	
	void buildMultiElements (double value);
	void setMultiElement (std::string *names, double value);
	MultiElement *getMultiElement (std::string *names);
	
	std::string toString ();
};


class MultiSubset
{
public:
	int dim;
	std::vector<Subset*> subsets;

	double cost = std::numeric_limits<double>::quiet_NaN();
	double optimalCost = std::numeric_limits<double>::quiet_NaN();

	std::list<MultiPartition*> multiPartitions;
	MultiPartition *optimalMultiPartition = NULL;

	// void computeMultiPartitions ();
	// double computeCost ();
	// double computeOptimalCost ();
};


class MultiPartition
{
public:
	int dim;
	std::list<MultiSubset*> multiSubsets;
};
