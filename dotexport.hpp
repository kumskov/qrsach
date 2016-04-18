#ifndef __DOTEXPORT_HPP__
#define __DOTEXPORT_HPP__

#include "package.hpp"
#include "graph.hpp"
#include "cyclecontainer.hpp"
#include <fstream>


class DotExporter
{
protected:
	struct Link
	{
		std::string _path;
		int _from;
		int _to;
		std::string _dep;
		bool _loop;
	};

	struct SlotLink
	{
		std::vector<std::string> _paths;
		std::string _slotname;
		int _from;
		std::vector<int> _to;
		std::vector<bool> _loop;
	};

	std::vector<std::string> _names;
	std::vector<std::string> _encodednames;

	std::vector<Link> _paths;
	std::vector<SlotLink> _slotpaths;

	int _intcounter;

	bool _slotIgnore;
	bool _markBroken;

	std::string _fl;

	std::string _graphname;

	int findName(std::string) const;
	std::string generateName(std::string);

	std::vector<int> transformSingleSlot(std::vector< std::vector<int> >) const;
	bool checkSingleSlot(std::vector< std::vector<int> >) const;

	void addPackage(Package);
	void generateLinks(int, std::vector<int>, Graph);
	void generateSlotLinks(int, std::vector< std::vector<int> >, Graph);

public:
	DotExporter();
	DotExporter(std::string);
	DotExporter(std::string, std::string);

	void setFile(std::string);

	void setSlotIgnore(bool);
	bool getSlotIgnore() const;

	void setMarkBroken(bool);
	bool getMarkBroken() const;

	void setName(std::string);
	std::string getName() const;

	void generateFromGraph(Graph);
	void markCycles(CycleContainer);

	void save();

};



#endif