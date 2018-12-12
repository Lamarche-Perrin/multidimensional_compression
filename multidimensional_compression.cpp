// -*- compile-command: "g++ -Wall -std=c++11 multidimensional_compression.cpp -o multidimensional_compression"; -*-

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

#include <cstdlib>
#include <iostream>
#include <limits>
#include <cmath>

#include "multidimensional_compression.hpp"
	
int main (int argc, char *argv[])
{
    srand (time (NULL));

	// Build multiset
	MultiSet *ABC = new MultiSet ("ABC");

	// Build sets
	Set *A = new Set (ABC, "A");
	Element *a1 = new Element (A, "a1");
	Element *a2 = new Element (A, "a2");
	Element *a3 = new Element (A, "a3");
	Element *a4 = new Element (A, "a4");

	Set *B = new Set (ABC, "B");
	Element *b1 = new Element (B, "b1");
	Element *b2 = new Element (B, "b2");
	Element *b3 = new Element (B, "b3");
	
	Set *C = new Set (ABC, "C");
	Element *c1 = new Element (C, "c1");
	Element *c2 = new Element (C, "c2");

	// Build susbets
	Subset *A1 = new Subset (A, "A1", a1);
	Subset *A2 = new Subset (A, "A2", a2);
	Subset *A3 = new Subset (A, "A3", a3);
	Subset *A4 = new Subset (A, "A4", a4);
	
	Subset *A12 = new Subset (A, "A12");
	new Partition (A12, {A1, A2});

	Subset *A34 = new Subset (A, "A34");
	new Partition (A34, {A3, A4});

	Subset *A1234 = new Subset (A, "A1234", true);
	new Partition (A1234, {A12, A34});

	
	Subset *B1 = new Subset (B, "B1", b1);
	Subset *B2 = new Subset (B, "B2", b2);
	Subset *B3 = new Subset (B, "B3", b3);
	
	Subset *B12 = new Subset (B, "B12");
	new Partition (B12, {B1, B2});

	Subset *B23 = new Subset (B, "B23");
	new Partition (B23, {B2, B3});
	
	Subset *B123 = new Subset (B, "B123", true);
	new Partition (B123, {B1, B23});
	new Partition (B123, {B12, B3});


	Subset *C1 = new Subset (C, "C1", c1);
	Subset *C2 = new Subset (C, "C2", c2);

	Subset *C12 = new Subset (C, "C12", true);
	new Partition (C12, {C1, C2});


	// Fill multiset
	ABC->buildMultiElements ();
	
	std::string names [3] = {"a3","b2","c1"};
	ABC->setMultiElement (names, 2);

	ABC->buildMultiSubsets ();
	
	// Print multiset
	std::cout << ABC->toString (true) << std::endl;

	std::cout << ABC->getMultiPartition(100000.0)->toString() << std::endl;

	return EXIT_SUCCESS;
}



Element::Element (Set *vSet, std::string vName) : set (vSet), name (vName)
{
	id = set->elementNb++;
	set->elements.push_back (this);
	set->elementsByName.insert (std::pair<std::string,Element*> (name, this));
}


std::string Element::toString (bool rec) { return name; }



Set::Set (MultiSet *vMultiSet, std::string vName) : multiSet (vMultiSet), name (vName)
{
	dim = multiSet->dim++;
	multiSet->sets.push_back (this);
	multiSet->setsByName.insert (std::pair<std::string,Set*> (name, this));
}


Element *Set::getElement (int id) { return elements[id]; }

Element *Set::getElement (std::string name) { return elementsByName.at (name); }

Subset *Set::getSubset (int id) { return subsets[id]; }

Subset *Set::getSubset (std::string name) { return subsetsByName.at (name); }


std::string Set::toString (bool rec)
{
	std::string str = name + " = {";

	bool first = true;
	for (Element *element : elements) {
		if (!first) { str += ", "; }
		first = false;
		str += element->toString (rec);
	}

	str += "}";

	if (rec) {
		for (Subset *subset : subsets) { str += "\n\t" + subset->toString (rec); }
	}
	
	return str;
}



Subset::Subset (Set *vSet, std::string vName, bool vTop) : set (vSet), name (vName), top (vTop)
{
	id = set->subsetNb++;
	set->subsets.push_back (this);
	set->subsetsByName.insert (std::pair<std::string,Subset*> (name, this));
}

Subset::Subset (Set *vSet, std::string vName, Element *vElement, bool vTop) : Subset (vSet, vName, vTop) { element = vElement; bot = true; }



void Subset::getElements (std::list<Element*> &elements)
{
	elements.clear();	
	if (bot) { elements.push_back (element); }
	else {
		if (partitions.size() == 0) { std::cerr << "ERROR: No partition found on intermediate subset '" << name << "' of set '" << set->name << "'" << std::endl; return; }
		for (Subset *subset : partitions.front()->subsets) { subset->getElements (elements); }
	}
}


std::string Subset::toString (bool rec)
{
	std::string str = "";

	if (rec) {
		if (bot) { str += "v"; } else { str += "-"; }
		if (top) { str += "^"; } else { str += "-"; }
		str += " ";
	}

	str += name + " = {";

	std::list<Element*> elements;
	getElements (elements);

	bool first = true;
	for (Element *element : elements) {
		if (! first) { str += ", "; }
		first = false;
		str += element->toString (rec);
	}
	str += "}";

	if (rec) {
		for (Partition *partition : partitions) { str += " " + partition->toString (rec); }
	}
	
	return str;
}


Partition::Partition (Subset *vSubset, std::list<Subset*> subsetsToAdd) : subset (vSubset)
{
	subset->partitions.push_back (this);
	for (Subset *subsetToAdd : subsetsToAdd)
		subsets.push_back (subsetToAdd);
}


std::string Partition::toString (bool rec)
{
	std::string str = "{";

	bool first = true;
	for (Subset *subset : subsets) {
		if (! first) { str += ", "; }
		first = false;
		str += subset->name;
	}

	str += "}";

	return str;
}



MultiElement::MultiElement (double vValue) : value (vValue) {}


int MultiElement::addElement (Element *element)
{
	elements.push_back (element);
	return dim++;
}


std::string MultiElement::toString (bool rec)
{
	std::string str = "(";
	for (int d = 0; d < dim; d++) { str += elements[d]->toString (rec) + ", "; }
	str += std::to_string (value) + ")";
	return str;
}



MultiSet::MultiSet (std::string vName) : name (vName) {}


Set *MultiSet::getSet (std::string name) { return setsByName.at (name); }


void MultiSet::buildMultiElements ()
{
	multiElementNb = 1;
	for (int d = 0; d < dim; d++) multiElementNb *= sets[d]->elementNb;
		
	multiElements.clear ();
	multiElements.reserve (multiElementNb);

	std::vector<std::vector<Element*>::iterator> elementIterators;
	elementIterators.reserve (dim);
	
	for (int d = 0; d < dim; d++) elementIterators[d] = sets[d]->elements.begin();

	int id = 0;
	bool stop = false;
	do {
		MultiElement *multiElement = new MultiElement (0);
		for (int d = 0; d < dim; d++) multiElement->addElement (* (elementIterators[d]));
		multiElement->id = id++;
		multiElement->multiSet = this;
				
		multiElements.push_back (multiElement);
				
		stop = true;
		for (int d = 0; d < dim; d++) {
			elementIterators[d]++;
			if (elementIterators[d] != sets[d]->elements.end()) { stop = false; d = dim; }
			else { elementIterators[d] = sets[d]->elements.begin(); }
		}
	} while (! stop);
}


MultiElement *MultiSet::getMultiElement (std::list<Element*>::iterator *elementIterators) {
	int id = 0;
	for (int d = dim-1; d >= 0; d--) {
		id *= sets[d]->elementNb;
		id += (*elementIterators[d])->id;
	}
	return multiElements[id];
}


MultiElement *MultiSet::getMultiElement (std::string *names) {
	int id = 0;
	for (int d = dim-1; d >= 0; d--) {
		id *= sets[d]->elementNb;
		id += sets[d]->getElement (names[d])->id;
	}
	return multiElements[id];
}
	

void MultiSet::setMultiElement (std::string *names, double value)
{
	getMultiElement(names)->value = value;
}


void MultiSet::buildMultiSubsets ()
{
	multiSubsetNb = 1;
	for (int d = 0; d < dim; d++) multiSubsetNb *= sets[d]->subsetNb;

	multiSubsets.clear ();
	multiSubsets.reserve (multiSubsetNb);

	std::vector<std::vector<Subset*>::iterator> subsetIterators;
	subsetIterators.reserve (dim);
			
	for (int d = 0; d < dim; d++) subsetIterators[d] = sets[d]->subsets.begin();

	int id = 0;
	bool stop = false;
	do {
		MultiSubset *multiSubset = new MultiSubset (0);
		multiSubset->multiSet = this;

		multiSubset->id = id++;
		multiSubset->top = true;
		multiSubset->bot = true;
		for (int d = 0; d < dim; d++) {
			multiSubset->addSubset (*subsetIterators[d]);
			multiSubset->top = multiSubset->top && (*subsetIterators[d])->top;
			multiSubset->bot = multiSubset->bot && (*subsetIterators[d])->bot;
		}
				
		multiSubsets.push_back (multiSubset);
		if (multiSubset->top) topMultiSubset = multiSubset;
				
		stop = true;
		for (int d = 0; d < dim; d++) {
			subsetIterators[d]++;
			if (subsetIterators[d] != sets[d]->subsets.end()) { stop = false; d = dim; }
			else { subsetIterators[d] = sets[d]->subsets.begin(); }
		}
	} while (! stop);	

	for (MultiSubset *multiSubset : multiSubsets) {
		multiSubset->multiPartitions.clear();

		std::vector<Subset*> buildingSubsets (multiSubset->subsets);
		for (int d = 0; d < multiSubset->dim; d++) {
			for (Partition *partition : multiSubset->subsets[d]->partitions) {
				MultiPartition *multiPartition = new MultiPartition (dim);
				multiSubset->multiPartitions.push_back (multiPartition);
				
				for (Subset *subset : partition->subsets) {
					buildingSubsets[d] = subset;
					multiPartition->multiSubsets.push_back (getMultiSubset (buildingSubsets));
				}
			}
			
			buildingSubsets[d] = multiSubset->subsets[d];
		}
	}

	topMultiSubset->computeCost();
}


MultiSubset *MultiSet::getMultiSubset (std::vector<Subset*> subsets)
{
	int id = 0;
	for (int d = dim-1; d >= 0; d--) {
		id *= sets[d]->subsetNb;
		id += sets[d]->getSubset (subsets[d]->id)->id;
	}
	return multiSubsets[id];
}


MultiPartition *MultiSet::getMultiPartition (double lambda)
{
	topMultiSubset->computeOptimalCost (lambda);

	MultiPartition *multiPartition = new MultiPartition (dim);
	std::list<MultiSubset*> subsetQueue;
	subsetQueue.push_back (topMultiSubset);

	while (! subsetQueue.empty()) {
		MultiSubset *multiSubset = subsetQueue.front();
		subsetQueue.pop_front();

		if (multiSubset->optimalMultiPartition == NULL) { multiPartition->multiSubsets.push_back (multiSubset); }
		else {
			for (MultiSubset *nextMultiSubset : multiSubset->optimalMultiPartition->multiSubsets) { subsetQueue.push_back (nextMultiSubset); }
		}
	}

	return multiPartition;
}



std::string MultiSet::toString (bool rec)
{
	std::string str = "";

	if (rec) {
		for (Set *set : sets) { str += set->toString (rec) + "\n"; }
	}
	
	str += name + " = {\n";

	bool first = true;
	for (MultiElement *multiElement : multiElements) {
		if (! first) { str += ",\n"; }
		first = false;
		str += "\t" + multiElement->toString (rec);
	}

	str += "\n}";

	if (rec) {
		for (MultiSubset *multiSubset : multiSubsets) { str += "\n\t" + multiSubset->toString (rec); }
	}

	return str;
}



int MultiSubset::addSubset (Subset *subset)
{
	subsets.push_back (subset);
	return dim++;
}


void MultiSubset::getMultiElements (std::list<MultiElement*> &multiElements)
{
	multiElements.clear();	

	std::list<Element*> *elementLists = new std::list<Element*> [dim];
	for (int d = 0; d < dim; d++) subsets[d]->getElements (elementLists[d]);
		
	std::list<Element*>::iterator *elementIterators = new std::list<Element*>::iterator [dim];
	for (int d = 0; d < dim; d++) elementIterators[d] = elementLists[d].begin();

	bool stop;
	do {
		multiElements.push_back (multiSet->getMultiElement (elementIterators));

		stop = true;
		for (int d = 0; d < dim; d++) {
			elementIterators[d]++;
			if (elementIterators[d] != elementLists[d].end()) stop = false;
			else elementIterators[d] = elementLists[d].begin();
		}
	} while (! stop);

	delete [] elementLists;
	delete [] elementIterators;
}


void MultiSubset::computeCost ()
{
	if (! std::isnan (cost)) return;

	sumValue = 0;
	sumInfo = 0;
	multiElementNb = 0;
	
	if (multiPartitions.size() > 0) {
		for (MultiPartition *multiPartition : multiPartitions) {	
			for (MultiSubset *multiSubset : multiPartition->multiSubsets) multiSubset->computeCost();
		}
		for (MultiSubset *multiSubset : multiPartitions.front()->multiSubsets) {
			sumValue += multiSubset->sumValue;
			sumInfo += multiSubset->sumInfo;
			multiElementNb += multiSubset->multiElementNb;
		}
	}
	
	else {
		std::list<MultiElement*> multiElements;
		getMultiElements (multiElements);
		for (MultiElement *multiElement : multiElements)
		{
			sumValue += multiElement->value;
			sumInfo += multiElement->value * log2 (multiElement->value);
			multiElementNb++;
		}
	}

	cost = sumInfo - sumValue * log2 (sumValue) + sumValue * log2 (multiElementNb);
}
	
	
void MultiSubset::computeOptimalCost (double lambda)
{
	if (! std::isnan (optimalCost)) return;

	optimalCost = 1 + lambda * cost;
	optimalMultiPartition = NULL;

	for (MultiPartition *multiPartition : multiPartitions) {
		double multiPartitionCost = 0;
		for (MultiSubset *multiSubset : multiPartition->multiSubsets) {
			multiSubset->computeOptimalCost (lambda);
			multiPartitionCost += multiSubset->optimalCost;
		}
		if (multiPartitionCost < optimalCost) {
			optimalCost = multiPartitionCost;
			optimalMultiPartition = multiPartition;
		}
	}
}




MultiSubset::MultiSubset (MultiSet *vMultiSet) : multiSet (vMultiSet) {}


std::string MultiSubset::toString (bool rec)
{
	std::string str = "";

	if (rec) {
		if (bot) { str += "v"; } else { str += "-"; }
		if (top) { str += "^"; } else { str += "-"; }
		str += " ";
	}

	str += "(";

	for (int d = 0; d < dim; d++) {
		str += subsets[d]->name + ", ";
	}
	str += std::to_string (sumValue / multiElementNb) + ")";

	if (rec) {
		for (MultiPartition *multiPartition : multiPartitions) { str += " " + multiPartition->toString (rec); }
	}

	return str;
}


MultiPartition::MultiPartition (int vDim) : dim (vDim) {}


std::string MultiPartition::toString (bool rec)
{
	std::string str = "{";
	
	bool first = true;
	for (MultiSubset *multiSubset : multiSubsets) {
		if (! first) { str += ", "; }
		first = false;
		str += multiSubset->toString ();
	}

	str += "}";

	return str;
}
