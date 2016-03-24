#include "directorymr.hh"
#include "../messages/idatainfo.hh"
#include "../messages/igroupinfo.hh"
#include "../messages/iblockinfo.hh"
#include "../messages/idatainforequest.hh"
#include "../messages/igroupinforequest.hh"
#include "../messages/iblockinforequest.hh"
#include "../messages/idatainsert.hh"
#include "../messages/igroupinsert.hh"
#include "../messages/iblockinsert.hh"
#include "../common/context.hh"

namespace eclipse {

void DirectoryMR::init_db() {
  Context con;
  open_db();
  // Create SQL statement of IData
  sprintf(sql, "CREATE TABLE idata_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      num_reducer    INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("idata_table created successfully\n");
  }
  // Create SQL statement of IGroup
  sprintf(sql, "CREATE TABLE igroup_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      num_block      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("igroup_table created successfully\n");
  }
  // Create SQL statement of IBlock
  sprintf(sql, "CREATE TABLE iblock_table( \
      job_id         INT       NOT NULL, \
      map_id         INT       NOT NULL, \
      reducer_id     INT       NOT NULL, \
      block_seq      INT       NOT NULL, \
      PRIMARY KEY (job_id, map_id, reducer_id, block_seq));"); 
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("iblock_table created successfully\n");
  }
  sqlite3_close(db);
}

void DirectoryMR::insert_idata_metadata(IDataInsert idata_insert) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO idata_table (\
      job_id, map_id, num_reducer) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      idata_insert.job_id, idata_insert.map_id, idata_insert.num_reducer);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("idata_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::insert_igroup_metadata(IGroupInsert igroup_insert) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO igroup_table (\
      job_id, map_id, reducer_id, num_block) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      igroup_insert.job_id, igroup_insert.map_id, igroup_insert.reducer_id,
      igroup_insert.num_block);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("igroup_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::insert_iblock_metadata(IBlockInsert iblock_insert) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "INSERT INTO iblock_table (\
      job_id, map_id, reducer_id, block_seq) \
      VALUES (%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ");",
      iblock_insert.job_id, iblock_insert.map_id, iblock_insert.reducer_id,
      iblock_insert.block_seq);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("iblock_metadata inserted successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

int DirectoryMR::idata_callback(void *idata_info, int argc, char **argv,
    char **azColName) {
  auto idata = reinterpret_cast<IDataInfo*>(idata_info);
  idata->job_id = atoi(argv[0]);
  idata->map_id = atoi(argv[1]);
  idata->num_reducer = atoi(argv[2]);
  return 0;
}
int DirectoryMR::igroup_callback(void *igroup_info, int argc, char **argv,
    char **azColName) {
  auto igroup = reinterpret_cast<IGroupInfo*>(igroup_info);
  igroup->job_id = atoi(argv[0]);
  igroup->map_id = atoi(argv[1]);
  igroup->reducer_id = atoi(argv[2]);
  igroup->num_block = atoi(argv[3]);
  return 0;
}
int DirectoryMR::iblock_callback(void *iblock_info, int argc, char **argv,
    char **azColName) {
  auto iblock = reinterpret_cast<IBlockInfo*>(iblock_info);
  iblock->job_id = atoi(argv[0]);
  iblock->map_id = atoi(argv[1]);
  iblock->reducer_id = atoi(argv[2]);
  iblock->block_seq = atoi(argv[3]);
  return 0;
}
void DirectoryMR::select_idata_metadata(uint32_t job_id, uint32_t map_id,
    IDataInfo *idata_info) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from idata_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 ";", job_id, map_id);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, idata_callback, (void*)idata_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("idata_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::select_igroup_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, IGroupInfo *igroup_info) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from igroup_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 ";", job_id, map_id,
      reducer_id);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, igroup_callback, (void*)igroup_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("igroup_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}
void DirectoryMR::select_iblock_metadata(uint32_t job_id, uint32_t map_id,
    uint32_t reducer_id, uint32_t block_seq, IBlockInfo *iblock_info) {
  Context con;
  // Open database
  open_db();
  // Create sql statement
  sprintf(sql, "SELECT * from iblock_table where job_id=%" PRIu32 " and \
      map_id=%" PRIu32 " and reducer_id=%" PRIu32 " and block_seq=%" PRIu32 ";",
      job_id, map_id, reducer_id, block_seq);
  // Execute SQL statement
  rc = sqlite3_exec(db, sql, iblock_callback, (void*)iblock_info, &zErrMsg);
  if(rc != SQLITE_OK) {
    con.logger->error("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    con.logger->info("iblock_metadata selected successfully\n");
  }
  // Close Database
  sqlite3_close(db);
}

}  // namespace eclipse
