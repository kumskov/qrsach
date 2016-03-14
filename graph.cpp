#include "graph.hpp"


//##################################################//
//													//
//						Graph						//
//													//
//##################################################//

Graph::Graph()
{
	//_ingraph = std::vector<Package>();
}

int Graph::getAmount() const
{
	return _ingraph.size();
}

void Graph::buildGraph(Container src)
{
	if (src.size() == 0)
		return;

	for (int i = 0; i < src.size(); ++i)
	{
		_ingraph.push_back(GraphNode(src[i]));
	}

	fillRequires();
	fillProvidesFor();
}


int Graph::findNodeProviding(std::string pkg) const
{
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		Package grpkg = _ingraph[i]._pkg;
		//std::cout << grpkg.provides(pkg) << std::endl;
		if (grpkg.provides(pkg))
		{
			//std::cout << "GRAPH PROVIDES TRUE:" << grpkg.getDesc() << " - " << pkg << std::endl;
			return i;
		}
	}

	return -1;
}

int Graph::findNodeRequiring(std::string pkg) const
{
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		if (_ingraph[i]._pkg.requires(pkg))
		{
			return i;
		}
	}

	return -1;
}

//TODO: Fix with exceptions or something, printing for debug right now
void Graph::fillRequires()
{
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		std::vector<std::string> reqs = _ingraph[i]._pkg.getRequires();
		for (int j = 0; j < reqs.size(); ++j)
		{
			int reqindex = findNodeProviding(reqs[j]);
			if (reqindex == -1)
			{
				_ingraph[i]._brokenDep = true;
				//std::string exmsg = "Graph: failed to find anyone providing <" + reqs[j] + "> for <" + _ingraph[i]._pkg.getName() + ">\n";
				//throw std::runtime_error(exmsg);
				
				//std::cout << "Nobody provides <" << reqs[j] << "> for <" << _ingraph[i]._pkg.getName() << ">\n"; 
			}
			else
			{
				_ingraph[i]._requires.push_back(reqindex);
			}
			//sleep(1);
		}

		if (_ingraph[i]._requires.size() != _ingraph[i]._pkg.getReqAmount())
		{
			//throw std::runtime_error("One of the package requirements could not be resolved propely");
			//std::cout << "Req " << i << ": something went horribly wrong" << std::endl;
		}
		else
		{
			//std::cout << "Req " << i << ": All Good!" << std::endl;
		}
	}
}

//TODO: Fix with exceptions or something, printing for debug right now
void Graph::fillProvidesFor()
{
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		std::vector<std::string> provs = _ingraph[i]._pkg.getProvides();
		for (int j = 0; j < provs.size(); ++j)
		{
			int provindex = findNodeRequiring(provs[j]);
			if (provindex == -1)
			{
				//Nobody cares
				//std::cout << "Failed to find anyone who needs: " << provs[j] << std::endl;
			}
			else
			{
				_ingraph[i]._providesFor.push_back(provindex);
			}
		}
		//std::cout << "Prov " << i << ": done\n";
	}
}


bool Graph::inGraph(const Package cmp) const
{
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		if (cmp == _ingraph[i]._pkg)
		{
			return true;
		}
	}

	return false;
}

const Package& Graph::operator[] (int index) const
{
	if ((index >= _ingraph.size()) ||
		(index < 0))
	{
		throw std::logic_error("Graph: Tried to request invalid index");
	}

	return _ingraph[index]._pkg;
}

/*
void Graph::debugPrintNodeVectors(int index) const
{
	Graph::GraphNode tmp = _ingraph[index];
	int provsize = tmp._providesFor.size();
	int reqsize = tmp._requires.size();

	std::cout << "req:\n";

	for (int i = 0; i < reqsize; ++i)
	{
		std::cout << "\t " << tmp._requires[i] << std::endl;
	}

	std::cout << "prov:\n";

	for (int i = 0; i < provsize; ++i)
	{
		std::cout << "\t " << tmp._providesFor[i] << std::endl;
	}
}
*/

std::string Graph::printInfo(int index) const
{
	if (index >= _ingraph.size())
	{
		throw std::logic_error("Graph: Tried to request index higher than amount of elements");
	}

	//debugPrintNodeVectors(index);

	Graph::GraphNode tmp = _ingraph[index];

	int provsize = tmp._providesFor.size();
	int reqsize = tmp._requires.size();

	std::string ret = "";
	if (tmp._brokenDep)
	{
		ret+= "!!! BROKEN DEPENDENCIES !!!\n\n";
	}
	ret += "PACKAGE NAME: " + tmp._pkg.getName() + "\n";
	ret += "PACKAGE DESCRIPTION: " + tmp._pkg.getDesc() + "\n";
	
	ret += "REQUIRES " + std::to_string(reqsize) + " PACKAGES\n";
	ret += "REQUIRES:\n";
	for (int i = 0; i < reqsize; ++i)
	{
		int pkgindex = _ingraph[index]._requires[i];
		ret += "\t* " + _ingraph[pkgindex]._pkg.getName() + '\n';
		ret += "\t-> " + _ingraph[pkgindex]._pkg.getDesc() + '\n';
	}

	ret += "PROVIDES FOR " + std::to_string(provsize) + " PACKAGES\n";	
	for (int i = 0; i < provsize; ++i)
	{
		ret += "\t* " + _ingraph[_ingraph[index]._providesFor[i]]._pkg.getName() + '\n';
		ret += "\t-> " + _ingraph[_ingraph[index]._providesFor[i]]._pkg.getDesc() + '\n';

	}
	
	return ret;
	
}

void Graph::writeNode(std::ofstream& out, const GraphNode& node) const
{
	out << "node" << std::endl;
	out << node._pkg;
	out << "broken " << node._brokenDep << std::endl;
	out << "reqindex " << node._requires.size() << std::endl;
	for (int i = 0; i < node._requires.size(); ++i)
	{
		out << node._requires[i] << ' ';
	}
	out << std::endl;
	out << "provindex " << node._providesFor.size() << std::endl;
	for (int i = 0; i < node._providesFor.size(); ++i)
	{
		out << node._providesFor[i] << ' ';
	}
	out << std::endl;
}

Graph::GraphNode Graph::readNode(std::ifstream& in) const
{
	Graph::GraphNode ret;
	std::string checker = "";
	in >> checker;
	if (checker != "node")
	{
		throw std::runtime_error("Graph input: Failed to read node header");
	}
	in >> ret._pkg;

	in >> checker;
	if (checker != "broken")
	{
		throw std::runtime_error("Graph input: Failed to read brokendep header");
	}
	in >> ret._brokenDep;

	in >> checker;
	if (checker != "reqindex")
	{
		throw std::runtime_error("Graph input: Failed to read require index header");
	}
	int reqamount = 0;
	in >> reqamount;
	for (int i = 0; i < reqamount; ++i)
	{
		int val = 0;
		in >> val;
		ret._requires.push_back(val);
	}

	in >> checker;
	if (checker != "provindex")
	{
		throw std::runtime_error("Graph input: Failed to read provide index header");
	}
	int provamount = 0;
	in >> provamount;
	for (int i = 0; i < provamount; ++i)
	{
		int val = 0;
		in >> val;
		ret._providesFor.push_back(val);
	}


	return ret;
}

void Graph::save(std::string flname) const
{
	std::ofstream fl;
	fl.open(flname.c_str(), std::ofstream::out);

	fl << "graph" << std::endl;
	fl << "grsize " << _ingraph.size() << std::endl;
	for (int i = 0; i < _ingraph.size(); ++i)
	{
		writeNode(fl, _ingraph[i]);
		//std::cout << "Graph export: written " << i << std::endl;
	}

	fl.close();
}

void Graph::load(std::string flname)
{
	std::ifstream fl;
	fl.open(flname.c_str(), std::ifstream::in);

	std::string checker;
	fl >> checker;
	if (checker != "graph")
	{
		throw std::runtime_error("Graph input: Failed to read graph header");
	}
	fl >> checker;
	if (checker != "grsize")
	{
		throw std::runtime_error("Graph input: Failed to read graph size header");
	}
	int nodeamount = 0;
	fl >> nodeamount;
	for (int i = 0; i < nodeamount; ++i)
	{
		_ingraph.push_back(readNode(fl));
	}

	fl.close();
}