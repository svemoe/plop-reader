#ifndef REPOSITORIES_ENTRY_REPOSITORY_H_
#define REPOSITORIES_ENTRY_REPOSITORY_H_


#include "../database/database.h"
#include "../entities/entry.h"


class EntryRepository
{
public:

	EntryRepository(Database &d) : db(d) {}

	void persist(Entry entry);

private:
	Database &db;

};


#endif /* REPOSITORIES_ENTRY_REPOSITORY_H_ */
