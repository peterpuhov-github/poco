//
// Binding.cpp
//
// $Id: //poco/Main/Data/samples/TypeHandler/src/TypeHandler.cpp#3 $
//
// This sample demonstrates the Data library.
//
// Copyright (c) 2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include "Poco/SharedPtr.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/TypeHandler.h"
#include "Poco/Data/SQLite/Connector.h"
#include <vector>
#include <iostream>


using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;


struct Person
{
	std::string name;
	std::string address;
	int         age;
};


namespace Poco {
namespace Data {


template <>
class TypeHandler<Person>
	/// Defining a specialization of TypeHandler for Person allows us
	/// to use the Person struct in use and into clauses.
{
public:
	static std::size_t size()
	{
		return 3;
	}
	
	static void bind(std::size_t pos, const Person& person, AbstractBinder* pBinder, AbstractBinder::Direction dir)
	{
		TypeHandler<std::string>::bind(pos++, person.name, pBinder, dir);
		TypeHandler<std::string>::bind(pos++, person.address, pBinder, dir);
		TypeHandler<int>::bind(pos++, person.age, pBinder, dir);
	}
	
	static void extract(std::size_t pos, Person& person, const Person& deflt, AbstractExtractor* pExtr)
	{
		TypeHandler<std::string>::extract(pos++, person.name, deflt.name, pExtr);
		TypeHandler<std::string>::extract(pos++, person.address, deflt.address, pExtr);
		TypeHandler<int>::extract(pos++, person.age, deflt.age, pExtr);
	}
	
	static void prepare(std::size_t pos, const Person& person, AbstractPreparator* pPrep)
	{
		TypeHandler<std::string>::prepare(pos++, person.name, pPrep);
		TypeHandler<std::string>::prepare(pos++, person.address, pPrep);
		TypeHandler<int>::prepare(pos++, person.age, pPrep);
	}
};


} } // namespace Poco::Data


int main(int argc, char** argv)
{
	// register SQLite connector
	Poco::Data::SQLite::Connector::registerConnector();
	
	// create a session
	Session session("SQLite", "sample.db");

	// drop sample table, if it exists
	session << "DROP TABLE IF EXISTS Person", now;
	
	// (re)create table
	session << "CREATE TABLE Person (Name VARCHAR(30), Address VARCHAR, Age INTEGER(3))", now;
	
	// insert some rows
	Person person = 
	{
		"Bart Simpson",
		"Springfield",
		12
	};
	
	Statement insert(session);
	insert << "INSERT INTO Person VALUES(?, ?, ?)",
		use(person);
		
	insert.execute();
	
	person.name    = "Lisa Simpson";
	person.address = "Springfield";
	person.age     = 10;
	
	insert.execute();
	
	// a simple query
	Statement select(session);
	select << "SELECT Name, Address, Age FROM Person",
		into(person),
		range(0, 1); //  iterate over result set one row at a time
		
	while (!select.done())
	{
		select.execute();
		std::cout << person.name << " " << person.address << " " << person.age << std::endl;
	}
	
	// another query - store the result in a container
	std::vector<Person> persons;
	session << "SELECT Name, Address, Age FROM Person",
		into(persons),
		now;
		
	for (std::vector<Person>::const_iterator it = persons.begin(); it != persons.end(); ++it)
	{
		std::cout << it->name << " " << it->address << " " << it->age << std::endl;
	}
	
	return 0;
}
