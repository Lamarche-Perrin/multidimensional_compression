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

// -*- compile-command: "g++ -Wall -std=c++11 multidimensional_compression.cpp -o multidimensional_compression"; -*-

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
	Subset *A1 = new BotSubset (A, "A1", a1);
	Subset *A2 = new BotSubset (A, "A2", a2);
	Subset *A3 = new BotSubset (A, "A3", a3);
	Subset *A4 = new BotSubset (A, "A4", a4);
	
	Subset *A12 = new MidSubset (A, "A12");
	new Partition (A12, {A1, A2});

	Subset *A34 = new MidSubset (A, "A34");
	new Partition (A34, {A3, A4});

	Subset *A1234 = new TopSubset (A, "A1234");
	new Partition (A1234, {A12, A34});

	
	Subset *B1 = new BotSubset (B, "B1", b1);
	Subset *B2 = new BotSubset (B, "B2", b2);
	Subset *B3 = new BotSubset (B, "B3", b3);
	
	Subset *B12 = new MidSubset (B, "B12");
	new Partition (B12, {B1, B2});

	Subset *B23 = new MidSubset (B, "B23");
	new Partition (B23, {B2, B3});
	
	Subset *B123 = new TopSubset (B, "B123");
	new Partition (B123, {B1, B23});
	new Partition (B123, {B12, B3});


	Subset *C1 = new BotSubset (C, "C1", c1);
	Subset *C2 = new BotSubset (C, "C2", c2);

	Subset *C12 = new TopSubset (C, "C12");
	new Partition (C12, {C1, C2});


	// Fill multiset
	ABC->buildMultiElements (0);
	
	std::string names [3] = {"a3","b2","c1"};
	ABC->setMultiElement (names, 2);

	// Print multiset
	std::cout << ABC->toString() << std::endl;


	return EXIT_SUCCESS;
}



Element::Element (Set *vSet, std::string vName) : set (vSet), name (vName)
{
	id = set->size++;
	set->elements.push_back (this);
	set->elementsByName.insert (std::pair<std::string,Element*> (name, this));
}


std::string Element::toString () { return name; }



Set::Set (MultiSet *vMultiSet, std::string vName) : multiSet (vMultiSet), name (vName)
{
	dim = multiSet->dim++;
	multiSet->sets.push_back (this);
	multiSet->setsByName.insert (std::pair<std::string,Set*> (name, this));
}


Element *Set::getElement (std::string name) { return elementsByName.at (name); }


Subset *Set::getSubset (std::string name) { return subsetsByName.at (name); }


std::string Set::toString ()
{
	std::string str = name + " = {";

	bool first = true;
	for (Element *element : elements) {
		if (!first) { str += ", "; }
		first = false;
		str += element->toString();
	}

	str += "}";

	for (Subset *subset : subsets) { str += "\n\t" + subset->toString(); }
	
	return str;
}



Subset::Subset (Set *vSet, std::string vName) : set (vSet), name (vName)
{
	set->subsets.push_back (this);
	set->subsetsByName.insert (std::pair<std::string,Subset*> (name, this));
}


void Subset::getElements (std::list<Element*> &elements)
{
	if (dynamic_cast<BotSubset*>(this)) {
		elements.push_back (((BotSubset*) this)->element);
	} else {
		if (partitions.size() == 0) { std::cerr << "ERROR: No partition found on intermediate subset '" << name << "' of set '" << set->name << "'" << std::endl; return; }
		for (Subset *subset : partitions.front()->subsets) { subset->getElements (elements); }
	}
}


std::string Subset::toString ()
{
	std::string str = name + " = {";

	std::list<Element*> elements;
	getElements (elements);

	bool first = true;
	for (Element *element : elements) {
		if (! first) { str += ", "; }
		first = false;
		str += element->toString();
	}
	str += "}";

	for (Partition *partition : partitions) {
		str += " {";
		first = true;
		for (Subset *subset : partition->subsets) {
			if (! first) { str += ", "; }
			first = false;
			str += subset->name;
		}
		str += "}";
	}

	return str;
}



BotSubset::BotSubset (Set *vSet, std::string vName, Element *vElement) : Subset (vSet, vName), element (vElement) {}

BotSubset::BotSubset (Set *vSet, std::string vName, std::string vElementName) : Subset (vSet, vName)
{
	element = set->getElement (vElementName);
}

MidSubset::MidSubset (Set *vSet, std::string vName) : Subset (vSet, vName) {}

TopSubset::TopSubset (Set *vSet, std::string vName) : Subset (vSet, vName)
{
	set->topSubset = this;
}



// Partition::Partition (Subset *vSubset, std::list<std::string> subsetNames) : subset (vSubset)
// {
// 	for (std::string subsetName : subsetNames)
// 		subsets.push_back (subset->set->getSubset (subsetName));
// }


Partition::Partition (Subset *vSubset, std::list<Subset*> subsetsToAdd) : subset (vSubset)
{
	subset->partitions.push_back (this);
	for (Subset *subsetToAdd : subsetsToAdd)
		subsets.push_back (subsetToAdd);
}


std::string Partition::toString ()
{
	return name;
}



MultiElement::MultiElement (double vValue) : value (vValue) {}


int MultiElement::addElement (Element *element)
{
	elements.push_back (element);
	return dim++;
}


std::string MultiElement::toString ()
{
	std::string str = "(";
	for (int d = 0; d < dim; d++) { str += elements[d]->toString() + ", "; }
	str += std::to_string (value) + ")";
	return str;
}



MultiSet::MultiSet (std::string vName) : name (vName) {}


Set *MultiSet::getSet (std::string name) { return setsByName.at (name); }


void MultiSet::build ()
{
	multiElements.clear ();
	multiElements.reserve (size);

	std::vector<std::list<Element*>::iterator> elementIterators;
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


MultiElement *MultiSet::getMultiElement (std::string *names) {
	int id = 0;
	for (int d = dim-1; d >= 0; d--) {
		id *= sets[d]->size;
		id += sets[d]->getElement (names[d])->id;
	}
	return multiElements[id];
}
	

void MultiSet::setMultiElement (std::string *names, double value)
{
	getMultiElement(names)->value = value;
}


std::string MultiSet::toString ()
{
	std::string str = "";
	for (Set *set : sets) { str += set->toString() + "\n"; }
	
	str += name + " = {\n";

	bool first = true;
	for (MultiElement *multiElement : multiElements) {
		if (! first) { str += ",\n"; }
		first = false;
		str += "\t" + multiElement->toString();
	}

	str += "\n}";
	return str;
}



void MultiSubset::build ()
	{
		multiPartitions.clear();

		for (int d = 0; d < dim; d++) {
			for (Partition *partition : subsets[d]->partitions) {
				MultiPartition multiPartition = new MultiPartition (dim);
				multiPartitions.push_back (multiPartition);
				
				Subsets **buildingSubsets = new Subsets *[dim];
				std::copy (subsets, subsets + dim, buildingSubsets);
				
				for (Subset *subset : partition) {
					buildingSubsets[d] = subset;
					multiPartition->multiSubsets.push_back (getMultiSubset (buildingSubsets));
				}
			}
		}
	}


// double MultiSubset::computeCost ()
// 	{
// 		if (! std::isnan (cost)) return cost;

// 		cost = 0;
// 		if (multiPartitions.size() > 0) {
// 			for (MultiSubset *multiSubset : multiPartitions.front()) cost += multiSubset->computeCost();
// 		}

// 		else {
// 			std::set<Element*>::iterator *elementIterators = new std::set<Element*>::iterator [dim];
// 			for (int d = 0; d < dim; d++) elementIterators[d] = subsets[d]->elements->begin();
					
// 			do {
// 				Element **buildingElements = new Element *[dim];
// 				for (int d = 0; d < dim; d++) buildingElements[d] = elementIterators[d]->id;
// 				cost += getMultiElement (buildingElements)->cost;

// 				bool stop = true;
// 				for (int d = 0; d < dim; d++) {
// 					elementIterators[d]++;
// 					if (elementIterators[d] != subsets[d]->elements-end()) stop = false;
// 					else elementsIterators[d] = subsets[d]->elements->begin();
// 				}
// 			} while (! stop);
// 		}

// 		return cost;
// 	}
	
	
// double MultiSubset::computeOptimalCost ()
// 	{
// 		if (! std::isnan (optimalCost)) return optimalCost;

// 		optimalCost = cost;
// 		optimalMultiPartition = NULL;

// 		for (MultiPartition *multiPartition : multiPartitions) {
// 			double multiPartitionCost = 0;
// 			for (MultiSubset *multiSubset : multiPartition->multiSubsets)
// 				multiPartitionCost += multiSubset->computeOptimalCost();
// 			if (multiPartitionCost < optimalCost) {
// 				optimalCost = multiPartitionCost;
// 				optimalMultiPartition = multiPartition;
// 			}
// 		}

// 		return optimalCost;
// 	}




