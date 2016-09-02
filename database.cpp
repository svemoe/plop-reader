#include "belladonna.h"


#define DB_FILE USERDATA TEMPDIR "/belladonna.sqlite3"


static sqlite3 *db;


static int callback_01(void *not_used, int argc, char **argv, char **col_name)
{
	char buffer[128*5];
	char row_buffer[256];

	strcpy(buffer, " > ");
	for(int i=0; i<argc && i<5 ; i++) {
		snprintf(row_buffer, sizeof(row_buffer), "%s=%s, ", col_name[i], argv[i] ? argv[i] : "NULL");
		strcat(buffer, row_buffer);
	}

	if (strlen(buffer) > 3) {
		log_message(buffer);
	}
	return 0;
}


void database_drop()
{
	iv_unlink(DB_FILE);
}


void database_open()
{
	char *err_msg;
	char buffer[2048];

	log_message("Create database + tables...");

	if (sqlite3_open(DB_FILE, &db)) {
		snprintf(buffer, sizeof(buffer), "Fail opening DB : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}

	const char *sql = R"sql(
create table entries (
    id integer primary key,
    remote_id integer not null,
    is_archived integer not null default 0,
    is_starred integer not null default 0,
    title text not null,
    url text not null,
    content text not null
)
)sql";
	if (sqlite3_exec(db, sql, callback_01, 0, &err_msg) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail creating table : %s", err_msg);
		log_message(buffer);
	}
}


void database_write_entry(int remote_id, int is_archived, int is_starred, const char *title, const char *url, const char *content)
{
	char buffer[2048];

	sqlite3_stmt *stmt;
	const char *tail;
	const char *sql = R"sql(
insert into entries (remote_id, is_archived, is_starred, title, url, content)
values (:remote_id, :is_archived, :is_starred, :title, :url, :content)
)sql";
	if (sqlite3_prepare(db, sql, -1, &stmt, &tail) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail preparing : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}

	//snprintf(buffer, sizeof(buffer), "%d - (%c%c) %s (%s)", remote_id, (is_archived ? 'a' : '.'), (is_starred ? '*' : '.'), title, url);
	//log_message(buffer);

	if (sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":remote_id"), remote_id) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	if (sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":is_archived"), is_archived) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	if (sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":is_starred"), is_starred) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	if (sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, ":title"), title, strlen(title), SQLITE_STATIC) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	if (sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, ":url"), url, strlen(url), SQLITE_STATIC) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	if (sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, ":content"), content, strlen(content), SQLITE_STATIC) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail binding : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		snprintf(buffer, sizeof(buffer), "Fail inserting : %s", sqlite3_errmsg(db));
		log_message(buffer);
	}
	sqlite3_finalize(stmt);
}


// debugging function
void database_display_entries()
{
	char *err_msg;
	char buffer[2048];

	log_message("Entries from database:");

	if (sqlite3_exec(db, "select * from entries", callback_01, 0, &err_msg) != SQLITE_OK) {
		snprintf(buffer, sizeof(buffer), "Fail selecting : %s", err_msg);
		log_message(buffer);
	}
}


void database_close()
{
	sqlite3_close(db);
}

